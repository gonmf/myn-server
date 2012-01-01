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
	Socket transport layer
*/
int synchronous_receive(int ifd, char * buffer, size_t max_len){
	struct pollfd arr_fds[1];
	arr_fds[0].fd = ifd;
	arr_fds[0].events = POLLIN | POLLPRI | POLLRDHUP;
	arr_fds[0].revents = 0;
	if(poll(arr_fds, 1, __MAX_MSEC_ELAPSED_) <= 0)
		return -1;
	return read(ifd, buffer, max_len);
}
int synchronous_send(int ofd, const char * buffer, size_t len){
	struct pollfd arr_fds[1];
	arr_fds[0].fd = ofd;
	arr_fds[0].events = POLLOUT | POLLWRBAND | POLLRDHUP;
	arr_fds[0].revents = 0;
	if(poll(arr_fds, 1, __MAX_MSEC_ELAPSED_) <= 0)
		return -1;
	int written = write(ofd, buffer, len);
	if(written < len)
		if(synchronous_send(ofd, buffer, len - written) < 0)
			return -1;
	return len;
}
int synchronous_send_fd(int socket, int file, int t_id){
	file_stats stats;
	file_stat(file, &stats);
	int siz = file_stat_siz(&stats);
	int init_siz = siz;
	char * buf = memory(t_id);
	int read = 0;
	int not_first = 0;
	while(siz){
		if(not_first){
			struct pollfd arr_fds[1];
			arr_fds[0].fd = socket;
			arr_fds[0].events = POLLOUT | POLLWRBAND | POLLRDHUP;
			arr_fds[0].revents = 0;
			if(poll(arr_fds, 1, __MAX_MSEC_ELAPSED_) <= 0){
				release(t_id, buf);
				return -1;
			}
			int written = write(socket, buf, read);
			if(written < read){
				release(t_id, buf);
				return -1;
			}
			siz -= read;
		}else
			not_first = 1;
		if(siz)
			if((read = synchronous_receive(file, buf, ((siz < STREAM_BUF_SIZ) ? siz : STREAM_BUF_SIZ))) < 0){
				release(t_id, buf);
				return -1;
			}
	}
	release(t_id, buf);
	return init_siz;
}
