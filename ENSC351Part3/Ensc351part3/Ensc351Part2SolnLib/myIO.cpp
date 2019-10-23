//============================================================================
//
//% Student Name 1: Rico Chao
//% Student 1 #: 301310624
//% Student 1 userid (email): rchao (rchao@sfu.ca)
//
//% Student Name 2: Reed Lu
//% Student 2 #: 301289044
//% Student 2 userid (email): reedl (reedl@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put 'None' if nobody helped (the two of) you.
//
// Helpers: _TA__
//
// Also, list any resources beyond the course textbooks and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  ___________
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Also enter the above information in other files to submit.
//% * Edit the "Helpers" line and, if necessary, the "Resources" line.
//% * Your group name should be "P3_<userid1>_<userid2>" (eg. P3_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// File Name   : myIO.cpp
// Version     : September, 2019
// Description : Wrapper I/O functions for ENSC-351
// Copyright (c) 2019 Craig Scratchley  (wcs AT sfu DOT ca)
//============================================================================

#include <unistd.h>			// for read/write/close
#include <fcntl.h>			// for open/creat
#include <sys/socket.h> 		// for socketpair
#include "SocketReadcond.h"
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>

using namespace std;

int dataCountStore;	//Used to consider myReadcond

class Socket {

private:
	mutex socketMutex;
	condition_variable socketCVDrain;
	condition_variable socketCVRead;
	int dataCount = 0;
	int pair;
	bool boolFile;

public:

	Socket()
	{
		boolFile = true;
		cout << "Created file" << endl;
	}

	Socket(int pair)
	{
		this->pair = pair;
		dataCount = 0;
		boolFile = false;
	}

	int setDataCount(int descriptor, const void* buffer, size_t nbyte)
	{
		lock_guard<mutex> slk(socketMutex);

		int temp = write(descriptor, buffer, nbyte);

		dataCount += temp;

		//cout << "setDataCount write: " << temp << " to dataCount: " << dataCount << endl;

		if(dataCount >= 0)
		{
			socketCVRead.notify_one();
		}

		//cout << "function setDataCount: " << dataCount << endl;

		return temp;
	}

	bool isFile() {
		lock_guard<mutex> slk(socketMutex);
		return boolFile == true;
	}

	void drain(unique_lock<mutex>& vectorLock)
	{
		unique_lock<mutex> slk(socketMutex);

		vectorLock.unlock();

		if(dataCount > 0)
		{
			//socketCVDrain.wait(slk,[this]{cout << "waiting for dataCount is 0" << endl; return dataCount == 0;});
			socketCVDrain.wait(slk,[this]{return dataCount <= 0;});
		}
	}

	int readCond(int des, void * buf, int n, int min, int time, int timeout)
	{
		unique_lock<mutex> slk(socketMutex);

		//cout << "inside function readCond" << endl;

		int readData;

		if(dataCount < min)
		{
			//cout << "dataCount: " << dataCount << " smaller than min: " << min << endl;

			dataCount -= min;

			if(dataCount <= 0)
			{
				socketCVDrain.notify_all();
			}

			socketCVRead.wait(slk,[this]{return (dataCount) >= 0;});

			readData = wcsReadcond(des, buf, n, min, time, timeout );

			//cout << "readData: " << readData << endl;

			if(readData != -1)
			{
				dataCount -= (readData - min);
			}
			else {
				dataCount += min;
			}
		}
		else
		{
			//cout << "dataCount4: " << dataCount << " greater or equal to min: " << min << endl;

			//reading after the wait
			readData = wcsReadcond(des, buf, n, min, time, timeout );

			//cout << "readData2: " << readData << endl;

			//subtracting readData from dataCount
			if(dataCount > 0)
			{
				dataCount -= readData;

				//cout << "dataCount5: " << dataCount << endl;

				if(!dataCount)
				{
					socketCVDrain.notify_all();
				}
			}
		}
		return readData;
	}

	int getPair()
	{
		return pair;
	}

	void setPair(int pairNum)
	{
		lock_guard<mutex> slk(socketMutex);
		pair = pairNum;
	}

	void checkClose()
	{
		lock_guard<mutex> slk(socketMutex);

		cout << "dataCount when closing " << dataCount << endl;

		if(dataCount > 0)
		{
			socketCVDrain.notify_all();
		}
		else if(dataCount < 0)
		{
			socketCVDrain.notify_one();
		}
	}

};

vector<Socket*> vectorSocket;
mutex vectorMutex;

