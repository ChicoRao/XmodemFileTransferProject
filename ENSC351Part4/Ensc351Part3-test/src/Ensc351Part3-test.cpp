/* Ensc351Part3-test.cpp -- October -- Copyright 2019 Craig Scratchley */
#include <sys/socket.h>
#include <stdlib.h>				// for exit()
#include <sched.h>
#include <thread>
#include "AtomicCOUT.h"
#include "VNPE.h"
#include "myIO.h"

/* This program can be used to test your changes to myIO.cpp
 *
 * Put this project in the same workspace as your Ensc351Part2SolnLib and Ensc351 library projects,
 * and build it.
 *
 * With two created threads for a total of 3 threads, the output that I get is:
 *

RetVal 1 in primary: 14 Ba: abcd123456789
RetVal in T42: 0
RetVal in T32: 0
RetVal 2 in primary: 4 Ba: xyz

 *
 */

using namespace std;

static int daSktPr[2];	  // Descriptor Array for Socket Pair
cpu_set_t set;
int myCpu=0;

void threadT42Func(void)
{
    PE(sched_setaffinity(0, sizeof(set), &set)); // set processor affinity for current thread

	PE_NOT(myWrite(daSktPr[1], "ijkl", 5), 5);
	int RetVal = PE(myTcdrain(daSktPr[1])); // will block until myClose
	cout << "RetVal in T42: " << RetVal << endl;
}

void threadT32Func(void)
{
    PE(sched_setaffinity(0, sizeof(set), &set)); // set processor affinity for current thread

    PE_NOT(myWrite(daSktPr[0], "abcd", 4), 4);
	PE(myTcdrain(daSktPr[0])); // will block until 1st myReadcond

	PE_NOT(myWrite(daSktPr[0], "123456789", 10), 10); // don't forget nul termination character
    this_thread::sleep_for (chrono::milliseconds(10));

	PE(myWrite(daSktPr[0], "xyz", 4));
	int RetVal = PE(myClose(daSktPr[0]));
    this_thread::sleep_for (chrono::milliseconds(10));

	cout << "RetVal in T32: " << RetVal << endl; // not needed on submission
}

int main() {
    CPU_SET(myCpu, &set);
    PE(sched_setaffinity(0, sizeof(set), &set)); // set processor affinity for current thread

	PE(mySocketpair(AF_LOCAL, SOCK_STREAM, 0, daSktPr));
	thread threadT32(threadT32Func);
    this_thread::sleep_for (chrono::milliseconds(10));

	thread threadT42(threadT42Func);  // you can try without this thread too.
	//sched_yield();
    this_thread::sleep_for (chrono::milliseconds(10));

    char	Ba[200];
    int
	RetVal = PE(myReadcond(daSktPr[1], Ba, 200, 12, 0, 0));  // will block until myWrite of 10 characters
	cout << "RetVal 1 in primary: " << RetVal << " Ba: " << Ba << endl;

    RetVal = PE(myReadcond(daSktPr[1], Ba, 200, 12, 0, 0)); // will block until myClose

	threadT32.join();
	threadT42.join(); // only needed if you created the thread above.
	cout << "RetVal 2 in primary: " << RetVal << " Ba: " << Ba << endl;

	return 0;
}
