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
	Use this program to quickly generate ErrorDocument pages updated by
	release.
*/
#include <stdio.h>
#include "../../inc/common.h"
static int code_nums[] = {200, 201, 202, 204, 301, 400, 401, 403, 404, 500, 501, 502, 503, 0};
static char * code_msgs[] = {"OK", "Created", "No Content", "Moved Permanently", "Not Modified", "Bad Request", "Unauthorized", "Forbidden", "Not Found", "Internal Server Error", "Not Implemented", "Bad Gateway", "Service Unavailable"};
static char tmp_buf[STRING_BUF_SIZ];
int main(){
	int i = 0;
	while(code_nums[i]){
		sprintf(tmp_buf, "../../%s/%d", FOLDER_ERROR, code_nums[i]);
		FILE * fout = fopen(tmp_buf, "w");
		if(fout != NULL){
			fprintf(fout, "<html><head><title>%d %s</title><style>body{font-family:\"Verdana\";font-weight:normal;font-size: .7em;color:black;}b {font-family:\"Verdana\";font-weight:bold;color:black;margin-top: -5px}h1{ font-family:\"Verdana\";font-weight:normal;font-size:18pt;color:red }h2{ font-family:\"Verdana\";font-weight:normal;font-size:14pt;color:maroon }</style></head><body bgcolor=\"white\"><span><h1>This is an automated error response<hr width=100%% size=1 color=silver></h1><h2> <i>%d %s.</i> </h2></span><hr width=100%% size=1 color=silver><b>Version Information:</b>&nbsp;<b>%s</b> HTTP/1.1 web server Version:%s <i>http://pwp.net.ipl.pt/alunos.isel/35393/myn.htm</i></font></body></html>", code_nums[i], code_msgs[i], code_nums[i], code_msgs[i], __PROGRAM_NAME_, __PROGRAM_VERSION_);
			fclose(fout);
		}
		++i;
	}
	return 0;
}
