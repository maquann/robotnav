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

#include <iostream>
#include <cmath>
#include "Odometry.h"
#include "Archer.h"

 //TCP
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include "tcpconnector.h"
#include "TCPComm.h"

using namespace std;

TCPComm::TCPComm(Archer *pSensors, char* server, int port) : Odometry(pSensors)
{
    TCPConnector* connector = new TCPConnector();
	TCPStream* stream = connector->connect(server, port);

}

void TCPComm::sndMessage()
{
	size_t buffsize;
	buffsize = 60;
    char str1 [buffsize];    //char line[256];
    if (stream) {
       	sprintf(str1, "Current: %d X: %f Y: %f Speed:%f", mpSensors->mCurrent, mX, mY, mSpeed);
    	stream->send(str1, buffsize);
     	printf("sent - %s\n", str1);
      	//len = stream->receive(line, sizeof(line));
      	//line[len] = 0;
      	//printf("received - %s\n", line);
    }
}

void TCPComm::rcvMessage()
{
	size_t buffsize;
	buffsize = 60;
    char str1 [buffsize];    //char line[256];
    if (stream) {
       	sprintf(str1, "Current: %d X: %f Y: %f Speed:%f", mpSensors->mCurrent, mX, mY, mSpeed);
    	stream->send(str1, buffsize);
     	printf("sent - %s\n", str1);
      	//len = stream->receive(line, sizeof(line));
      	//line[len] = 0;
      	//printf("received - %s\n", line);
    }
}

void TCPComm::closeConn()
{
    delete stream;
    delete connector;
}