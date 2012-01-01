#
#    This file is part of myn.
#
#    myn is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    myn is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with myn.  If not, see <http://www.gnu.org/licenses/>.
#



all:
	$(CC) -std=c99 -fPIC -o ./mynserver -O3 -Wall -Wno-unused-result \
	transport.c \
	memory.c \
	system.c \
	ring.c \
	cgi.c \
	global.c \
	loader.c \
	parser.c \
	http.c \
	worker.c \
	server.c \
	myn.c \
	-ldl -lrt

run: ./mynserver
	x-terminal-emulator -e ./mynserver &

clean:
	$(RM) ./mynserver ./tmp/*
