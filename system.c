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

#ifdef _GNU_SOURCE

void abort(){
	exit(EXIT_FAILURE);
}
void abort_m(char * str){
	char tmpbuf[STRING_BUF_SIZ];
	time_t t = time(NULL);
	struct tm * tmp = localtime(&t);
	strftime(tmpbuf, STRING_BUF_SIZ, "(%a, %d %b %Y %T GMT)", tmp);
	fprintf(_out, "\n%s\n%s\n\n", tmpbuf, str);
	abort();
}

void yield(){
	while(sched_yield());
}

void posix_as_init(posix_cb_t * pct, const int fd, void * buf, const unsigned int nbytes, const unsigned int offset){
	memset(pct, 0, sizeof(posix_cb_t));
	pct->aio_fildes = fd;
	pct->aio_buf = buf;
	pct->aio_nbytes = nbytes;
	pct->aio_offset = offset;
}

int file_stat_siz(file_stats * fs){
	return fs->st_size;
}

void server_socket_init(struct sockaddr_in * server_addr, int ms, int af, int inaddr, int port){
     memset((char *)server_addr, 0, ms);
     server_addr->sin_family = af;
     server_addr->sin_addr.s_addr = inaddr;
	server_addr->sin_port = port;
}
#if DIRENT_LISTING
static void htmlink(char * buf, char * dir_name, char * entry_name){
	if(strlen(dir_name) == 0 || ((strlen(dir_name) == 1) && (*dir_name == '/'))){
		strcat(buf, "  <a href=\"");
		strcat(buf, entry_name);
		strcat(buf, "\">");
		strcat(buf, entry_name);
		strcat(buf, "</a>\n");
		return;
	}
	if(*dir_name == '/')
		strcat(buf, "  <a href=\".");
	else
		strcat(buf, "  <a href=\"./");
	strcat(buf, dir_name);
	strcat(buf, "/");
	strcat(buf, entry_name);
	strcat(buf, "\">");
	strcat(buf, entry_name);
	strcat(buf, "</a>\n");
}
void dir_list(int t_id, char * dir_name, char * data){
	if(vol_mem(t_id) < 2)
		abort_m("Out of memory directory listing.");
	int count;
	char * tmp_last = data;
	char * tmp_buf = tmp_last;
	char * tmp1 = vemory(t_id);
	*tmp_buf = 0;
	strcat(tmp_buf, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\"><html><head><title>");
	strcat(tmp_buf, dir_name);
	strcat(tmp_buf, "</title></head><body><h1>Index of ");
	strcat(tmp_buf, dir_name);
	strcat(tmp_buf, "</h1><pre>  <a href=\"..\">..</a>\n");
	tmp_buf = vemory(t_id);
	*tmp_buf = 0;
	strcat(tmp_buf, "<hr>Static\n");
	sprintf(tmp1, "./%s%s", FOLDER_STATIC, dir_name);
	dir_handle dp;
	dirnode * ep0;
	dp = open_dir(tmp1);
	if(dp != NULL){
		count = 0;
		while((ep0 = read_dir(dp))){
			if(*(ep0->d_name) == '.')
				continue;
			sprintf(tmp1, "./%s%s/%s", FOLDER_STATIC, dir_name, ep0->d_name);
			file_stats s;
			if(path_stat(tmp1, &s) == 0)
				if(s.st_mode & S_IFDIR){
					if(strstr(tmp_last, ep0->d_name) == NULL)
						htmlink(tmp_last, dir_name, ep0->d_name);
					continue;
				}
			++count;
			htmlink(tmp_buf, dir_name, ep0->d_name);
		}
		close_dir(dp);
		if(!count)
			strcat(tmp_buf, "  (empty)\n");
	}else
		strcat(tmp_buf, "  (not found)");
	strcat(tmp_buf, "<hr>CGI\n");
	sprintf(tmp1, "./%s%s", FOLDER_CGI, dir_name);
	dirnode * ep1;
	dp = open_dir(tmp1);
	if(dp != NULL){
		count = 0;
		while((ep1 = read_dir(dp))){
			if(*(ep1->d_name) == '.')
				continue;
			sprintf(tmp1, "./%s%s/%s", FOLDER_CGI, dir_name, ep1->d_name);
			file_stats s;
			if(path_stat(tmp1, &s) == 0)
				if(s.st_mode & S_IFDIR){
					if(strstr(tmp_last, ep1->d_name) == NULL)
						htmlink(tmp_last, dir_name, ep1->d_name);
					continue;
				}
			++count;
			htmlink(tmp_buf, dir_name, ep1->d_name);
		}
		close_dir(dp);
		if(!count)
			strcat(tmp_buf, "  (empty)\n");
	}else
		strcat(tmp_buf, "  (not found)");
	strcat(tmp_buf, "<hr>Compiled\n");
	sprintf(tmp1, "./%s%s", FOLDER_COMPILED, dir_name);
	dirnode * ep2;
	dp = open_dir(tmp1);
	if(dp != NULL){
	 	count = 0;
		while((ep2 = read_dir(dp))){
			if(*(ep2->d_name) == '.')
				continue;
			sprintf(tmp1, "./%s%s/%s", FOLDER_COMPILED, dir_name, ep2->d_name);
			file_stats s;
			if(path_stat(tmp1, &s) == 0)
				if(s.st_mode & S_IFDIR){
					if(strstr(tmp_last, ep2->d_name) == NULL)
						htmlink(tmp_last, dir_name, ep2->d_name);
					continue;
				}
			++count;
			htmlink(tmp_buf, dir_name, ep2->d_name);
		}
		close_dir(dp);
		if(!count)
			strcat(tmp_buf, "  (empty)\n");
	}else
		strcat(tmp_buf, "  (not found)");
	strcat(tmp_buf, "<hr>");
	strcat(tmp_buf, "</pre><address>");
	sprintf(tmp1, "%s HTTP/1.1 Server (build %s:%s)", __PROGRAM_NAME_, __PROGRAM_VERSION_, __PROGRAM_DATE_);
	strcat(tmp_buf, tmp1);
	strcat(tmp_buf, "</address></body></html>");
	strcat(tmp_last, tmp_buf);
}
#endif
#endif
