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
	Socket transport layer
	Synchronous non blocking functions
*/


// Avoid modifying this unless good connections are dropping
// or bad connections are trying to (D)DoS the server.
// Connection request timeout in milliseconds
#define __MAX_MSEC_ELAPSED_ 250

int synchronous_receive(int socket, char * buffer, size_t max_len);
int synchronous_send(int socket, const char * buffer, size_t len);
int synchronous_send_fd(int socket, int file, int t_id);


