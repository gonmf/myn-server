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
static void default_response_headers(http_context_t * ctx){
	strcpy(ctx->response_headers_f[0], "Server");
	strcpy(ctx->response_headers_v[0], "myn(1) free software");
	strcpy(ctx->response_headers_f[1], "Allow");
	strcpy(ctx->response_headers_v[1], "GET, HEAD, POST, DELETE");
	strcpy(ctx->response_headers_f[2], "Accept-Ranges");
	strcpy(ctx->response_headers_v[2], "bytes");
	strcpy(ctx->response_headers_f[3], "Cache-Control");
	strcpy(ctx->response_headers_v[3], "no-cache");
	strcpy(ctx->response_headers_f[4], "Date");
	time_t t = time(NULL);
	struct tm * tmp = localtime(&t);
	strftime(ctx->response_headers_v[4], STRING_BUF_SIZ, "%a, %d %b %Y %T GMT", tmp);
	*ctx->response_headers_f[5] = 0;
	*ctx->response_headers_v[5] = 0;
}
void * worker_main(void * arg){
	int t_id = (int)arg;
	int fd;
	char * buf0;
	int keep_alive = 0;
	int request_socket;
	http_context_t ctx;
	fprintf(_out, "(slave%d) Worker thread %d/%d is now active.\n", t_id, t_id + 1, WORKER_THREADS);
	while(1){
		if(!keep_alive)
			while(ring_get(&_conns, (void *)&request_socket));
		keep_alive = 0;
		if(_HALT)
			return NULL;
		// Init HTTP context
		*ctx.uri = 0;
		ctx.uri_svcgi = __NO_TRANSLATION_;
		ctx.compress_mode = __NO_COMPRESS_;
		default_response_headers(&ctx);
		*ctx.request_body = 0;
		ctx.reentrant = _reentrant_mem;
		ctx.base.request_type = request_type_unknown;
		ctx.base.return_code = OK;
		ctx.base.no_body = LDR_SEND_BODY;
		for(int i = 0; i < __MAX_ARGS_; ++i)
			*ctx.base.argv[i] = 0;
		// End of init HTTP context
		char * request_data = memory(t_id);
		if(synchronous_receive(request_socket, request_data, STREAM_BUF_SIZ) <= 0){
			close(request_socket);
			fprintf(_out, "(slave%d) Request failed to be read.\n", t_id);
			release(t_id, request_data);
			continue;
		}
#if __PERFORM_TTFB_TRICK_
		if(forcewrite(request_socket, "HTTP", 4)){
			fprintf(_out, "(slave%d) Write to socket failed on TTFB benchmark optimization.\n", t_id);
			release(t_id, request_data);
			continue;
		}
#endif
		parse(&ctx, t_id, request_data);
		release(t_id, request_data);
		switch(ctx.base.no_body){
			case LDR_HEADERS_ONLY:
				keep_alive = http_answer(request_socket, HTT_HEADERS_ONLY, NULL, &ctx, t_id);
				break;
			case LDR_SEND_ERRORDOC:
				keep_alive = http_answer(request_socket, HTT_ERROR_DOCUMENT, NULL, &ctx, t_id);
				break;
			case LDR_SEND_BODY:
				switch(ctx.uri_svcgi){
					case __TRANSLATION_IS_COMPILED_:
						fprintf(_out, "(slave%d) Translated to %s.\n", t_id, ctx.uri);
						buf0 = memory(t_id);
						sprintf(buf0, VIRTUAL_BUFFER, t_id);
						fd = open(buf0, O_RDWR | O_CREAT, 0xfff);
						if(virtual_loader(&ctx, t_id, fd) < 0){
							ctx.base.return_code = ServiceUnavailable;
							keep_alive = http_answer(request_socket, HTT_ERROR_DOCUMENT, NULL, &ctx, t_id);
							release(t_id, buf0);
							break;						
						}
						strcpy(ctx.uri, buf0);
						release(t_id, buf0);
						if(lseek(fd, 0, SEEK_SET)){
							fprintf(_out, "(slave%d) Error: Compiled object closed output file descriptor.\n", t_id);
							ctx.base.return_code = ServiceUnavailable;
							keep_alive = http_answer(request_socket, HTT_ERROR_DOCUMENT, NULL, &ctx, t_id);
							break;						
						}
						keep_alive = http_answer(request_socket, HTT_SEND_FILE, (void *)fd, &ctx, t_id);
						break;
					case __TRANSLATION_IS_CGI_:
						fprintf(_out, "(slave%d) Translated to %s.\n", t_id, ctx.uri);
						buf0 = memory(t_id);
						sprintf(buf0, VIRTUAL_BUFFER, t_id);
						fd = open(buf0, O_RDWR);
						int success = cgi_file(fd, &ctx, t_id);					
						switch(success){
							case __CGI_MODULE_DONT_MATCH_:
								release(t_id, buf0);
								close(fd);
								fprintf(_out, "(slave%d) No suitable CGI module found.\n", t_id);
								ctx.base.return_code = ServiceUnavailable;
								keep_alive = http_answer(request_socket, HTT_ERROR_DOCUMENT, NULL, &ctx, t_id);
								break;
							case __CGI_MODULE_MATCH_:
								release(t_id, buf0);
								if(lseek(fd, 0, SEEK_SET)){
									fprintf(_out, "(slave%d) Error: CGI script closed output file descriptor.\n", t_id);
									ctx.base.return_code = ServiceUnavailable;
									keep_alive = http_answer(request_socket, HTT_ERROR_DOCUMENT, NULL, &ctx, t_id);
									break;						
								}
								sprintf(ctx.uri, VIRTUAL_BUFFER, t_id);
								keep_alive = http_answer(request_socket, HTT_SEND_FILE, (void *)fd, &ctx, t_id);
						}
						break;
					case __TRANSLATION_IS_STATIC_:
						fprintf(_out, "(slave%d) Translated to %s.\n", t_id, ctx.uri);
						fd = open(ctx.uri, O_RDONLY);
						keep_alive = http_answer(request_socket, HTT_SEND_FILE, (void *)fd, &ctx, t_id);
						break;
					case __DIRENT_LISTING_:
						fprintf(_out, "(slave%d) Index of %s.\n", t_id, ctx.uri);
						char * data = memory(t_id);
						dir_list(t_id, ctx.uri, data);
						ctx.base.return_code = OK;
						keep_alive = http_answer(request_socket, HTT_SEND_STREAM, (void *)data, &ctx, t_id);
						release(t_id, data);
						break;
					case __NO_TRANSLATION_:
						fprintf(_out, "(slave%d) No translation found.\n", t_id);
						ctx.base.return_code = NotFound;
						keep_alive = http_answer(request_socket, HTT_ERROR_DOCUMENT, NULL, &ctx, t_id);
				}
		}
#if __ERASE_TEMP_FILES_
		buf0 = memory(t_id);
		sprintf(buf0, VIRTUAL_BUFFER, t_id);
		unlink(buf0);
		release(t_id, buf0);
#endif
	}
	return NULL;
}
