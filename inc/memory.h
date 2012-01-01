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
	Definitions related to the memory allocator of worker threads
*/

#define __BUF_MGNT_VOL_ 2
#define __BUF_MGNT_BSY_ 1
#define __BUF_MGNT_FRE_ 0

#define __MEM_MNGR_STATE_READY_ 1
#define __MEM_MNGR_STATE_CLOSED_ 2


void * alloc(size_t size);

int free_mem(int t_id);
int vol_mem(int t_id);
void memmngr_init();
void memmngr_cleanup();
char * vemory(int t_id);
char * memory(int t_id);
void release(int t_id, char * ptr);
void print_memory_state(int t_id);
