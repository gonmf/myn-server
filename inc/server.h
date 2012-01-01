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
	Server exports and definitions
*/

// Do not modify
#define __NO_SOCKET_ -1
#define __END_THREAD_ -2

// Do not modify
#define __DIRENT_LISTING_ 9
#define __TRANSLATION_IS_COMPILED_ 3
#define __TRANSLATION_IS_CGI_ 2
#define __TRANSLATION_IS_STATIC_ 1
#define __NO_TRANSLATION_ 0


// Size of the connection pool: number of HTTP requests to have simultaniously
// on a waiting list
#ifndef CONN_POOL
#ifdef SOMAXCONN
#define CONN_POOL SOMAXCONN
#else
#define CONN_POOL 64
#endif
#endif


void server_main(int port, int dmn);



#define __ERASE_TEMP_FILES_ 1


