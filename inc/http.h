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
	Definitions related to HTTP connections
*/

typedef enum _HTTP_CODE{
	OK = 200,
	Created = 201,
	NoContent = 202,
	MovedPermanently = 204,
	NotModified = 301,
	BadRequest = 400,
	Unauthorized = 401,
	Forbidden = 403,
	NotFound = 404,
	InternalServerError = 500,
	NotImplemented = 501,
	BadGateway = 502,
	ServiceUnavailable = 503
} HTTP_CODE;

#define HTT_HEADERS_ONLY 1
#define HTT_ERROR_DOCUMENT 2
#define HTT_SEND_STREAM 3
#define HTT_SEND_FILE 4
int http_answer(int socket, int htt_mode, void * blob, http_context_t * ctx, int t_id);


