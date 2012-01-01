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
	Entry routine, add/validate parameters here.
*/

#include "inc/common.h"

int main(int argc, char * argv[]){
	switch(argc){
		case 1:
			server_main(DEFAULT_PORT, 0);
			break;
		case 2:
			if(strcmp(argv[1], "-d"))
				server_main(atoi(argv[1]), 0);
			else
				server_main(DEFAULT_PORT, 1);
			break;
		case 3:
			if(strcmp(argv[2], "-d"))
				server_main(atoi(argv[1]), 0);
			else
				server_main(atoi(argv[1]), 1);
			break;				
		default:
			printf("Usage: %s [port] [-d]\n\tport : Number of the listen port (default %d)\n\t-d : make the program a daemon(3)\n", argv[0], DEFAULT_PORT);
			exit(EXIT_FAILURE);
	}
	return 0;
}
