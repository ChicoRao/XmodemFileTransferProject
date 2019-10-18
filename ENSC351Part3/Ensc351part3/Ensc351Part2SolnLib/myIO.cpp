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
// Helpers: _everybody helped us/me with the assignment (list names or put 'None')__
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

class Socket {

private:

	condition_variable socketCV;
	mutable mutex socketMutex;
	int dataCount;
	int pair;
	bool boolFile;

public:


	Socket(int pair)
	{
		lock_guard<mutex> lk(socketMutex);
		this->pair = pair;
		dataCount = 0;
		boolFile = false;
		cout << "Created socket" << endl;
	}

	int setDataCount(ssize_t dataCount)
	{
		lock_guard<mutex> lk(socketMutex);
		this->dataCount += dataCount;
		socketCV.notify_all();

		return this->dataCount;
	}

	bool isFile() {
		lock_guard<mutex> lk(socketMutex);
		return boolFile == true;
	}

	void drain()
	{
		unique_lock<mutex> lk(socketMutex);
		socketCV.wait(lk,[this]{return dataCount == 0;});
	}

	void readDrain(int readData) {
		lock_guard<mutex> lk(socketMutex);

		dataCount -= readData;

		socketCV.notify_all();
	}
};

vector<Socket*> vectorSocket;
mutex vectorMutex;

int mySocketpair( int domain, int type, int protocol, int des[2] )
{
	lock_guard<mutex> lk(vectorMutex);

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

	cout << "Hello" << endl;

	int returnVal = socketpair(domain, type, protocol, des);
	return returnVal;
}

//Open the file to read
int myOpen(const char *pathname, int flags, mode_t mode)
{
	lock_guard<mutex> lk(vectorMutex);

	return open(pathname, flags, mode);
}

//Creates the output file to write into
int myCreat(const char *pathname, mode_t mode)
{
	lock_guard<mutex> lk(vectorMutex);

	return creat(pathname, mode);
}

//Write into the output file or socket
ssize_t myWrite( int fildes, const void* buf, size_t nbyte )
{
	lock_guard<mutex> lk(vectorMutex);

	if(vectorSocket[fildes]->isFile())
	{
		return write(fildes, buf, nbyte);
	}
	else
	{
		//socket
		int tempDataCount = vectorSocket[fildes]->setDataCount(write(fildes, buf, nbyte));;
		return tempDataCount;
	}
}

//Close the open files or socket
int myClose( int fd )
{
	lock_guard<mutex> lk(vectorMutex);

	if(vectorSocket[fd]->isFile())	//Checks if the vector index contains a socket or file
	{
		return close(fd);
	}
	else
	{
		//clear socket data

		return close(fd);

	}
}

int myTcdrain(int des)
{ //is also included for purposes of the course.

	lock_guard<mutex> lk(vectorMutex);

	vectorSocket[des]->drain();

	return 0;
}

/* See:
 *  https://developer.blackberry.com/native/reference/core/com.qnx.doc.neutrino.lib_ref/topic/r/readcond.html
 *
 *  */
int myReadcond(int des, void * buf, int n, int min, int time, int timeout)
{
	lock_guard<mutex> lk(vectorMutex);

	int tempReadcond = wcsReadcond(des, buf, n, min, time, timeout );

	vectorSocket[des]->readDrain(tempReadcond);

	return wcsReadcond(des, buf, n, min, time, timeout );
}

//Read from input file
ssize_t myRead( int des, void* buf, size_t nbyte )
{
	lock_guard<mutex> lk(vectorMutex);
	// deal with reading from descriptors for files

	if(vectorSocket[des]->isFile())
	{
		return read(des, buf, nbyte);
	}
	else
	{
		// myRead (for our socketpairs) reads a minimum of 1 byte
		return myReadcond(des, buf, nbyte, 1, 0, 0);
	}
}

