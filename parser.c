/*
    This file is part of myn.

    myn is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    myn is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with myn.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "inc/common.h"

static void uri_extract(char * dest, char * source, http_context_t * ctx){
	char * saveptr;
	strcpy(dest, strtok_r(source, __SEPARATOR_, &saveptr));
	char * next;
	ctx->base.argc = 0;
	int just_parsed = 1;
	while((next = strtok_r(NULL, __SEPARATOR_, &saveptr)) != NULL){
		if(just_parsed){
			strcpy(ctx->base.argv[ctx->base.argc++], next);
			strcat(dest, __SEPARATOR_);
			just_parsed = 0;
		}else{
			strcat(dest, next);
			just_parsed = 1;
		}
	}
}
static void parse_compiled(http_context_t * ctx, int t_id){
	char * buf = vemory(t_id); // translated
	char * buf2 = vemory(t_id);
	uri_extract(buf, ctx->uri, ctx);
	sprintf(buf2, "./%s%s", FOLDER_COMPILED, buf);
	file_stats s;
	if(path_stat(buf2, &s) == 0){
		if(s.st_mode & S_IFDIR){
			if(buf2[strlen(buf2) - 1] != '/')
	    			strcat(buf2, "/");
			strcat(buf2, DEFAULT_COMPILED_FILE);
			int fd;
			if((fd = open(buf2, O_RDONLY)) != -1){
				close(fd);
				strcpy(ctx->uri, buf2);
				ctx->uri_svcgi = __TRANSLATION_IS_COMPILED_;
			}else{
#if DIRENT_LISTING
				ctx->uri_svcgi = __DIRENT_LISTING_;
#endif
			}
		}else{
			strcpy(ctx->uri, buf2);
			ctx->uri_svcgi = __TRANSLATION_IS_COMPILED_;
		}
	}
}
static void parse_cgi(http_context_t * ctx, int t_id){
	char * buf = vemory(t_id);
	sprintf(buf, "./%s%s", FOLDER_CGI, ctx->uri);
	file_stats s;
	if(path_stat(buf, &s) == 0){
		if(s.st_mode & S_IFDIR){
			if(buf[strlen(buf) - 1] != '/')
	    			strcat(buf, "/");
			strcat(buf, DEFAULT_CGI_FILE);
			int fd;
			if((fd = open(buf, O_RDONLY)) != -1){
				close(fd);
				strcpy(ctx->uri, buf);
				ctx->uri_svcgi = __TRANSLATION_IS_CGI_;
			}else{
#if DIRENT_LISTING
				ctx->uri_svcgi = __DIRENT_LISTING_;
#endif
			}
		}else{
			strcpy(ctx->uri, buf);
			ctx->uri_svcgi = __TRANSLATION_IS_CGI_;
		}
	}
}
static void parse_static(http_context_t * ctx, int t_id){
	char * buf = vemory(t_id);
	sprintf(buf, "./%s%s", FOLDER_STATIC, ctx->uri);
	file_stats s;
	if(path_stat(buf, &s) == 0){
		if(s.st_mode & S_IFDIR){
			if(buf[strlen(buf) - 1] != '/')
	    			strcat(buf, "/");
			strcat(buf, DEFAULT_STATIC_FILE);
			int fd;
			if((fd = open(buf, O_RDONLY)) != -1){
				close(fd);
				strcpy(ctx->uri, buf);
				ctx->uri_svcgi = __TRANSLATION_IS_STATIC_;
			}else{
#if DIRENT_LISTING
				ctx->uri_svcgi = __DIRENT_LISTING_;
#endif
			}
		}else{
			strcpy(ctx->uri, buf);
			ctx->uri_svcgi = __TRANSLATION_IS_STATIC_;
		}
	}
}
// return: HTTP request type
void parse(http_context_t * ctx, int t_id, char * request_data){
	char * raw_data = request_data;
	char * uri_original = memory(t_id);
	char b[10];
	int i = 0;
	while((raw_data[i] != 0) && (raw_data[i] != ' ') && (i < 9)){
		b[i] = raw_data[i];
		++i;
	}
	b[i] = 0;
	char * get = "GET ";
	char * head = "HEAD ";
	char * post = "POST ";
	char * delete = "DELETE ";
	if(strcasecmp(get, b) == 0)
		ctx->base.request_type = request_type_get;
	else
		if(strcasecmp(head, b) == 0)
			ctx->base.request_type = request_type_head;
		else
			if(strcasecmp(post, b) == 0)
				ctx->base.request_type = request_type_post;
			else
				if(strcasecmp(delete, b) == 0)
					ctx->base.request_type = request_type_delete;
	// set request uri before translation
	++i;
	int j = 0;
	while((raw_data[i] != 0) && (raw_data[i] != '\n') && (raw_data[i] != '\r') && (raw_data[i] != ' '))
		ctx->uri[j++] = raw_data[i++];
	ctx->uri[j] = 0;
	fprintf(_out, "(slave%d) Request method: GET   Request URI: %s\n", t_id, ctx->uri);
	strcpy(uri_original, ctx->uri);

	// Filter ../ and ~/
	int len = strlen(uri_original);
	for(i = 2; i < len; ++i)
		if(uri_original[i - 2] == '.' && uri_original[i - 1] == '.' && uri_original[i] == '/'){
			release(t_id, uri_original);
			return;
		}
	// Translate URI
	parse_compiled(ctx, t_id);
	if(ctx->uri_svcgi == __NO_TRANSLATION_ || ctx->uri_svcgi == __DIRENT_LISTING_)
		parse_cgi(ctx, t_id);
	if(ctx->uri_svcgi == __NO_TRANSLATION_ || ctx->uri_svcgi == __DIRENT_LISTING_)
		parse_static(ctx, t_id);
	if(ctx->uri_svcgi == __NO_TRANSLATION_ || ctx->uri_svcgi == __DIRENT_LISTING_){
		release(t_id, uri_original);
		return;
	}
	if(ctx->uri_svcgi == __TRANSLATION_IS_COMPILED_){
		for(i = 1; i < len; ++i)
			if((uri_original[i - 1] == '~' && uri_original[i] == '/') || (uri_original[i - 1] == '?' && uri_original[i] == '?')){
				ctx->uri_svcgi = __NO_TRANSLATION_;
				release(t_id, uri_original);
				return;
			}
	}
	release(t_id, uri_original);
	*ctx->request_body = 0;
	if((ctx->uri_svcgi == __TRANSLATION_IS_COMPILED_) || (ctx->uri_svcgi == __TRANSLATION_IS_CGI_)){
		// Get request body
		i = 3;
		j = 0;
		int copy = 0;
		while(raw_data[i]){
			if(copy){
				ctx->request_body[j] = raw_data[i];
				++i;
				++j;
				continue;
			}
			if((raw_data[i - 3] == '\r' && raw_data[i - 2] == '\n' && raw_data[i - 1] == '\r' && raw_data[i] == '\n') || (raw_data[i - 1] == '\n' && raw_data[i] == '\n'))
				copy = 1;
			++i;
		}
		ctx->request_body[j] = 0;
	}
	
	// Get headers on context structure
	char * raw_data_tmp = vemory(t_id);
	char * saveptr = NULL; // reentrant pointer
	// get headers only on a different buffer
	i = 0;
	j = 0;
	while(raw_data[i]){
		if(raw_data[i] != '\r')
			raw_data_tmp[j++] = raw_data[i];
		++i;
	}
	raw_data_tmp[j] = '\0';
	if(strtok_r(raw_data_tmp, "\n", &saveptr) == NULL){
		ctx->uri_svcgi = __NO_TRANSLATION_;
		return;
	}
	// copy headers to context structure
	i = 0;
	while(i < __MAX_HEADERS_){
		char * n = strtok_r(NULL, ":", &saveptr);
		if(n == NULL || !strlen(n) || strcmp(n, "\n") == 0)
			break;
		char * m = strtok_r(NULL, "\n", &saveptr);
		if(m == NULL || !strlen(m))
			break;
		if((!__FORCE_NO_COMPRESS_) && (strcasecmp("Accept-Encoding", n) == 0) && (strcasestr(m + 1, "gzip") != NULL))
			ctx->compress_mode = __GNU_ZIP_;
		strcpy(ctx->request_headers_f[i], n);
		strcpy(ctx->request_headers_v[i], (m + 1));
		++i;
	}	
	return;
}
