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

static int code_nums[] = {200, 201, 202, 204, 301, 302, 304, 400, 401, 403, 404, 500, 501, 502, 503, 0};
static char * code_msgs[] = {"OK", "Created", "No Content", "Moved Permanently", "Moved Temporarily", "Not Modified", "Bad Request", "Unauthorized", "Forbidden", "Not Found", "Internal Server Error", "Not Implemented", "Bad Gateway", "Service Unavailable"};
static char * code_to_text(int code){
	int i = 0;
	while(code_nums[i]){
		if(code_nums[i] == code)
			return code_msgs[i];
		++i;
	}
	return code_to_text(InternalServerError);
}
static void build_header_string(http_context_t * ctx, int content_size, int t_id, char * tmp_buf){
	char * tmp_buf2 = memory(t_id);
#if __PERFORM_TTFB_TRICK_
	sprintf(tmp_buf, "/1.1 %d %s\n", ctx->base.return_code, code_to_text(ctx->base.return_code));
#else
	sprintf(tmp_buf, "HTTP/1.1 %d %s\n", ctx->base.return_code, code_to_text(ctx->base.return_code));
#endif
	int i;
	int has_len = 0;
	for(i = 0; i < __MAX_HEADERS_; ++i){
		if(*ctx->response_headers_f[i] == 0 || *ctx->response_headers_v[i] == 0)
			continue;
		if(strcmp(ctx->response_headers_f[i], "Content-Length") == 0)
			has_len = 1;
		sprintf(tmp_buf2, "%s: %s\n", ctx->response_headers_f[i], ctx->response_headers_v[i]);
		strcat(tmp_buf, tmp_buf2);
	}
	if(!has_len){
		sprintf(tmp_buf2, "Content-Length: %d\n", content_size);
		strcat(tmp_buf, tmp_buf2);
	}
	if(ctx->compress_mode == __GNU_ZIP_){
		strcat(tmp_buf, "Content-Encoding: gzip\n");
	}
	strcat(tmp_buf, "\n");
	release(t_id, tmp_buf2);
}
static int send_headers(int socket, http_context_t * ctx, int t_id, int len){
	char * head = memory(t_id);
	build_header_string(ctx, len, t_id, head);
	int res = synchronous_send(socket, head, strlen(head));
	release(t_id, head);
	return res;
}
int http_answer(int socket, int htt_mode, void * blob, http_context_t * ctx, int t_id){
	file_stats tmp_stat_buf;
	int result = 0;
	int size;
	int fd;
	char * head;
	char * tmp_buf;
	switch(htt_mode){
		case HTT_HEADERS_ONLY:
			result = send_headers(socket, ctx, t_id, 0);
			if(result >= 0)
				fprintf(_out, "(slave%d) Sent headers only.\n", t_id);
			break;
		case HTT_ERROR_DOCUMENT:
			sprintf(ctx->uri, "%s/%d", FOLDER_ERROR, ctx->base.return_code);
			ctx->compress_mode = __GNU_ZIP_;return http_answer(socket, HTT_SEND_FILE, (void *)open(ctx->uri, O_RDONLY), ctx, t_id);
			break;
		case HTT_SEND_STREAM:
			head = memory(t_id);
			size = strlen((char *)blob);
			build_header_string(ctx, size, t_id, head);
			if(synchronous_send(socket, head, strlen(head)) < 0){
				release(t_id, head);
				result = -1;
				break;
			}
			release(t_id, head);
			if((result = synchronous_send(socket, (char *)blob, size)) >= 0)
				fprintf(_out, "(slave%d) Sent %d bytes (no compression) plus headers.\n", t_id, size);
			break;
		case HTT_SEND_FILE:
			fd = (int)blob;
			if(fd < 0){
				fprintf(_out, "(slave%d) Illegal file descriptor.\n", t_id);
				result = -1;
				break;
			}
			path_stat(ctx->uri, &tmp_stat_buf);
			size = file_stat_siz(&tmp_stat_buf);
			if(size > __MAX_COMPRESS_SIZ_)
				ctx->compress_mode = __NO_COMPRESS_;
			switch(ctx->compress_mode){
				case __GNU_ZIP_:
					close(fd);
					tmp_buf = memory(t_id);
					sprintf(tmp_buf, __GZIP_TMP_, ctx->uri, t_id);
					sys_call(tmp_buf);
					sprintf(tmp_buf, __GZIP_FILE_, t_id);
					path_stat(tmp_buf, &tmp_stat_buf);
					size = file_stat_siz(&tmp_stat_buf);
					head = memory(t_id);
					build_header_string(ctx, size, t_id, head);
					if(synchronous_send(socket, head, strlen(head)) < 0){
						release(t_id, tmp_buf);
						release(t_id, head);
						result = -1;
						goto cleanup_tmp_file;
					}
					release(t_id, head);
					fd = open(tmp_buf, O_RDONLY);
					release(t_id, tmp_buf);
					if(fd < 0){
						fprintf(_out, "(slave%d) Illegal file descriptor (err2).\n", t_id);
						result = -1;
						goto cleanup_tmp_file;
					}
					result = synchronous_send_fd(socket, fd, t_id);
					close(fd);
					if(result >= 0)
						fprintf(_out, "(slave%d) Sent %d bytes (GZIP compression) plus headers.\n", t_id, size);
cleanup_tmp_file:
#if __ERASE_TEMP_FILES_
					tmp_buf = memory(t_id);
					sprintf(tmp_buf, __GZIP_FILE_, t_id);
					unlink(tmp_buf);	
					release(t_id, tmp_buf);
#endif
					break;
				case __NO_COMPRESS_:
					head = memory(t_id);
					build_header_string(ctx, size, t_id, head);
					if(synchronous_send(socket, head, strlen(head)) < 0){
						release(t_id, head);
						result = -1;
						break;
					}
					release(t_id, head);
					result = synchronous_send_fd(socket, fd, t_id);
					close(fd);
					if(result >= 0)
						fprintf(_out, "(slave%d) Sent %d bytes (no compression) plus headers.\n", t_id, size);
			}
	}
	if(result < 0){
		close(socket);
		fprintf(_out, "(slave%d) Connection abruptly closed.\n", t_id);
		return 0;
	}
	return 1;
}
