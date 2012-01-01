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


#include <stdio.h>
#include "../../../inc/prototype.h"
static void main(loader_args_t * args){
	http_context_t * context = (http_context_t *)((void *)args + sizeof(loader_args_t) - sizeof(http_context_t));
	dprintf(args->out_body, "<html><body><h3>Simple program that shows how to implement the prototype and redirect input to the socket stream.</h3>");
	dprintf(args->out_body, "<br><br>Example that retrieves information from the HTTP request:<br></body></html>");
	int i;
	for(i = 0; i < __MAX_HEADERS_; ++i){
		if(*(context->request_headers_f[i]) && *(context->request_headers_v[i]))
			dprintf(args->out_body, "<br>%s - %s", context->request_headers_f[i], context->request_headers_v[i]);
	}
	args->return_code = 200;
}


void get(loader_args_t * args){
	main(args);
}
void head(loader_args_t * args){
	main(args);
}
void post(loader_args_t * args){
	main(args);
}
void unknown(loader_args_t * args){
	main(args);
}
