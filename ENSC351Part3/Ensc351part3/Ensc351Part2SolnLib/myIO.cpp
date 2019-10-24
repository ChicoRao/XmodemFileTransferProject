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
#include <thread>

using namespace std;

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
		pair = -1;
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

		if(dataCount >= 0)
		{
			socketCVRead.notify_one();
		}

		return temp;
	}

	bool isFile() {
		return boolFile == true;
	}

	void drain(unique_lock<mutex>& vectorLock)
	{
		unique_lock<mutex> slk(socketMutex);

		vectorLock.unlock();

		socketCVDrain.wait(slk,[this]{return dataCount <= 0;});
	}

	int readCond(int des, void * buf, int n, int min, int time, int timeout)
	{
		unique_lock<mutex> slk(socketMutex);

		int readData;

		if(dataCount < min)
		{
			dataCount -= min;

			if(dataCount <= 0)
			{
				socketCVDrain.notify_all();
			}

			socketCVRead.wait(slk,[this]{return dataCount >= 0;});

			readData = wcsReadcond(des, buf, n, min, time, timeout);

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
			//reading after the wait
			readData = wcsReadcond(des, buf, n, min, time, timeout );

			//subtracting readData from dataCount
			if(dataCount > 0)
			{
				dataCount -= readData;

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

	bool checkClose()
	{
		lock_guard<mutex> slk(socketMutex);

		if(dataCount > 0)
		{
			dataCount = 0;
			socketCVDrain.notify_all();
		}
		else if(dataCount < 0)
		{
			dataCount = 0;
			socketCVRead.notify_one();
		}

		return true;	//Returns true to show that it is done checking
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

	if(pairDes == -1)
	{
		return write(fildes, buf, nbyte);
	}

	if(!(vectorSocket[pairDes]->isFile()))
	{
		//socket
		int totalWrite = vectorSocket[pairDes]->setDataCount(fildes, buf, nbyte);

		return totalWrite;
	}
}

//Close the open files or socket
int myClose( int fd )
{
	//Returns 0 if successfully closed, -1 if error
	unique_lock<mutex> vlk(vectorMutex);

	if(vectorSocket[fd]->isFile())	//Checks if the vector index contains a socket or file
	{
		delete vectorSocket[fd];

		vectorSocket[fd] = nullptr;

		return close(fd);
	}
	else
	{
		//clear socket data

		int temp = close(fd);

		bool check = false;
		bool checkPair = false;

		check = vectorSocket[fd]->checkClose();	//Returns true when it is done checking

		checkPair = vectorSocket[vectorSocket[fd]->getPair()]->checkClose();	//Returns true when it is done checking for the pair

		while(!(check && checkPair))	//Deletes the vector socket when it finishes checkClose for both itself and its pair
		{
			delete vectorSocket[fd];

			vectorSocket[fd] = nullptr;
		}

		return temp;
	}
}

//Tcdrain waits for something to be read GOOD?
int myTcdrain(int des)
{ //is also included for purposes of the course.

	unique_lock<mutex> vlk(vectorMutex);

	if(vectorSocket[des]->getPair() != -1)
	{
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
	int totalDataRead = vectorSocket[des]->readCond(des, buf, n, min, time, timeout);

	return totalDataRead;
}

//Read from input file
ssize_t myRead( int des, void* buf, size_t nbyte )
{
	// deal with reading from descriptors for files

	if(!(vectorSocket[des]->isFile()))
	{
		// myRead (for our socketpairs) reads a minimum of 1 byte
		return myReadcond(des, buf, nbyte, 1, 0, 0);
	}

	return read(des, buf, nbyte);
}





