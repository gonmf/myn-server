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

typedef struct __ring_{
	pthread_spinlock_t sl;
	sem_t sem;
	void ** list;
	size_t get;
	size_t put;
	size_t max;
} ring_t;
void ring_init(ring_t * r, size_t capacity);
int ring_put(ring_t * r, void * value);
int ring_get(ring_t * r, void ** value);
int ring_try_get(ring_t * r, void ** value);
void ring_destroy(ring_t * r);
