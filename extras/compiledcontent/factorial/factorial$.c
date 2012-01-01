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



#include <stdio.h>
#include <string.h>
#include "../../../inc/prototype.h"
static void main(loader_args_t * args){
	if(args->argc != 1){
		args->return_code = 400;
		return;
	}
	int n = atoi(args->argv[0]);
	int i;
	int t = 1;
	for(i = 1; i <= n; ++i)
		t *= i;
	dprintf(args->out_body, "<html><body><h2>Factorial of %d</h2><h3>%d</h3></body></html>", n, t);
	args->return_code = 200;
}


void get(loader_args_t * args){
	main(args);
}
void head(loader_args_t * args){
	main(args);
}
void post(loader_args_t * args){
	main(args);
}
void unknown(loader_args_t * args){
	main(args);
}
