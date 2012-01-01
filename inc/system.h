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
	System dependant definitions	
*/

#ifdef _GNU_SOURCE


void abort();
void abort_m(char * str);

void yield();


#if DIRENT_LISTING
void dir_list(int t_id, char * dir_name, char * data);
#endif

typedef struct aiocb posix_cb_t;
void posix_as_init(posix_cb_t * pct, const int fd, void * buf, const unsigned int nbytes, const unsigned int offset);
#define posix_as_read aio_read
#define posix_as_error aio_error
#define posix_as_return aio_return
typedef pthread_t posix_thread_t;
typedef pthread_mutex_t posix_mutex_t;
#define posix_thread_create pthread_create
#define posix_mutex_init pthread_mutex_init
#define posix_mutex_adquire pthread_mutex_lock
#define posix_mutex_release pthread_mutex_unlock
#define posix_thread_join pthread_join

#define multi_sleep usleep

void server_socket_init(struct sockaddr_in * server_addr, int ms, int af, int inaddr, int port);
#define socket_accept socket
#define ip_to_text inet_ntop
#define server_listen listen
#define server_bind bind

typedef struct stat file_stats;
#define file_stat fstat
#define path_stat stat
int file_stat_siz(file_stats * fs);

#define sys_call system

#define dll_open dlopen
#define dll_error dlerror
#define dll_sym dlsym
#define dll_close dlclose


typedef DIR * dir_handle;
typedef struct dirent dirnode;
#define open_dir opendir
#define read_dir readdir
#define close_dir closedir


#endif
