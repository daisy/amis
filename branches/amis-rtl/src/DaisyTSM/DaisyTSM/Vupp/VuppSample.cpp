// VuppSample.cpp: implementation of the CVuppSample class.
//
//////////////////////////////////////////////////////////////////////

#include "VuppSample.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVuppSample::CVuppSample()
{
	pStartTime = pEndTime = 0;
	pStartMediaTime = pEndMediaTime = 0;
	pMediaType = NULL;
	syncPoint = false;
	bufferSize = 0;
}

CVuppSample::~CVuppSample()
{
	//ke20030623 old said 'delete[]'
	delete[bufferSize] buffer;
	buffer = NULL;
	delete[] pMediaType;
	pMediaType = NULL;
}


void CVuppSample::saveMediaSample(IMediaSample *pMediaSample)
{
	// This function takes an IMediaSamples and copies
	// its contents for future use. 	

	pMediaSample->GetTime(&pStartTime, &pEndTime);
	
	pMediaSample->GetMediaTime(&pStartMediaTime, &pEndMediaTime);	
	pMediaSample->GetMediaType(&pMediaType);
	syncPoint = (S_OK == pMediaSample->IsSyncPoint());

	short *pMS;
	unsigned int ccMS; // Number of samples in IMediaSample buffer	


	// Get pointer to sound buffers. Both input and output
	if(S_OK  != pMediaSample->GetPointer((unsigned char**) &pMS))
		perror("Error CVuppSample::saveBuffer: ");


	// Converting from bytes to number of samples.
	// We assume the audio stream to have mono 16 bit, which 
	// means 2 bytes per sample.

	ccMS   = pMediaSample->GetActualDataLength()  / 2; 
	bufferSize = ccMS;
	
//	char buf[300];
//	sprintf(buf, "Bufsize: %d, ccMS: %d, First cell:%d", bufferSize, ccMS, (int)buffer[0]);

	if((long)ccMS <= bufferSize )
		// Copy the samples 
		while(ccMS--)
			 buffer[ccMS] = pMS[ccMS];
	else
		;//MessageBox(NULL, "Error: the buffer isn't big enough", "VUPP", MB_OK);
}

void CVuppSample::initializeBuffer(int nbrOfSamples)
{
	if(bufferSize == 0)
	{
		buffer = new short[nbrOfSamples];
		bufferSize = nbrOfSamples;
	}
	else
		;//MessageBox(NULL, "Error: You have tried to initialize the buffer more than one time", "VUPP", MB_OK);
}


void CVuppSample::loadMediaSample(IMediaSample *pMediaSample)
{

	short *pMS;
	unsigned int ccMS = bufferSize; // Number of samples in IMediaSample buffer	

	// Get pointer to sound buffers. Both input and output
	if(S_OK  != pMediaSample->GetPointer((unsigned char**) &pMS))
		perror("Error CVuppSample::loadBuffer: ");


	// Converting from bytes to number of samples.
	// We assume the audio stream to have mono 16 bit, which 
	// means 2 bytes per sample.

	pMediaSample->SetActualDataLength(ccMS * 2); 

	pMediaSample->SetTime(&pStartTime, &pEndTime);

	pMediaSample->SetMediaTime(&pStartMediaTime,&pEndMediaTime);

	pMediaSample->SetMediaType(pMediaType);

	pMediaSample->SetSyncPoint(syncPoint);


//ke20030624
//	// Get the samplespeed
//	WAVEFORMATEX *pWFEXTest;
//	pWFEXTest = (WAVEFORMATEX*) malloc(sizeof(WAVEFORMATEX));

	// Copy the samples 
	while(ccMS--)
		 pMS[ccMS] = buffer[ccMS];
}

LONGLONG CVuppSample::getStartTime()
{
	return pStartTime;
}

LONGLONG CVuppSample::getEndTime()
{
	return pEndTime;
}

void CVuppSample::setStartTime(LONGLONG sTime)
{
	pStartTime = sTime;
}

void CVuppSample::setEndTime(LONGLONG sTime)
{
	pEndTime = sTime;
}
