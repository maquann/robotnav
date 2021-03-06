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
#include <stdio.h>
 #include <stdlib.h> 
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>
#include "CruizCoreGyro.h"
#include "MathFunctions.h"

using namespace std;

CruizCoreGyro::CruizCoreGyro(float period, float track, float encoderScaleFactor, int COUNTS_REVOLUTION_in, char GYRO_PORT[]) : Archer(period, track, encoderScaleFactor, COUNTS_REVOLUTION_in)
{
	//initialize read_in variables
	PACKET_SIZE = 8;
	SAMPLES = 1000;

	int i = system("./init_gyro_port.sh");
 	if(i != 0) {
  		cout << "failure to initialize gyro port" << endl;
  		exit(1);
 	}
 	else if(i == 0){
 		cout << "BAUD RATE SET" << endl;
 	}


	if(-1 == (file_descriptor = open(GYRO_PORT,O_RDWR)))
	{
		cout << "Error opening port \n";
		cout << "Set port parameters using the following Linux command:\n stty -F /dev/ttyUSB0 115200 raw\n";
		cout << "You may need to have ROOT access";
	}
	cout << "CruizCoreR1050 communication port is ready\n";

	//software reset
	string str1 = "$MIB,RESET*87";
	int countSent1 = write(file_descriptor, str1.c_str(), str1.length());
	//Verify weather the Transmitting Data on UART was Successful or Not
	if(countSent1 < 0) {
		cout << "could not write to CruizCore" << endl;
		cout << "could not reset software" << endl;
		exit(1);
	}
	else {
		cout << "CruizCore reset" << endl;
	}

	usleep(1500);

	//cout<<"Writing: "<<ReplaceString(str, "\r", "\r\n");
	string str = "$MIA,,,,R,10,,,*EA";
	int countSent = write(file_descriptor, str.c_str(), str.length());
	//Verify weather the Transmitting Data on UART was Successful or Not
	if(countSent < 0) {
		cout << "could not write to CruizCore" << endl;
		cout << "output rate incorrect" << endl;
		exit(1);
	}
	else {
		cout << "CruizCore outout set to 10Hz" << endl;
	}

	strcpy(mName,"CruizCore");
	cout << "CruizCore Gyro Robot ready!\n";
	cout << "GYRO_PORT: " << GYRO_PORT << endl;

	if (tcflush(file_descriptor, TCIOFLUSH) == 0)
       printf("The input and output queues have been flushed.\n");
    else {
       perror("tcflush error\n");
       exit(1);
   	}

   	readSensors();

   	cout << "CruizCore Robot ready!" << endl;


}

CruizCoreGyro::~CruizCoreGyro()
{
	close(file_descriptor);
	cout << "Closing CruizCoreGyro port";
}

int CruizCoreGyro::readSensors()
{
	// if (tcflush(file_descriptor, TCIOFLUSH) == 0)
 //           printf("The input and output queues have been flushed.\n");
 //        else
 //           perror("tcflush error\n");


	//may need to double packet size to deal with extra info in buffer
	//may need to do this because


	//Get Encoder information
	Archer::readSensors();

	//read from file
	//8 bites

	//Get angle from CruizCore gyro

	short header;
	short rate_int;
	short angle_int;
	//float rate_float;
	//float angle_float;
	short check_sum;
	unsigned char data_packet[PACKET_SIZE * 10];


	//read(file_descriptor,data_packet,PACKET_SIZE*100);

	//if you flush right here then there is not enough time for the buffer to completely refill
	//you only get a few bytes when readin, not a full 8
	int hold;
	//if not making it past this line, dont forget to type into terminal:
	// $stty -F /dev/ttyUSB0 115200 raw
	hold = read(file_descriptor,data_packet,PACKET_SIZE * 10);
	if(PACKET_SIZE != hold) {
		cout << "ERROR: # of bytes actually read(8): " << hold << endl;
		mRotation = 0;
		return 0;
	}

	// Verify data packet header 
	memcpy(&header,data_packet,sizeof(short));
	if(header != (short)0xFFFF)
	{
		cout << "ERROR: HEADER(" << (short)0xFFFF << "): " << header << endl;
		mRotation = 0;
		return 0;
	}

	// Copy values from data string 
	memcpy(&rate_int,data_packet+2,sizeof(short));
	memcpy(&angle_int,data_packet+4,sizeof(short));
	memcpy(&check_sum,data_packet+6,sizeof(short));

	// Verify checksum
	if(check_sum != (short)(0xFFFF + rate_int + angle_int))
	{
		cout<< "ERROR: checksum\n";
		mRotation = 0;
		return 0;
	}

	static float s_last_angle = 0;

	// Apply scale factors
	//rate_float = rate_int/100.0;
 	//angle_float = angle_int/100.0;
	
	//cout << "rate_float:" << rate_float << " [deg/sec]" << endl << "angle_float: " << angle_float << " [deg]\n";

	mRotation = (angle_int - s_last_angle)/100.0;

	mRotation = math_functions::deg2rad(mRotation);
	mRotation = math_functions::unwrap(mRotation);

	s_last_angle = angle_int;

 	cout << "Gyro: " << math_functions::rad2deg(mRotation) << endl;


/*

	static float s_last_angle = 0;
	short new_angle = pIic->Raw[mGyroPort][pIic->Actual[mGyroPort]][0]*256 + pIic->Raw[mGyroPort][pIic->Actual[mGyroPort]][1];
	mRotation = - (new_angle - s_last_angle)/100.0; //XGL angle must be inverted
	mRotation = math_functions::deg2rad(mRotation);
	mRotation = math_functions::unwrap(mRotation);
	s_last_angle = new_angle;
	
	cout << "XG1300L: " << math_functions::rad2deg(mRotation) << endl;
*/
	return DATA_READY;
}

