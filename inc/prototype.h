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

/*
	This header file is shared among the software and CGI/Compiled parts.
*/


// Max number of response and request headers that are exposed to dynamic pages
#define __MAX_HEADERS_ 32
// Separator used to parse requests for compiled resources
#define __SEPARATOR_ "$"
// Number of bytes of a small buffer, used mostly for strings
// This buffers are used for small things such as parameters, arguments and 
// chunks of strings.
#define STRING_BUF_SIZ 4048
// Number of bytes of a bigger buffer, used mostly for I/O.
#define STREAM_BUF_SIZ 16192
// Max arguments a dynamic page may receive (strings)
#define __MAX_ARGS_ 16
// Max number of CGI modules that can be loaded
#define __MAX_MODULES_ 16



// To use in the int request_type field
#define request_type_unknown 0
#define request_type_get 1
#define request_type_post 2
#define request_type_head 3
#define request_type_delete 4

// To use in the int no_body field
#define LDR_SEND_BODY 0
#define LDR_HEADERS_ONLY 1
#define LDR_SEND_ERRORDOC 2

typedef struct __loader_args_{
	int out_body; // OPEN FILE DESCRIPTOR TO OUTPUT FILE
	int return_code; // Valid HTTP/1.1 code
	int request_type; // do not modify
	int argc; // do not modify
	int no_body;
	char argv[__MAX_ARGS_][STRING_BUF_SIZ]; // do not modify
} loader_args_t;

typedef struct __http_context_{
	char uri[STRING_BUF_SIZ]; // do not modify
	int uri_svcgi; // do not modify
	int compress_mode; // do not modify
	char request_headers_f[__MAX_HEADERS_][STRING_BUF_SIZ]; // do not modify
	char request_headers_v[__MAX_HEADERS_][STRING_BUF_SIZ]; // do not modify
	char response_headers_f[__MAX_HEADERS_][STRING_BUF_SIZ];
	char response_headers_v[__MAX_HEADERS_][STRING_BUF_SIZ];
	char request_body[STREAM_BUF_SIZ]; // do not modify
	void * reentrant; // for use in reentrant state maintaining
	loader_args_t base;
} http_context_t;

// To be returned by the cgi_main function
#define __CGI_MODULE_MATCH_ 0
#define __CGI_MODULE_DONT_MATCH_ 1

// CGI Modules must implement this entry point
int cgi_main(FILE * out, char * uri, loader_args_t * args);

