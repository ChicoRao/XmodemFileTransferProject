//============================================================================
// Version     : September, 2019
// Copyright   : Copyright 2019, Craig Scratchley
// Description : A solution file for ENSC 351 Project Part 2
//============================================================================

#include <stdlib.h> // EXIT_SUCCESS
#include <sys/socket.h>
#include <pthread.h>
#include <thread>
#include <chrono>         // std::chrono::
#include "myIO.h"
#include "SenderX.h"
#include "ReceiverX.h"
#include "Medium.h"
#include "VNPE.h"
#include "AtomicCOUT.h"

using namespace std;

enum  {Term1, Term2};
enum  {TermSkt, MediumSkt};

//static int daSktPr[2];	  //Socket Pair between term1 and term2
static int daSktPrT1M[2];	  //Socket Pair between term1 and medium
static int daSktPrMT2[2];	  //Socket Pair between medium and term2

void termFunc(int termNum)
{
	if (termNum == Term1) {
		const char *receiverFileName;

		receiverFileName = "transferredFile-cs";
	    COUT << "Will try to receive with Checksum to file:  " << receiverFileName << endl;
		//ReceiverX xReceiver(daSktPr[Term1], receiverFileName, false);
		ReceiverX xReceiverCS(daSktPrT1M[TermSkt], receiverFileName, false);
		xReceiverCS.receiveFile();
		COUT << "xReceiver result was: " << xReceiverCS.result << endl;
		
	    COUT << "Term1 waiting for 2 seconds" << endl;
	    this_thread::sleep_for (chrono::seconds(2));
	    COUT << endl;
	    this_thread::sleep_for (chrono::milliseconds(1));

		receiverFileName = "transferredFile-crc16";
		COUT << "Will try to receive with CRC16 to file:  " << receiverFileName << endl;
		//ReceiverX xReceiver(daSktPr[Term1], receiverFileName, true);
		ReceiverX xReceiver(daSktPrT1M[TermSkt], receiverFileName, true);
		xReceiver.receiveFile();
		COUT << "xReceiver result was: " << xReceiver.result << endl;

	    this_thread::sleep_for (chrono::milliseconds(5));
		PE(myClose(daSktPrT1M[TermSkt]));
	}
	else {
		PE_0(pthread_setname_np(pthread_self(), "T2")); // give the thread (terminal 2) a name
	    // PE_0(pthread_setname_np("T2")); // Mac OS X

		const char *senderFileName = "/etc/init.d/vmware-tools"; // for ubuntu target
		// const char *senderFileName = "/etc/mailcap"; // for ubuntu target
		COUT << "Will try to send the file:  " << senderFileName << endl;
		//SenderX xSender(senderFileName, daSktPr[Term2]);
		SenderX xSender(senderFileName, daSktPrMT2[TermSkt]);
		xSender.sendFile();
		COUT << "xSender result was: " << xSender.result << endl;

		if (xSender.result != "UnexpectedC") { 
		    this_thread::sleep_for (chrono::milliseconds(1));
			COUT << endl;
		    this_thread::sleep_for (chrono::milliseconds(1));

			COUT << "Will try to send the file:  " << senderFileName << endl;
			//SenderX xSender(senderFileName, daSktPr[Term2]);
			SenderX xSender2(senderFileName, daSktPrMT2[TermSkt]);
			xSender2.sendFile();
			COUT << "xSender result was: " << xSender2.result << endl;
		}

	    this_thread::sleep_for (chrono::milliseconds(10));
		PE(myClose(daSktPrMT2[TermSkt]));
	}
    //std::this_thread::sleep_for (std::chrono::milliseconds(1));
	//PE(myClose(daSktPr[termNum]));
}

// ***** you will need this at some point *****
void mediumFunc(void)
{
	PE_0(pthread_setname_np(pthread_self(), "M")); // give the thread (medium) a name
	Medium medium(daSktPrT1M[MediumSkt],daSktPrMT2[MediumSkt], "xmodemData.dat");
	medium.run();
}

int myMain()
{
	PE_0(pthread_setname_np(pthread_self(), "P-T1")); // give the primary thread (terminal 1) a name
    // PE_0(pthread_setname_np("P-T1")); // Mac OS X

	// ***** switch from having one socketpair for direct connection to having two socketpairs
	//			for connection through medium thread *****
//	PE(mySocketpair(AF_LOCAL, SOCK_STREAM, 0, daSktPr));
	PE(mySocketpair(AF_LOCAL, SOCK_STREAM, 0, daSktPrT1M));
	PE(mySocketpair(AF_LOCAL, SOCK_STREAM, 0, daSktPrMT2));

	thread term2Thrd(termFunc, Term2);

	// ***** create thread for medium *****
	thread mediumThrd(mediumFunc);

	termFunc(Term1);

	term2Thrd.join();
	// ***** join with thread for medium *****
	mediumThrd.join();

    std::this_thread::sleep_for (std::chrono::milliseconds(10));
	return EXIT_SUCCESS;
}
