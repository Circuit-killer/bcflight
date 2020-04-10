/*
 * BCFlight
 * Copyright (C) 2016 Adrien Aubry (drich)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef SOCKET_H
#define SOCKET_H

#if ( BUILD_SOCKET == 1 )

#include <netinet/in.h>
#include "Link.h"

class Main;

class Socket : public Link
{
public:
	typedef enum {
		TCP,
		UDP,
		UDPLite
	} PortType;

	Socket( uint16_t port, PortType type = TCP, bool broadcast = false, uint32_t timeout = 0 );
	virtual ~Socket();

	int Connect();
	int setBlocking( bool blocking );
	void setRetriesCount( int retries );
	int retriesCount() const;
	int32_t Channel();
	int32_t RxQuality();
	int32_t RxLevel();

	SyncReturn Read( void* buf, uint32_t len, int32_t timeout );
	SyncReturn Write( const void* buf, uint32_t len, bool ack = false, int32_t timeout = -1 );

	static int flight_register( Main* main );

protected:
	static Link* Instanciate( Config* config, const string& lua_object );

	uint16_t mPort;
	PortType mPortType;
	bool mBroadcast;
	uint32_t mTimeout;
	int mSocket;
	struct sockaddr_in mSin;
	int mClientSocket;
	struct sockaddr_in mClientSin;

	// Stats
	int32_t mChannel;
};

#else

class Socket {
public:
	static int flight_register( Main* main ) { return 0; }
};

#endif // ( BUILD_SOCKET == 1 )

#endif // SOCKET_H
