/*
 * Robot Navigation Program
 * www.robotnav.com
 *
 * (C) Copyright 2010 - 2014 Lauro Ojeda
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
 */

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcpconnector.h"
#include "tcpacceptor.h"
#include <string>

class TCPServer
{
	char* server;
	int port;
	public:
		TCPAcceptor* acceptor;
		TCPConnector* connector;
		TCPStream* stream;
		char message[256];
		//TCPComm(Robot *pSensors, char* server, int port);
		TCPServer(char* pserver, int pport);
		virtual void makeServer();
		virtual void makeClient();
		virtual void sndMessage(string strmes);
		virtual string rcvMessage();
		virtual void closeConn();
};

#endif
