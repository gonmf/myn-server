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
	Implementation of a int ring, blocking on get only, using spin locks and
	same-process semaphores.
*/

#include "inc/common.h"

void ring_init(ring_t * r, size_t capacity){
	pthread_spin_init(&(r->sl), PTHREAD_PROCESS_PRIVATE /* same process */);
	if(sem_init(&(r->sem), 0 /* same process */, 0))
		abort_m("Ring error on semaphore init.");
	r->list = (void **)alloc(sizeof(void *) * capacity);
	r->get = 0;
	r->put = 0;
	r->max = capacity;
}
int ring_put(ring_t * r, void * value){
	pthread_spin_lock(&(r->sl));
	if((r->put + 1 == r->get) || ((r->put + 1 == r->max) && (r->get == 0))){
		pthread_spin_unlock(&(r->sl));
		return 1;
	}			
	if(r->put == r->max)
		r->put = 0;
	r->list[r->put++] = value;
	pthread_spin_unlock(&(r->sl));
	if(sem_post(&(r->sem)))
		abort_m("Ring error on semaphore post.");
	return 0;
}
int ring_get(ring_t * r, void ** value){
	pthread_spin_lock(&(r->sl));
	if(r->get == r->put){
		pthread_spin_unlock(&(r->sl));
	}else{
		*value = r->list[r->get++];
		if(r->get == r->max)
			r->get = 0;
		pthread_spin_unlock(&(r->sl));
		return 0;
	}
	if(sem_wait(&(r->sem)))
		abort_m("Ring error on semaphore wait.");
	return ring_get(r, value);
}
int ring_try_get(ring_t * r, void ** value){
	pthread_spin_lock(&(r->sl));
	if(r->get == r->put){
		pthread_spin_unlock(&(r->sl));
		return 1;
	}		
	*value = r->list[r->get++];
	if(r->get == r->max)
		r->get = 0;
	pthread_spin_unlock(&(r->sl));
	return 0;
}
void ring_destroy(ring_t * r){
	if(sem_destroy(&(r->sem)))
		abort_m("Ring error on semaphore cleanup.");
	pthread_spin_destroy(&(r->sl));
	free(r->list);
}
