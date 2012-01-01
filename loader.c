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
int virtual_loader(http_context_t * ctx, int t_id, int fd){
	if(fd < 0){
		fprintf(_out, "(slave%d) Loader failed with bad file descriptor.\n", t_id);
		return -1;
	}
	void * lib_handle = dll_open(ctx->uri, RTLD_LAZY);
	if(lib_handle == NULL){
		fprintf(_out, "(slave%d) Loader failed to open %s.\n", t_id, ctx->uri);
		ctx->base.return_code = ServiceUnavailable;
		return -1;
	}
	void * (* entry_point)(void *);
	switch(ctx->base.request_type){
		case request_type_get:
			entry_point = dll_sym(lib_handle, "get");
			break;
		case request_type_head:
			entry_point = dll_sym(lib_handle, "head");
			break;
		case request_type_post:
			entry_point = dll_sym(lib_handle, "post");
			break;
		case request_type_delete:
			entry_point = dll_sym(lib_handle, "delete");
			break;
		default:
			entry_point = dll_sym(lib_handle, "unknown");
	}
	if(entry_point == NULL){
		fprintf(_out, "(slave%d) Loader failed to execute %s.\n", t_id, ctx->uri);
		ctx->base.return_code = ServiceUnavailable;
		return -1;
	}
	ctx->base.out_body = fd;
	entry_point((void *)&ctx->base);
	dll_close(lib_handle);
	return 0;
}
