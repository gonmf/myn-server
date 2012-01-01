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
	Common server definitions
*/

// Default server port
#define DEFAULT_PORT 80
// Number of worker threads that will be serving requests
#define WORKER_THREADS 8
// Default static file to return on root directory query
#define DEFAULT_STATIC_FILE "index.htm"
// Default dynamic compiled file to return on root directory query
#define DEFAULT_COMPILED_FILE "index"
// Default dynamic CGI file to return on root directory query
#define DEFAULT_CGI_FILE "index.cgi"
// Default error file to return on root directory ErrorDocument file not found 
#define DEFAULT_ERROR_FILE "other"
// Folder that contains ErrorDocument files
#define FOLDER_ERROR "server/errordocuments"
// Folder that contains static content
#define FOLDER_STATIC "server/content/static"
// Folder that contains compiled content
#define FOLDER_COMPILED "server/content/compiled"
// Folder that contains CGI-scrips
#define FOLDER_CGI "server/content/cgi-bin"
// Folder that contains CGI modules
#define FOLDER_CGI_MODULES "server/cgimodules/"
// String for swap files of dynamic content
#define VIRTUAL_BUFFER "tmp/c%d"
// Define whether to show directory listings when a default index file is not
// found
#define DIRENT_LISTING 1
/*
	IMPORTANT
	
	Some other very important contants are defined in prototype.h.
	They are in a different file for exporting to external modules.
	They are still very important to adjust carefully.
*/
// Internal connection queue of sockets waiting to be processed
#define __ACCEPTED_PENDING_CONNS_ 2048
// Instruction for the encoding of content using gzip(1)
#define __GZIP_TMP_ "gzip -c --fast %s > tmp/g%d"
// String for gzip(1) output files
#define __GZIP_FILE_ "tmp/g%d"
// Set to force compression to be never used
#define __FORCE_NO_COMPRESS_ 0
// Define the maximum size of files to be compressed (prior to compression)
// WARNING: This only applies to static content, compiled routines and CGI
// scripts will always use compression if possible and the
// __FORCE_NO_COMPRESS_ flag is not set.
#define __MAX_COMPRESS_SIZ_ 4194304
// This is a very important value. Each worker thread will use these buffers for
// everything. Even though certain functions adapt themselves if the number of
// buffers is very low others may not. Also the operation of assynchronously
// reading a file from disk and writing it on a socket is set to use ALL buffers
// for maximum performance (only on bigger files).
// The reccomended value is a MINIMUM of 4.
#define __THREAD_BUFFERS_ 4
// Set to send the first byte of the HTTP response without actually treating the
// request first. Benchmarks that rely on the TTFB to measure server speed will
// be influenced greatly; actual performance impact is miniscule.
#define __PERFORM_TTFB_TRICK_ 0

