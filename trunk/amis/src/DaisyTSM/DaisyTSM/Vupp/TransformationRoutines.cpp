// Transformationroutines 
//#include <math.h>

#include "TransformSample.h"

HRESULT CTransformSample::Transform(IMediaSample *pIn, IMediaSample *pOut) 
{
	return E_INVALIDARG;
}

HRESULT CTransformSample::Receive(IMediaSample *pIn)
{
/* MEMDEBUG for VUPP class debugging
	_CrtMemDumpAllObjectsSince( NULL );
	_CrtMemCheckpoint( &s1 );
*/
	// We assume that every ImediaSample we get has 
	// buffers of identical size.

	// Initializing buffers and creating the CVupp
	if(!vuppSmpInitialized) 
	{
		// We want half a second of total buffer (22050 samples). We have to check
		// the incoming buffersize to know how many buffers we need.

		int nbrOfSamples, halfaSecond;
		MSampleDataLength = pIn->GetActualDataLength();
		halfaSecond = (int)(pWFEXIn->nSamplesPerSec * 0.5); 
		
		// A 16-bit sample is two bytes
		nbrOfSamples = MSampleDataLength / 2;
		// Buffersize is half a second.
		nbrOfvuppSmp =  halfaSecond / nbrOfSamples; 
		// There must be at least one CvuppSample
		nbrOfvuppSmp = max(1,nbrOfvuppSmp); 

		// Outbuffer can be twice as big as inbuffer
		vuppSmp = new CVuppSample[nbrOfvuppSmp * 2];
		vuppSmpInitialized = true;		
		// vuppSmp can be NULL if there isn't enough memory to allocate
		if(vuppSmp == NULL) 
			;//MessageBox(NULL, "Error: not enough memory to create vuppSmp", "VUPP", MB_OK);

		// Creating the buffer array 
		for(int i = 0; i < nbrOfvuppSmp*2; i++) 
			vuppSmp[i].initializeBuffer(nbrOfSamples); 

		// Creating the processing class with a big buffer
		int bigBufferSize = nbrOfSamples * nbrOfvuppSmp;

		//ke20030623
		if (vuppen) {
			delete vuppen;
		}

		vuppen = new CVupp(bigBufferSize, pWFEXIn);

		vuppen->crossfadeSpeed = this->crossfadeSpeed;
		vuppen->cycleSpeed = this->cycleSpeed;
		vuppen->windowLength = this->windowLength;
		vuppen->silenceRemainderLength = this->silenceRemainderLength;
		vuppen->silenceThresholdPercent = this->silenceThresholdPercent;
		vuppen->silenceSpeed = this->silenceSpeed;

		vuppen->nbrOfVSamplesIn = nbrOfvuppSmp;
	}

	// Storing the sample in a buffer.
	vuppSmp[MSCounter].saveMediaSample(pIn);
	
	MSCounter++;
	
	static long dataCounter=0;
	dataCounter += pIn->GetActualDataLength();

	// Processing all buffered samples and pouring them downstream
	if(MSCounter == nbrOfvuppSmp)
	{	
		int msc;
		// Fill the big buffer with the small samples
		// Cleaning up the bigBuffeIn		
		vuppen->fillBigBuffer(vuppSmp, nbrOfvuppSmp);

		// Transform the big buffer
		vuppen->transform();

		// Copy from the big buffer to the small ones.
		vuppen->copyFromBigBuffer(vuppSmp);

		// Pour the small smaples downstream
		for(msc = 0; msc < vuppen->nbrOfVSamplesOut; msc++)
		{

			IMediaSample *pOut;
			m_pOutput->GetDeliveryBuffer(&pOut, NULL, NULL, 0);

			vuppSmp[msc].loadMediaSample(pOut);

			// FULHACK DETTA SABBAR TIDSANGIVELSEN I MP3-STRÖMMAR
			// Försöker rätta till chipmunkfel
	        CMediaType mt;
		    GetMediaType (0, &mt);
			pOut->SetMediaType (&mt);
			
			m_pOutput->Deliver(pOut);

			pOut->Release();
		}

		// Prepare to start buffering again.
		MSCounter = 0;
		
		return S_OK;
	}


	return S_OK;
}


STDMETHODIMP CTransformSample::setCrossFadeSpeed(double speed)
{
	this->crossfadeSpeed = speed;
	if (vuppen) vuppen->crossfadeSpeed = speed;	
	return S_OK;
}


STDMETHODIMP CTransformSample::setCycleSpeed(short speed)
/* 1 < speed < 300 (100 == normal) */
{
	this->cycleSpeed = speed;
	if (vuppen) vuppen->cycleSpeed = speed;
	return S_OK;
}

STDMETHODIMP CTransformSample::setSilenceLoudnessThreshold(short threshold)
/* 0 < threshold < 100 (percentage of volume) */
{
	this->silenceThresholdPercent = threshold;
	if (vuppen) vuppen->silenceThresholdPercent = threshold;
	return S_OK;
}

STDMETHODIMP CTransformSample::setSilenceRemainderLength(short length)
/* 500 < length < 1000 (?) */
{
	this->silenceRemainderLength = length;
	if (vuppen) vuppen->silenceRemainderLength = length;
	return S_OK;
}

STDMETHODIMP CTransformSample::setSilenceSpeed(short speed)
/* 100 < speed < 2000 (?) (percentage?) */
{
	this->silenceSpeed = speed;
	if (vuppen) vuppen->silenceSpeed = speed;
	return S_OK;
}

STDMETHODIMP CTransformSample::setWindowLength(int length)
/* 1500 < length < 3000 (?) */
{
	this->windowLength = length;
	if (vuppen) vuppen->windowLength = length;
	return S_OK;
}

STDMETHODIMP CTransformSample::getPosition(LONGLONG &position)
{
/*	if (!IsActive()) { position = 0; return S_OK; }

	CRefTime streamTime;
	StreamTime(streamTime);
	
	REFERENCE_TIME time;
	m_pClock->GetTime(&time);
	time = time + streamTime->m_time;

	position = timeQueue.RemoveTillCurrentTime(time);
*/


/*
	if (mediaPosition < 0)
		position = 0;
	else {
		LONGLONG currentTime, pos, speed;
		pos = 0; speed = 0;

		m_pClock->GetTime(&currentTime);
		timeQueue.RemoveTillCurrentTime2(currentTime - m_tStart, pos, speed);
		mediaPosition = pos; // + (pos - (currentTime - m_tStart));
		if (mediaPosition < 0) mediaPosition = 0;
		position = mediaPosition;
	}
*/
	return S_OK;
}

HRESULT CTransformSample::EndOfStream(void)
{	
	for(int msc = 0; msc < MSCounter; msc++)
	{
		IMediaSample *pOut;
		m_pOutput->GetDeliveryBuffer(&pOut, NULL, NULL, 0);
		
		vuppSmp[msc].loadMediaSample(pOut);

		m_pOutput->Deliver(pOut);
		pOut->Release();
	}
		
	MSCounter = 0;
	
	HRESULT hr = NOERROR;
    if (m_pOutput != NULL) {
        hr = m_pOutput->DeliverEndOfStream();
    }

    return hr;
}



