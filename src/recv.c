/*
    Copyright (c) 2014 Tightrope Media Systems inc.  All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom
    the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#include "nml.h"

/// @module nml

/***
Receives a message on the specified SP socket.
It creates a new nml message, and puts the data received over the socket in the userdata payload.
The user needs to call nml.msg_free when done with the message or a memory leak will occur.
see <http://nanomsg.org/v0.3/nn_recv.3.html> for more details on recv.
@function recv
@tparam integer socket the SP socket identifier
@tparam integer flags the desired flags
@return the new message userdata object, or nil
@return nil, or a string containing an error message
*/
int l_recv(lua_State* L)
{
	// receive data using a nn-allocated buffer
	// push the received data to a lua string (new buffer) -- we expect to always receive strings
	// free the nn-alloc'd buffer
	//
	// NOTE: this creates a copy of the data into lua space, which is what we want in this case since we're dealing with command strings

	struct nn_iovec iov;
	struct nn_msghdr hdr;
	
	size_t sizeRecv;
	int flags = luaL_optint(L, P2, 0); // flags

   void** ppck;
   
   // allocate a new nml message
   l_nml_msg(L);

	ppck = (void**)lua_touserdata(L, -1);

	iov.iov_base = ppck;
	iov.iov_len = NN_MSG;

	hdr.msg_iov = &iov;
	hdr.msg_iovlen = 1;
	hdr.msg_control = NULL;
	hdr.msg_controllen = 0;

	sizeRecv = nn_recvmsg (luaL_checkint(L, P1), &hdr, flags);

	if (sizeRecv==-1) {
		lua_pushnil(L);
		lua_pushstring(L, nn_strerror(nn_errno()));
		return 2;
	}
   // adjust the ppck, what we got through the socket was the raw RIFF
   *ppck = ck_get_data(*ppck);

	// result
	return 1;
}