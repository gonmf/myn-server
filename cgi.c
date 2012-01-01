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


/*
	Must write output to FILE * to_write, global variables accessible through
	int t_id. Do not close to_write.
*/
static int (* cgi_modules[__MAX_MODULES_])(int, char *, loader_args_t *);
void load_cgi_modules(){
	int i;
	for(i = 0; i < __MAX_MODULES_; ++i)
		cgi_modules[i] = NULL;
	DIR * dir_entry = opendir(FOLDER_CGI_MODULES);
	if(dir_entry == NULL){
		fprintf(_out, "Error reading CGI modules directory.\n");
		return;
	}
	struct dirent * de;
	char * buf = (char *)alloc(STRING_BUF_SIZ);
	i = 0;
	while((i < __MAX_MODULES_) && (de = readdir(dir_entry)) != NULL){
		if(*(de->d_name) == '.')
			continue;
		sprintf(buf, "%s%s", FOLDER_CGI_MODULES, de->d_name);
		void * lib_handle = dll_open(buf, RTLD_NOW);
		if(lib_handle == NULL)
			continue;
		dll_error();
		int (* entry_point)(int, char *, loader_args_t *) = dll_sym(lib_handle, "cgi_main");
		char * s;
		if((s = dll_error()) != NULL){
			dll_close(lib_handle);
			fprintf(_out, "CGI warning: %s\n", s);
			continue;
		}
		cgi_modules[i++] = entry_point;
		fprintf(_out, "CGI Module found: %s\n", buf);
	}
	closedir(dir_entry);
	free(buf);
	if(i == 1)
		fprintf(_out, "%d CGI module loaded.\n", i);
	else
		fprintf(_out, "%d CGI modules loaded.\n", i);
}
int cgi_file(int to_write /* file descriptor */, http_context_t * ctx, int t_id){
	int i = 0;
	while((i < __MAX_MODULES_) && (cgi_modules[i] != NULL) && (cgi_modules[i](to_write, ctx->uri, &ctx->base) == __CGI_MODULE_DONT_MATCH_))
		++i;
	return ((i == __MAX_MODULES_) || (cgi_modules[i] == NULL)) ? __CGI_MODULE_DONT_MATCH_ : __CGI_MODULE_MATCH_;
}
