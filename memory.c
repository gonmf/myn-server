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
	Thread safe if t_id is correct
*/
#include "inc/common.h"

static char * _tmp_buf[WORKER_THREADS][__THREAD_BUFFERS_];
static int _tmp_buf_bsy[WORKER_THREADS][__THREAD_BUFFERS_];
static int _tmp_buf_age[WORKER_THREADS][__THREAD_BUFFERS_];
static int _private_state;
void print_memory_state(int t_id){
	fprintf(_out, "[MEMORY FOR THREAD%d]\n", t_id);
	for(int i = 0; i < __THREAD_BUFFERS_; ++i)
		if(_tmp_buf_bsy[t_id][i] == __BUF_MGNT_FRE_)
			fprintf(_out, "\t[BUF%d\tFRE\t%d]\n", i, _tmp_buf_age[t_id][i]);
		else
			if(_tmp_buf_bsy[t_id][i] == __BUF_MGNT_VOL_)
				fprintf(_out, "\t[BUF%d\tVOL\t%d]\n", i, _tmp_buf_age[t_id][i]);
			else
				fprintf(_out, "\t[BUF%d\tBSY\t%d]\n", i, _tmp_buf_age[t_id][i]);
}
int free_mem(int t_id){
	int tot = 0;
	for(int i = 0; i < __THREAD_BUFFERS_; ++i)
		if(_tmp_buf_bsy[t_id][i] == __BUF_MGNT_FRE_)
			++tot;
	return tot;
}
int vol_mem(int t_id){
	int tot = 0;
	for(int i = 0; i < __THREAD_BUFFERS_; ++i)
		if((_tmp_buf_bsy[t_id][i] == __BUF_MGNT_VOL_) || (_tmp_buf_bsy[t_id][i] == __BUF_MGNT_FRE_))
			++tot;
	return tot;
}
void memmngr_init(int t_id){
	if(_private_state != 0)
		abort_m("Memory init called on already initiated memory manager.");
	_private_state = __MEM_MNGR_STATE_READY_;
	for(int i = 0; i < WORKER_THREADS; ++i)
		for(int j = 0; j < __THREAD_BUFFERS_; ++j){
			_tmp_buf[i][j] = (char *)alloc(STREAM_BUF_SIZ);
			_tmp_buf_bsy[i][j] = __BUF_MGNT_FRE_;
		}
}
void memmngr_cleanup(){
	if(_private_state != __MEM_MNGR_STATE_READY_)
		abort_m("Memory cleanup called on not ready memory manager.");
	_private_state = __MEM_MNGR_STATE_CLOSED_;
	for(int i = 0; i < WORKER_THREADS; ++i)
		for(int j = 0; j < __THREAD_BUFFERS_; ++j)
			free(_tmp_buf[i][j]);
}
static void age(int t_id, int newer){
	for(int i = 0; i < __THREAD_BUFFERS_; ++i)
		--_tmp_buf_age[t_id][i];
	_tmp_buf_age[t_id][newer] = __THREAD_BUFFERS_;
}
/*
	Allocates but in case of need will allocate same buffer, doesn't require release
	DO NOT USE MULTIPLE VEMORY'S TOGETHER IF UNSURE YOU HAVE ENOUGH FREE MEMORY
	Allocates the older allocated pieces if out of memory.
*/
char * vemory(int t_id){
	if(_private_state != __MEM_MNGR_STATE_READY_)
		abort_m("Memory allocation called on not ready memory manager.");
	for(int i = 0; i < __THREAD_BUFFERS_; ++i)
		if(_tmp_buf_bsy[t_id][i] == __BUF_MGNT_FRE_){
			_tmp_buf_bsy[t_id][i] = __BUF_MGNT_VOL_;
			age(t_id, i);
			return _tmp_buf[t_id][i];
		}
	int min = __THREAD_BUFFERS_ + 1;
	int min_b = -1;
	for(int i = 0; i < __THREAD_BUFFERS_; ++i)
		if(_tmp_buf_bsy[t_id][i] == __BUF_MGNT_VOL_)
			if(_tmp_buf_age[t_id][i] < min){
				min = _tmp_buf_age[t_id][i];
				min_b = i;
			}	/* Older buffer is reused first in volatile memory */
	if(min_b > -1){
		_tmp_buf_bsy[t_id][min_b] = __BUF_MGNT_VOL_;
		age(t_id, min_b);
		return _tmp_buf[t_id][min_b];
	}
	abort_m("Memory manager out of memory.");
	return NULL;
}
/*
	Allocates and prevents requests on the same buffer
*/
char * memory(int t_id){
	if(_private_state != __MEM_MNGR_STATE_READY_)
		abort_m("Memory allocation called on not ready memory manager.");
	for(int i = 0; i < __THREAD_BUFFERS_; ++i)
		if(_tmp_buf_bsy[t_id][i] == __BUF_MGNT_FRE_){
			_tmp_buf_bsy[t_id][i] = __BUF_MGNT_BSY_;
			return _tmp_buf[t_id][i];
		}
	int min = __THREAD_BUFFERS_ + 1;
	int min_b = -1;
	for(int i = 0; i < __THREAD_BUFFERS_; ++i)
		if(_tmp_buf_bsy[t_id][i] == __BUF_MGNT_VOL_)
			if(_tmp_buf_age[t_id][i] < min){
				min = _tmp_buf_age[t_id][i];
				min_b = i;
			}	/* Older buffer is reused first */
	if(min_b > -1){
		_tmp_buf_bsy[t_id][min_b] = __BUF_MGNT_BSY_;
		return _tmp_buf[t_id][min_b];
	}
	abort_m("Memory manager out of memory.");
	return NULL;
}
void release(int t_id, char * ptr){
	if(_private_state != __MEM_MNGR_STATE_READY_)
		abort_m("Memory deallocation called on not ready memory manager.");
	for(int i = 0; i < __THREAD_BUFFERS_; ++i)
		if(_tmp_buf[t_id][i] == ptr){
			_tmp_buf_bsy[t_id][i] = __BUF_MGNT_FRE_;
			return;
		}
	abort_m("Memory deallocation used with bad pointer.");
}
void * alloc(size_t size){
	void * r = malloc(size);
	if(r == NULL)
		abort_m("System out of memory for dynamic heap allocation.");
	return r;
}