//Creates the socketpair
int mySocketpair( int domain, int type, int protocol, int des[2] )
{
	lock_guard<mutex> vlk(vectorMutex);
	int returnVal = socketpair(domain, type, protocol, des);

	if(returnVal == -1)
	{
		return returnVal;
	}

	if(vectorSocket.size() <= des[0])
	{
		vectorSocket.resize(des[0]+1);
	}

	if(vectorSocket.size() <= des[1])
	{
		vectorSocket.resize(des[1]+1);
	}

	vectorSocket[des[0]] = new Socket(des[1]);

	vectorSocket[des[1]] = new Socket(des[0]);

	return returnVal;
}

//Open the file to read
int myOpen(const char *pathname, int flags, mode_t mode)
{
	lock_guard<mutex> vlk(vectorMutex);

	cout << "Hello myOpen" << endl;

	int temp = open(pathname, flags, mode);

	//For myOpen, also need to store in vector, but as isFile, so it is not socket
	if(vectorSocket.size() <= temp)
	{
		vectorSocket.resize(temp+1);
	}

	//Create new socket object but make isFile true
	vectorSocket[temp] = new Socket;

	return temp;
}

//Creates the output file to write into
int myCreat(const char *pathname, mode_t mode)
{
	lock_guard<mutex> vlk(vectorMutex);

	cout << "Hello myCreat" << endl;

	int temp = creat(pathname, mode);

	//For myCreat, also need to store in vector, but as file
	if(vectorSocket.size() <= temp)
	{
		vectorSocket.resize(temp+1);
	}

	//Create new socket object but make isFile true
	vectorSocket[temp] = new Socket;

	return temp;
}

//Write into the output file or socket
ssize_t myWrite( int fildes, const void* buf, size_t nbyte )
{
	unique_lock<mutex> vlk(vectorMutex);

	int pairDes = vectorSocket[fildes]->getPair();

	if(vectorSocket[pairDes]->isFile())
	{
		cout << "Writing into file" << endl;

		return write(pairDes, buf, nbyte);
	}
	else
	{
		//socket
		int totalWrite = vectorSocket[pairDes]->setDataCount(fildes, buf, nbyte);

		cout << "Writing into socket" << endl;

		return totalWrite;
	}

}

//Close the open files or socket
int myClose( int fd )
{
	//Returns 0 if successfully closed, -1 if error

	lock_guard<mutex> vlk(vectorMutex);

	cout << "Hello myClose" << endl;

	if(vectorSocket[fd]->isFile())	//Checks if the vector index contains a socket or file
	{
		delete vectorSocket[fd];

		vectorSocket[fd] = nullptr;

		return close(fd);

	}
	else
	{
		//clear socket data
		cout << "Closing socket" << endl;

		//See if we have to call checkClose before closing sockets
		vectorSocket[fd]->checkClose();

		vectorSocket[vectorSocket[fd]->getPair()]->checkClose();

		vectorSocket[fd]->setPair(-1);

		vectorSocket[vectorSocket[fd]->getPair()]->setPair(-1);

		delete vectorSocket[fd];

		delete vectorSocket[vectorSocket[fd]->getPair()];

		vectorSocket[fd] = nullptr;

		vectorSocket[vectorSocket[fd]->getPair()] = nullptr;

		return close(fd);
	}

}

//Tcdrain waits for something to be read GOOD?
int myTcdrain(int des)
{ //is also included for purposes of the course.

	unique_lock<mutex> vlk(vectorMutex);

	//cout << "Enter Tcdrain" << endl;

	if(vectorSocket[des]->getPair() != -1)
	{
		//cout << "Socket Tcdrain" << endl;

		vectorSocket[vectorSocket[des]->getPair()]->drain(vlk);

	}

	return 0;
}

/* See:
 *  https://developer.blackberry.com/native/reference/core/com.qnx.doc.neutrino.lib_ref/topic/r/readcond.html
 *
 *  */
int myReadcond(int des, void * buf, int n, int min, int time, int timeout)
{
	//cout << "Socket reading" << endl;

	int totalDataRead = vectorSocket[des]->readCond(des, buf, n, min, time, timeout);

	return totalDataRead;
}

//Read from input file
ssize_t myRead( int des, void* buf, size_t nbyte )
{
	// deal with reading from descriptors for files

	//cout << "Enter myRead" << endl;

	if(vectorSocket[des]->isFile())
	{
		//cout << "File reading" << endl;

		return read(des, buf, nbyte);
	}
	else
	{
		// myRead (for our socketpairs) reads a minimum of 1 byte
		return myReadcond(des, buf, nbyte, 1, 0, 0);
	}
}







