// Vupp1.cpp: implementation of the CVupp class.
//
//////////////////////////////////////////////////////////////////////

//#include "crossfade.cpp"
#include <math.h>
#include "Vupp1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVupp::CVupp(int vuppBufferSize, WAVEFORMATEX *m_pwfxInInit)
{
	// Initializing the buffer that will hold the sample data
	// that will be processed
	bufferSizeIn = vuppBufferSize;
	bufferIn = new short[bufferSizeIn];

	// Additionally we need a buffer to hold the processed data
	// This buffer may become bigger or smaller than the incoming buffer,
	// but it will never be bigger than twice the size.
	bufferSizeOut  = bufferSizeIn * 2;
	bufferSizeTemp1 = bufferSizeIn * 2;
	bufferSizeTemp2 = bufferSizeIn * 2;
	bufferOut  = new short[bufferSizeOut];
	bufferTemp1 = new short[bufferSizeOut];
	bufferTemp2 = new short[bufferSizeOut];
	nbrOfVSamplesIn = 0;
	nbrOfVSamplesOut = 0;

	// Sätter ett initialt värde pEsoundVolume.
	soundVolume = 200;

	// FULHACK - Det blir fel om samplerate byts efter att Vuppen är startad
	m_pwfxIn = m_pwfxInInit;

}

CVupp::~CVupp()
{
	delete[] bufferIn;
	delete[] bufferOut;
	delete[] bufferTemp1;
	delete[] bufferTemp2;
}

void CVupp::transform()
{
	// The sound volume is roughly an average of the sound volume
	// over 4 seconds.

	// x^y = 0.1
	// y is the number of blocks per 4 seconds, i.e. 4 * samplerate / ccIn 
	// x = y-th root from 0.1
	double antalSjok = 4 * m_pwfxIn->nSamplesPerSec / (double)bufferSizeIn;	
	double x = pow(0.1, 1.0/antalSjok);
	
	soundVolume = (long)(soundVolume * x + (1-x) * getSoundVolume(bufferIn, bufferSizeIn));
	soundVolume = max(300, soundVolume); // Makes sure that the sound volume is never lower than 300

	findAndCutCycles(     bufferIn, bufferTemp1, bufferSizeIn,    bufferSizeTemp1);
	crossFadeBigBuffer(bufferTemp1, bufferTemp2, bufferSizeTemp1, bufferSizeTemp2);	
	shortenPauses(     bufferTemp2, bufferOut,   bufferSizeTemp2, bufferSizeOut);
}

void CVupp::fillBigBuffer(CVuppSample *vSample, long nbrOfvSamples)
{

	int smp, bufSize, bigBufferSize;

	bigBufferSize = 0;
	int bigbufferIndex = 0;
	for(int i = 0; i < nbrOfvSamples; i++)
	{
		bufSize = vSample[i].bufferSize;
		bigBufferSize += bufSize;
		for(smp = 0; smp < bufSize; smp++)
		{
			bufferIn[bigbufferIndex] = vSample[i].buffer[smp];
			bigbufferIndex++;
		}
		char buf[200];
		sprintf(buf, "nbrOfvSamples:%d, bufferSizeIn:%d \n bufSize:%d", nbrOfvSamples, bufferSizeIn, bufSize);
	}
	bufferSizeIn = bigBufferSize;
	
}


void CVupp::copyFromBigBuffer(CVuppSample *vSample)
{
	// Denna funktion kopierar den stora buffern till smEbuffrar
	// Den passar ocksEpEatt fixa till timestamps

	int smp, smpIndex, bufSize;
	LONGLONG pStartTime, pEndTime, pDeltaTime;

	if(bufferSizeOut > bufferSizeIn)
		nbrOfVSamplesOut = nbrOfVSamplesIn * 2;
	else
		nbrOfVSamplesOut = nbrOfVSamplesIn;			


	bufSize = bufferSizeOut / nbrOfVSamplesOut;

	// Get the time
	pStartTime = vSample[0].getStartTime();
	pEndTime = vSample[nbrOfVSamplesIn-1].getEndTime();
	pDeltaTime = (pEndTime - pStartTime) / nbrOfVSamplesOut;

	smpIndex = 0;
	for(int i = 0; i < nbrOfVSamplesOut; i++)
	{
		// Compensating for round off errors by changing the size
		// of the last VSample
		if(i == nbrOfVSamplesOut-1)
		{
			int roundOffError;
			roundOffError = bufferSizeOut - bufSize * nbrOfVSamplesOut;
			bufSize += roundOffError;
		}
		for(smp = 0; smp < bufSize; smp++)
		{
			if(smpIndex < bufferSizeOut)
				vSample[i].buffer[smp] = bufferOut[smpIndex];
			else
				;//MessageBox(NULL, "error i copyFromBuffer", "VUPP", MB_OK);
			smpIndex++;
		}

		vSample[i].bufferSize = bufSize;
		vSample[i].setStartTime(pStartTime + pDeltaTime * i);
		vSample[i].setEndTime(pStartTime + pDeltaTime * (i+1));
		
	}
	// Kompenserar för eventuella avrundningsfel i algoritmen
	vSample[nbrOfVSamplesOut-1].setEndTime(pEndTime);
}
/*
void CVupp::dbgCpy(IMediaSample *pMediaSampleIn, IMediaSample *pMediaSampleOut)
{
	short *psIn, *psOut;
	unsigned int ccIn; // Number of samples in buffer	

	// Get pointer to sound buffers. Both input and output
	if(S_OK  != pMediaSampleIn->GetPointer((unsigned char**) &psIn))
		MessageBox(NULL, "Error getPointer", "VUPP", MB_OK);

	if(S_OK  != pMediaSampleOut->GetPointer((unsigned char**) &psOut))
		MessageBox(NULL, "Error getPointer", "VUPP", MB_OK);

	// Converting from bytes to number of samples.
	// We assume the audio stream to have mono 16 bit, which 
	// means 2 bytes per sample.

	ccIn   = pMediaSampleIn->GetActualDataLength()  / 2; // (ljud är 16 bitar mono)
	pMediaSampleOut->SetActualDataLength(ccIn * 2); // Ny längd pEoutput

	// Copy the samples 
	while(ccIn--)
	{
		psOut[ccIn] = psIn[ccIn] ;
	}

}

//////////////////////////////////////////////////////////////////////////////
// copyTime
//
// This function copies timestamps and other relevant non-sample information
// from one mediasample to another.

void CVupp::copyTime(IMediaSample *pMediaSampleIn, IMediaSample *pMediaSampleOut)
{
	LONGLONG pStartMediaTime, pEndMediaTime, pStartTime, pEndTime;
	bool syncPoint;
	AM_MEDIA_TYPE *pMediaType;


	pMediaSampleIn->GetTime(&pStartTime, &pEndTime);
	pMediaSampleIn->GetMediaTime(&pStartMediaTime, &pEndMediaTime);	
	pMediaSampleIn->GetMediaType(&pMediaType);
	syncPoint = (S_OK == pMediaSampleIn->IsSyncPoint());

	pMediaSampleOut->SetTime(&pStartTime, &pEndTime);
	pMediaSampleOut->SetMediaTime(&pStartMediaTime,&pEndMediaTime);
	pMediaSampleOut->SetMediaType(pMediaType);
	pMediaSampleOut->SetSyncPoint(syncPoint);
}
*/
//////////////////////////////////////////////////////////////////////////////
// crossFadeBigBuffer
//
// This function can either make the signal longer or shorter.
// It does so with crossfading. A method that completely disregards
// the content of the sound. The algorithm is fast and guarantees 
// a certain ratio of lengthening or shortening.
//
void CVupp::crossFadeBigBuffer(
			const short *psIn, // The original buffer to be processed
			short *psOut,      // The destination for the processed buffer
			long sizeIn,       // The size of the original buffer
			long &sizeOut)	   // The size of the processed buffer, 
{							   // which may be slightly altered
	


	// crossfade cannot change the size to more than 200% or less than 50%	
	sizeOut = (long)(sizeIn / crossfadeSpeed);
	sizeOut = min((long)(sizeIn * 1.9 - 1), sizeOut);
	sizeOut = max((long)(sizeIn / 1.9 + 1), sizeOut);

	const short *bufIn, *bufInBackup;
	short  *bufOut, *bufOutBackup;
	bufIn = bufInBackup = psIn;
	bufOut = bufOutBackup = psOut;

	int maxBlockSize;

	// Here I retrieve the windowlength from the registry.
	// I also compensate for other samplerates than 44100.
	//maxBlockSize = getCrossfadeWindowlengthFromRegistry() * m_pwfxIn->nSamplesPerSec / 44100;
	
	// OBS FULHACK, hårdkodning
	/* ke20030611
	maxBlockSize = 2000;
    */
	maxBlockSize = windowLength;

    if (sizeIn > sizeOut) // If the speed is larger than 100
    {
		// This is a picture to explain the crossfade procedure when reducing block sizes
		// 
		//		
		//                         center
		// o------------------------------------------------------o
		// |                         |                            | Original sound block
		// o------------------------------------------------------o
		//
		//
		// o-------------------------o
		// |     |\\\fade out area\\\| Left half of original block
		// o-------------------------o  
		//       o-------------------------o
		//       |///fade in area////|     |  Right half of original block
		//       o-------------------------o  
		//
		//
		// o-------------------------------o
		// |     |--crossfaded area--|     |  New smaller sound block
		// o-------------------------------o
		//
		//
		// This procedure works best if the original block size is less than that of a 
		// single phonem. Usually 3000 samples at 44.1kHz is good. The translates to
		// about 70 ms. At high speeds you usually have to reduce the window size.
		// At slow speeds you can increase them.
		// A very large block will result in echo and unintelligable speech, while 
		// a very small block will result in a squeaky chipmunk voice.
		

		// Since the buffer usually is much larger than the maximum block 
		// size it is divided into a number of blocks
		// Floor does the same as it does in Matlab, that is, round down.
		int numberOfBlocks, blockSize;
		
		numberOfBlocks = (int)floor((double)sizeIn / (double)maxBlockSize);
		
		
		// There must be at least one block. Usually the number of blocks
		// is bigger than zero.
		if(numberOfBlocks != 0 )
			blockSize = sizeIn / numberOfBlocks;
		else
		{
			blockSize = sizeIn;
			numberOfBlocks = 1;
		}
		
		int sideBlockSize,  centerBlockSize, shortBlockSize, blocknr, ss, smpl;

		shortBlockSize  = (int)(blockSize * sizeOut / (float)sizeIn);      // New smaller sound block
		sideBlockSize   = (int)((shortBlockSize * 2 - blockSize ) / 2);// Unfaded area	
		centerBlockSize = (int)(shortBlockSize - sideBlockSize * 2);   // Fade area
		if (centerBlockSize <= 0)
			centerBlockSize = 1;

		// A whole sinc window looks something like this
		//
		// 1.0          *****
		//            **     ** 
		// 0.5       *         * 
		//         **           ** 
		// 0.0  ***               ***

		// Creates half a sinc window
		double *window = new double[centerBlockSize];
		for(int windex = 0; windex < centerBlockSize; windex++) 
			window[windex]  = sin(3.14159265358*windex/(float)centerBlockSize-3.14159265358/2.0)/2.0+0.5;
	
		for (blocknr = 0; blocknr < numberOfBlocks; blocknr++)
		{
		
			// Due to round off errors, all blocks will not be equal in size.
			// The size of the last block will compensate.
			if(blocknr == numberOfBlocks-1)
			{

				blockSize = sizeIn -(bufIn - bufInBackup);
				shortBlockSize = (sizeOut) - (bufOut - bufOutBackup);
				sideBlockSize = (int)floor((shortBlockSize * 2 - blockSize) / 2.0);		
				centerBlockSize = (int)(shortBlockSize - sideBlockSize * 2);

				if (centerBlockSize > 0)
				{
					delete[] window;					
					window = new double[centerBlockSize];
					for(int windex = 0; windex < centerBlockSize; windex++) // sinuswindow
						window[windex]  = sin(3.14159265358*windex/(float)centerBlockSize-3.14159265358/2.0)/2.0+0.5;
				}

			}

			// Copying unfaded part of left half of original block
			copyWavBlock(&bufIn, &bufOut, sideBlockSize);

			// Copying faded part of left half of original block
			int sampel = 0;
			for(smpl = centerBlockSize-1; smpl >= 0; smpl--)
			{
				ss = bufIn[0];
				// Multiplicate with window to fade out.
				ss = (int)(ss * window[smpl]);
				bufOut[0] = (short)ss;
				bufIn++;
				bufOut++;

			}
			
			// Moving outPointer back in time to be able to add more faded sound to the same place
			bufOut -= centerBlockSize;

			// Adding faded part of right half of original block
			for(smpl = 0; smpl < centerBlockSize; smpl++)
			{
				ss = bufIn[0];
				// Multiplicate with window to fade in.
				ss = (int)(ss*window[smpl]);
				bufOut[0] += (short)ss;

				bufIn++;  // Jump to next sample
				bufOut++; 
			}


			// Copying unfaded part of right half of original block
			copyWavBlock(&bufIn, &bufOut, sideBlockSize);			

		}
		sizeOut--;
		delete[] window;
    }
    else // If the speed is less or equal to 100
    {
		// This is a picture to explain the crossfade procedure when increasing block sizes
		// The method is very similar to that of increasing size.
		//		
		// o------------------------------------------o
		// |                                          | Original sound block
		// o------------------------------------------o
		//
		//
		//                         
		// o------------------------------------------o
		// |                  |\\\\fade out area\\\\\\| Original sound block
		// o------------------------------------------o
		//                    o------------------------------------------o
		//                    |////fade in area///////|                  | Original sound block
		//                    o------------------------------------------o
		//
		//
		// o-------------------------------------------------------------o
		// |                  |----crossfaded area----|                  | New bigger sound block
		// o-------------------------------------------------------------o
		//
		//
		
		int numberOfBlocks, blockSize;
		
		// Since the buffer usually is much larger than the maximum block 
		// size it is divided into a number of blocks
		// Floor does the same as it does in Matlab, that is, round down.
		numberOfBlocks = (int)floor(sizeOut / (double)maxBlockSize);

		
		// There must be at least one block. Usually the number of blocks
		// is bigger than zero.
		if(numberOfBlocks != 0 )
			blockSize = (int)floor(sizeOut / (double)numberOfBlocks);
		else {
			blockSize = sizeOut;
			numberOfBlocks = 1;
		}
		
		int sideBlockSize, centerBlockSize, shortBlockSize, smpl, blocknr, ss;
		double sinc;
				
		// Some filtergraphs doesn't allow buffers to grow. Instead you have to use
		// less of the input buffer and report that. This method does just that.		
		shortBlockSize  = (int)(blockSize * sizeIn / (float)sizeOut); // Smaller block than original size
		sideBlockSize   = blockSize - shortBlockSize;             // Unfaded area
		centerBlockSize = blockSize - sideBlockSize * 2;          // Crossfade area

		for (blocknr = 0; blocknr < numberOfBlocks; blocknr++)
		{
			// Due to round off errors, all blocks will not be equal in size.
			// The size of the last block will compensate.
			if(blocknr == numberOfBlocks-1)
			{
				blockSize = sizeOut - (bufOut - bufOutBackup); // fönsterlängd minus avverkad längd
				shortBlockSize = sizeIn - (bufIn - bufInBackup);

				sideBlockSize = (int)(blockSize - shortBlockSize);		
				centerBlockSize = (int)(blockSize - sideBlockSize * 2);
			}

			
			// Copying left side of the unfaded area of the original block
			copyWavBlock(&bufIn, &bufOut, sideBlockSize);

			
			// Copying the first faded area of the original block
			for(smpl = centerBlockSize; smpl > 0; smpl--)
			{
				ss = bufIn[0];				
				// multiplicating with sinc window
				sinc = sin(3.14159265358*(smpl-1)/(float)centerBlockSize-3.14159265358/2.0)/2.0+0.5;
				ss = (int)(ss * sinc);

				bufOut[0] = (short)ss;
				
				bufIn++; // Jump to the next sample position
				bufOut++; 
			}
			// Here we have copied the entire original block.
			// Moving back in-pointer to the beginning. 
			// Moving back out-pointer to where the second faded block starts
			bufIn -= sideBlockSize;
			bufIn -= centerBlockSize;
			bufOut -= centerBlockSize;

			// Adding second faded block to sound.
			for(smpl = 0; smpl < centerBlockSize; smpl++)
			{
				ss = bufIn[0];
				// multiplicate with sinc window
				sinc = sin(3.14159265358*(smpl)/(float)centerBlockSize-3.14159265358/2.0)/2.0+0.5;
				ss = (int)(ss * sinc);

				bufOut[0] += (short)ss;

				bufIn++; // Jump to next sample position
				bufOut++; 
			}

			// Copying right side of the unfaded area of the original block
			copyWavBlock(&bufIn, &bufOut, sideBlockSize);
		}    
	}

}

//////////////////////////////////////////////////////////////////////////////
// copyWavBlock
//
// This function takes two pointers to sample-data. It copies a number of them.
// The number of samples copied is controlled by antalSamples.
// In any event only one channel is copied, but the information is neccessary
// since the left and right channel are interleaved in stereo data.

void CVupp::copyWavBlock(const short **psInOrig, short **psOutOrig, int antalSamples)
{
	const short *psIn = *psInOrig;
	short *psOut = *psOutOrig;

	for(int smpl = 0; smpl < antalSamples; smpl++) // Copies a bunch of samples
	{
		psOut[0] = psIn[0];
		psIn++;  // Jump to the next sample position
		psOut++; 
	}
	
	*psOutOrig = psOut; // Copy the pointers to new position
	*psInOrig = psIn;
}

//////////////////////////////////////////////////////////////////////////////
// shortenPauses
//
// This function finds places in the signal where the sound volume is low.
// These parts are then smoothly reduced in size.
//
void CVupp::shortenPauses(
			const short *psIn, // The original buffer to be processed
			short *psOut,      // The destination for the processed buffer
			long sizeIn,       // The size of the original buffer
			long &sizeOut)	   // A pointer to report the new size
{

	const short *pssIn;
	
	// We start by searching for periods of silence that are long enough.
	// We then remove some of them, leaving a set length that is crossfaded
	// to reduce audible aliasing.
	short silenceTroskel; // 0 - 32768 (sound volume for 16 bit sound)	
	long silenceRatio, ss;
	int endLength, smpl, ssmpl, silenceLength;
	
    silenceRatio = (long)(10000.0 / silenceSpeed);   
	silenceTroskel = soundVolume * silenceThresholdPercent / 100;

	// Compensating so that the amount of time that is left after a cut
	// is the same, no matter what samplerate is used.
    endLength = silenceRemainderLength * m_pwfxIn->nSamplesPerSec / 44100;
	
	double *window = new double[endLength]; // Creating half a sinc window
	for(int windex = 0; windex < endLength; windex++) 
		window[windex]  = sin(3.14159265358*windex/(float)endLength-3.14159265358/2.0)/2.0+0.5;
	
	int shortenLength=0;	
	
	for(smpl = 0; smpl < sizeIn; true)
	{
		// Search for silence
		ss = psIn[0];
		silenceLength = 0;
		ssmpl=smpl;
		pssIn = psIn;
		// If the samplevalue is less than the threshold value and we're
		// still within the allocated memory block, then increase silenceLength
		while(abs(ss) < silenceTroskel    && ssmpl < sizeIn-1)
		{
			pssIn++;
			ss = pssIn[0];
			silenceLength++;
			ssmpl++;
		}
		
		// Now we know how long this silence is. The next thing to determine 
		// is how much to cut away. We have to consider both remainderlength
		// and silence_ratio. 
		long tempEndLength;
		tempEndLength = (long)(silenceLength * (silenceRatio / 100.0) * (silenceRatio / 100.0));
		endLength = max(endLength,tempEndLength);

		// If we are going to cut anything away
		if(silenceLength > endLength * 2)
		{
			delete[] window;
			window = new double[endLength]; // Creating half a sinc window
			for(int windex = 0; windex < endLength; windex++) 
				window[windex]  = sin(3.14159265358*windex/(float)endLength-3.14159265358/2.0)/2.0+0.5;
						
			// Copy the left part of the silence and fade it out with the window
			for(ssmpl = 0; ssmpl < endLength; ssmpl++)
			{                             // mirroring window
				psOut[0] = (short)(psIn[0] * window[endLength -1 -ssmpl]);	
				psIn++;
				psOut++;
			}
			// Skip the middle part of the silence
			psIn += (silenceLength - endLength * 2);
			shortenLength += (silenceLength - endLength);
			psOut -=  endLength;

			// Fade in the right part of the silence and add it to the left part.
			for(ssmpl = 0; ssmpl < endLength; ssmpl++)
			{
				psOut[0] += (short)(psIn[0] * window[ssmpl]);	
				psIn++;
				psOut++;
			}
			smpl += silenceLength;	
		
		}
		else // If the pause is too short to process it will just be copied instead
		{
			if (silenceLength == 0) // If the sound wasn't silent at all we 
				silenceLength = 1;  // will still copy one sample

			// Copying samples
			for(ssmpl = 0; ssmpl < silenceLength; ssmpl++)
			{
				psOut[0] = psIn[0];		
				psIn++;
				psOut++;
				smpl++;
			}
		}
	}
	delete[] window;

	// Reducing the size of the big outputbuffer
	sizeOut = sizeIn - shortenLength;

}

//////////////////////////////////////////////////////////////////////////////
// getSoundVolume
//
// This function analyzes sound data and computes a loudness value
// It does this by calculating an average of the absolute value of a bunch of samples
//
long CVupp::getSoundVolume(
    short *psIn,			// pointer at indata
    long    sizeIn)			// length of indata
	
{
	long average = 0; // The data type long is 4 byte and can conatain 
	                     // numbers up to 2.147.483.648
							
	if(sizeIn > 65535)  // This guarantees that average never will become 
		sizeIn = 65535; // larger than 2.147.483.648


	for(int smpl = 0; smpl < sizeIn; smpl++)
	{
		average += abs(psIn[0]);
		psIn++;
	}
	average /= sizeIn;
	
	return (long)average;
}

//////////////////////////////////////////////////////////////////////////////
// normalizeToOne    
//
// This function takes a matrix of type double and normalizes so that the highest number
// in the matrix is one.

void CVupp::normalizeToOne(double * data, long len)
{
	// Searching for the highest value in the matrix
	double peak=0;
	int smpl;
	for(smpl = 0; smpl<len; smpl++)
	{
		if(fabs(data[smpl])>peak)
			peak = fabs(data[smpl]);
	}

	// Divides with the highes valuet, which normalizes the values in the matrix to one.
	for(smpl = 0; smpl<len; smpl++)
	{
		data[smpl] = data[smpl] / peak;					
	}
}

//////////////////////////////////////////////////////////////////////////////
// sign
//
// This checks a number to see if it is positive or negative. It returns either 1 or -1.
//
short CVupp::sign(short tal)
{
	if(tal>0)
		return 1;
	if(tal<0)
		return -1;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// harmonicPowerSpectrum
//
// This function analyzes voice data and tries to figure out its fundamental frequency.
//
// It uses a Fourier transform to get the frequency information. It then uses
// a subsampling overlap technique called Harmonic Power Spectrum to find 
// the fundamental frequency. The frequency is then limited to values within reason.
//
long CVupp::harmonicPowerSpectrum(
    const short* psIn,            // Pointer at indata
    long    sizeIn)            // The length of indata
{
	// In order to save clock-cycles the fourier transform is only performed
	// on the first 4096 samples of the buffer.

	// Planen här är att göra en frekvensanalys över hela blocket. Sedan ska ett medelvärde
	// räknas ut. framelength behöver inte var sEstort för noggrannheten behöver inte
	// vara större än en halv oktav. Anledningen till att vi vill hitta grundfrekvensen
	// är för att vi vill kunna göra ett lågpassfilter vid rätt rätt frekvens, för att
	// fEred pEvar cykler börjar och slutar.

	int  smpl, framelength;
	
	framelength = 4096;
	while(framelength > sizeIn) // This is a security measure to ensure that we don't read 
		framelength /= 2;     // outside the allocated memory.

	

	const short* psFrek = psIn; // New pointer to sound data

	double *frekData = new double[framelength * 2];
	double *hps = new double[framelength/2];

	// Creates a sinc window
	double *window = new double[framelength];
	for(long windex = 0; windex < framelength; windex++) // sinuswindow
		window[windex]  = sin(2 * 3.14159265358*windex/(float)framelength-3.14159265358/2.0)/2.0+0.5;

	// Grabs a chunk of sound and multiplies it with the sinc window to reduce
	// aliasing in the frequency transform.
	for(smpl = 0; smpl < framelength; smpl++)
	{
		// Man borde nog multiplicera med ett fönster
		frekData[smpl*2] = double(psFrek[0]) * window[smpl];
		frekData[smpl*2+1] = 0;
		psFrek++;
	}


	normalizeToOne(frekData, framelength*2);

	
	// Perform FFT
	smsFft(frekData, framelength-1, -1); // The last value, -1 is for fft. It would 
	                                     // have been 1 for ifft

	double tmpSmpl, divider, multiplier;
	int N, n;
	
	// Initializing the matrix hps
	for(smpl = 0; smpl < framelength/2; smpl++)
		hps[smpl] = 0;
	

	// Subsampling and overlapping frequency information in order to find the fundamental frequency
	for(N = 1; N < 8; N++)
		for(smpl = N; smpl < framelength/(N*2); smpl ++) // En av de tvEspeglarna
		{
			tmpSmpl = 0;
			divider = 0;
			for(n = 0; n < N; n ++) 
			{
				// To save clock cycles we use a square window instead of sinc
				multiplier = 1;
				//multiplier = sin(2 * 3.14159265358*n/(float)N-3.14159265358/2.0)/2.0+0.5;
				divider += multiplier;
				tmpSmpl += fabs(frekData[(smpl*N+n-N/2)*2] * multiplier);
			}
			hps[smpl] += tmpSmpl / divider;
		}

	// Find the highest peak in hps. The idéa being that this is likely to be 
	// the fundamental frequency
	double peak = 0;
	int peakSample;

	for(smpl = 0; smpl<framelength/2; smpl++)
	{
		if(fabs(hps[smpl])>peak)
		{
			peak = fabs(hps[smpl]);
			peakSample = smpl;
		}
	}

	// The idea with multiplying with (m_pwfxIn->nSamplesPerSec / 44100) is to make sure
	// that the period length in milliseconds is the same at any sample frequency.
	long HPS_periodLength = 300 * m_pwfxIn->nSamplesPerSec / 44100; 
	long lengthTemp = framelength / peakSample; 

	if(lengthTemp > 50  * (long)m_pwfxIn->nSamplesPerSec / 44100 &&
	   lengthTemp < 800 * (long)m_pwfxIn->nSamplesPerSec / 44100)
	{
		HPS_periodLength = lengthTemp;
	}

	delete[] frekData;
	delete[] window;
	delete[] hps;

	return HPS_periodLength;
}

//////////////////////////////////////////////////////////////////////////////
// FaltningFindCutPoints
//
// This function receives a block of samples. The samples are smoothed out with a 
// sinc window, i.e. a lowpass function. The length of the window is determined 
// by the "size" variable. 
// The next step in this function is to find local peaks. Peaks occur when the slope
// of the lowpass transformed signal changes from positive to negative. Usually
// there is excatly one such peak per glottal pulse, so that means we find
// glottal pulse boundry's that we can use when cutting. The idéa being that 
// cutting at these points introduces less artifacts than any other point in 
// the signal.
// The function returns an array of cutpoints
intvec* CVupp::faltningFindCutPoints(
    short* psIn, // pointer at indata/ outdata
    long    sizeIn, // Length of indata/outdata
	int smoothSize)
{
	intvec* zpos = new intvec();

	int i, smpl, f;
    short* psCopy; // pointer at outdata

	// Precumputing a sinc window
	// Usually I would build a window of the type float, with numbers ranging
	// from 0.0 to 1.0. But since floating point operations take so much longer
	// to compute than integers I use a range of integers from 0 to 512.
	// This window is used extensively further down.
	long *window = new long[smoothSize];

	for(long  windex = 0; windex < smoothSize; windex++) // sinc window
		window[windex]  = (long)(512 * (sin(2 * 3.14159265358*windex/(float)smoothSize-3.14159265358/2.0)/2.0+0.5)) ;	

	// Preparing an array that will receive the result of a filtering operation.
	// The purpose of the filter is to generate a loudness curve. Thus the 
	// operations involve low-pass filtering and making negative values positive.
	// I'm padding with zeroes in the beginning and at the end.	
	unsigned long *smooth = new unsigned long[sizeIn];
	short *inPad = new short[sizeIn + smoothSize];

	// Initializing inPad
	for(i = 0; i < smoothSize/2; i++)
	{
		inPad[i] = 0;
		inPad[sizeIn +  smoothSize/2 + i] = 0;
	}

	psCopy = psIn;
	for(i = 0; i < sizeIn; i++)
	{
		inPad[smoothSize/2+i] = psCopy[0];
		psCopy++;
	}

	// optimizefactor
	long oFac = smoothSize/10;
	oFac = max(1,oFac);

	// Low-pass positive operation
	for(smpl = 0; smpl < sizeIn; smpl+=oFac)
	{	// Initializing smooth
		smooth[smpl] = 0;
		for(f = 0; f < smoothSize; f++) // The low-pass window is of width "smoothSize"
			smooth[smpl] += abs(inPad[smpl + f] * window[f]); 
	}

	short* psOutTemp = psIn;

	// ***************  PeakFind *************************
	// I define a local peak as a place where the slope of the curve
	// changes from upwards to downwards.
	// I don't search for peaks near the beginning or the end, since it
	// impossible to perform a correct low-pass operation there.

	bool downwards = true;
	
	smpl = 0;
	while(smpl < smoothSize/2)
		smpl+= oFac;

	// Since we've removed high frequency energy from the signal it 
	// makes no sense checking the slope at every sample. The spacing 
	// between checkings is oFac, and is a number that has been tested
	// and proven to give good results.
	for(smpl = smpl; smpl+oFac < sizeIn - smoothSize/2; smpl+= oFac)
	{
		
		// If the last was leaning upwards and this one is down, i.e. a peak.
		if(smooth[smpl+oFac] < smooth[smpl]  && downwards == false) 
		{
			zpos->add(smpl);
		}
		
		if(smooth[smpl+oFac] > smooth[smpl])
			downwards = false;
		else
			downwards = true;
	}

	delete[sizeIn] smooth;
	delete[sizeIn + smoothSize] inPad;
	delete[smoothSize] window;
	
	// Returns a list of points where there are peaks.
	return zpos; 
}

//////////////////////////////////////////////////////////////////////////////
// adderaKlippTillKlippLista
//
// This function is called after FaltningFindCutPoints and it's purpose is to 
// add more cutPoints in order to get a more even result after cutting.
// When FaltningFindCutPoints searches for cutpoints it usually finds them
// clustered together, leaving large holes where no cutting will be performed.
// In order not to destroy the speech rhythm more than necessary we add more 
// cutpoints where they are lacking. This could introduce horrible artifacts
// but that's where the function finjusteraKlippLista steps in and safeguards
// against that.
intvec* CVupp::adderaKlippTillKlippLista(
    long    sizeIn, // The length of indata / outdata
 	intvec* klVec)
{
	// First we determine how large the average distance between
	// two cutpoints is. Then we insert new cutpoints to make sure
	// that there will never be cutpoints separated by more than
	// twice that distance.

	if(klVec->getSize() >= 2)
	{
		// Creating the vector that will be returned.
		intvec* justKLVec = new intvec();
		
		
		// Creating a pointer to the incoming vector
		int  klVecIndex = 0;

		// This vector will contain all the distances between adjacent cutpoints.
		long* klippAvstand = new long[klVec->getSize()-1];
		int klippPosL, klippPosR;

		for(int klipp = 0; klipp < klVec->getSize()-1; klipp++)
		{
			klippPosL = klVec->getAtIndex(klVecIndex);
			klVecIndex++;
			klippPosR = klVec->getAtIndex(klVecIndex);

			klippAvstand[klipp] = klippPosR - klippPosL;
		}

		// Sorting the cutpoint distance list, to determine the average distance.
		long tempAvstand, i, j;

		for(j = 0; j < klVec->getSize()-1; j++)
		{
			for(i =0 ; i < (klVec->getSize()-2); i++)
			{
				if(klippAvstand[i+1] > klippAvstand[i])
				{
					tempAvstand = klippAvstand[i];
					klippAvstand[i] = klippAvstand[i+1];
					klippAvstand[i+1] = tempAvstand;
				}
			}
		}

		long medianKlippAvstand;

		// This is the average cutpoint distance.
		medianKlippAvstand = klippAvstand[(klVec->getSize()-1) / 2];
		
		delete[] klippAvstand;

		// Now we search for cutpoint distances larger than twice the
		// average distance. When we find them we will add new cutpoints
		// in between them.
		// We also add cutpoints near the beginning and near the end.		

		long antalExtraKlipp, klippLength, pos;
		klVecIndex = 0;
		for(int klipp = 0; klipp < klVec->getSize()+1; klipp++)
		{
			if(klipp == 0) // Adding a cutpoint at the beginning of the block
				klippPosL = 0;
			else
			{
				klippPosL = klVec->getAtIndex(klVecIndex);
				klVecIndex++;
			}

			if(klipp == klVec->getSize())  // Adding a cutpoint at the end of the block
				klippPosR = sizeIn;
			else
				klippPosR = klVec->getAtIndex(klVecIndex);

			klippLength = klippPosR - klippPosL;


			justKLVec->add(klippPosL);
			if(klippLength > medianKlippAvstand*2)
			{
				antalExtraKlipp = klippLength / medianKlippAvstand;

				// Adding cutpoints
				for(i = 0; i < antalExtraKlipp; i++)
				{
					pos = klippPosL + i * klippLength / antalExtraKlipp;
					justKLVec->add(pos);
				}
				
			}
			
		}


		return justKLVec;
	}
	else
		return klVec;
}

//////////////////////////////////////////////////////////////////////////////
// finjusteraKlippLista
//
// This function adjusts the position of cutpoints to reduce artifacts in the
// transformed signal. It does so in a range of different ways. 
// They are listed below.
// - Move the cutpoints to the closest zeroCrossing
// - Check continuity. If the cutaway part is similar to adjacent parts of the speech
//   the cut is valid. If not, the cut is invalidated.
// - If the number of zeroCrossings in the cutaway part is large it probably consists
//   of consonants which are chaotic and the cut is always deemed valid.
// - If the cutaway part is very small in time it is invalidated
// - If different cutaway parts overlap after adjustment thay are invalidated.

intvec* CVupp::finjusteraKlippLista(
	const short* psIn, // pointer at indata
    long    sizeIn, // The length of indata
	intvec* klVec)
{
	// We have found that the best place to make a cut is where the signal
	// crosses the zero-boundary. We call this a zero-crossing.
	// The first part of this function moves cut-points so they are as close
	// to a zero-crossing as possible.

	int firstValue, secondValue, firstPos, secondPos, nyFirstPos, nySecondPos;
	int leftDelta, rightDelta, rightDelta1, rightDelta2, smpl;
	int oldFirstPos, oldSecondPos;
	oldFirstPos= 0;
	oldSecondPos = 0;

	short leftLutningTecken;
	bool giltig;
	intvec* justKLVec = new intvec();
	
	int klVecIndex = 0;

	for(int iter=0; iter < klVec->getSize()/2; iter++)
	{
		giltig = true;
		firstPos = klVec->getAtIndex(klVecIndex); // The sampleposition at the left cutpoint
		firstValue = psIn[firstPos]; // The samplevalue at the left cutpoint

		klVecIndex++;
		
		secondPos = klVec->getAtIndex(klVecIndex); // The sampleposition at the right cutpoint
		secondValue = psIn[secondPos]; // The samplevalue at the right cutpoint


		// Trying to move the left cutpoint to the closest cutpoint on the left hand.
		// ************  Left cutpoint  ***************
		short tecken;

		nyFirstPos = firstPos;
		leftDelta = 0;
		tecken = sign(psIn[firstPos]);
		// While the sign remains unchanged
		while(sign(psIn[(firstPos - leftDelta)]) == tecken)
		{
			leftDelta++;
			// We must never get cutpoints on negative positions
			if(firstPos - leftDelta <= 0) 
			{
				leftDelta = firstPos;
				break;
			}
		}
		nyFirstPos = firstPos - leftDelta;
		leftLutningTecken = sign(psIn[(nyFirstPos)] - psIn[(nyFirstPos-1)]);

		
		// ***********  Right cutpoint  *******************
		// We can assume that the original distance between the left cutpoint and
		// the right largely correct. We have moved the left cutpoint to the left.
		// That's why we start with moving the right cutpoint to the left with an 
		// equal amount. Then we check both left and right for a suitable zero-
		// crossing. A suitable zero-crossing should have the same sign on the
		// slope as the zerocrossing at the left cutpoint.

	
		rightDelta1 = leftDelta;
		rightDelta2 = leftDelta;
		tecken = sign(psIn[(secondPos - leftDelta)]);
		
		// Seeking a zero-crossing at the left
		while(sign(psIn[(secondPos - rightDelta1)]) == tecken)
		{
			rightDelta1++;
			// We must never get cutpoints on negative positions
			if(rightDelta1 > secondPos) 
			{
				rightDelta1 = 0;
				break;
			}
		}

		// Seeking a zero-crossing at the right
		while(sign(psIn[(secondPos - rightDelta2)]) == tecken)
		{
			rightDelta2--;
			// We must never get cutpoints on negative positions
			if(secondPos - rightDelta2 >= sizeIn) 
			{
				rightDelta2 = 0;
				break;
			}
		}

		// The closest zero-crossing is chosen.
		if(abs(leftDelta-rightDelta1) < abs(leftDelta-rightDelta2))
			rightDelta = rightDelta1;
		else
			rightDelta = rightDelta2;
		
		// The new position of the right zero-crossing is set.
		nySecondPos = secondPos - rightDelta;

		// ****** Validation - no zero-cross *******************************
		// If there are no zero-crossings between left and right cutpoint we
		// declare the cut invalid. Think about this. No zerocrossings in a 
		// vowel usually means that we're trying to cut half a glottal pulse.

		tecken = sign(psIn[(nyFirstPos+2)]);
		long teckenCounter = 0;
		for(smpl = nyFirstPos+2; smpl < nySecondPos-2; smpl++)
			if(tecken != sign(psIn[(smpl)]))
			{
				teckenCounter++;
				tecken = sign(psIn[(smpl)]);
			}
		if(teckenCounter < 1)
			giltig = false;

		// ****** Validation - redundance **********************************
		// If the area surrounding the cut looks a lot like the area between
		// the cutpoints, we're guaranteed to have good cutpoints in a vowel.
		// If it differs too much we declare the cut invalid.

		double olikhet = 0;
		double summa = 0;

		int klippLength = (nySecondPos - nyFirstPos); 

		for(smpl = 0 ; smpl < klippLength/2 && smpl+nySecondPos < sizeIn; smpl++)// Kollar halva cykeln
		{
			olikhet += abs(psIn[(nyFirstPos+smpl)] - psIn[(nySecondPos+smpl)]);
			summa += abs(psIn[(nyFirstPos+smpl)]);
		}
		olikhet = olikhet / (klippLength/2);
		summa   = summa   / (klippLength/2);
		
		if(olikhet > summa )
			giltig = false;

		// ****** Validation - lots of zero-crossings ********************
		// If there is a high frequency of zero-crossings between the left
		// and the right cutpoint we are not cutting in a vowel. That 
		// leaves silence and consonants. At these places it is not so 
		// important where the cutpoints are, so we validate all cuts
		// in these areas.

		int stmp, antalZeroCrossings, sign;

		sign = 1;
		antalZeroCrossings = 0;
		for(smpl = nyFirstPos; smpl < nySecondPos; smpl++)
		{	
			// Adding zero-crossings to antalZeroCrossings.
			stmp = psIn[smpl];
			if(stmp < 0 && sign == 1)
			{
				antalZeroCrossings++;
				sign = -1;
			}
			if(stmp >0 && sign == -1)
			{
				antalZeroCrossings++;
				sign = 1;
			}
		}

		if(antalZeroCrossings > klippLength/10)	
			giltig = true;
	
		// ****** Validation - too short cut ******************************
		// If the distance between the left and right cutpoints is so small
		// that probably cannot contain a complete glottal pulse we 
		// invalidate the cut.

		if(klippLength < 20 * (long)m_pwfxIn->nSamplesPerSec / 44100)
			giltig = false;

		// ****** Validation - left after right ******************************
		// If the left cutpoint is after the rigt one, we invalidate the cut.
		// This can occur at the zero-cross adjustment at the beginning of 
		// of this procedure.

		if(nyFirstPos <= oldSecondPos)
			giltig = false;
		if(nySecondPos <= nyFirstPos)
			giltig = false;

		// ****** Finished - writing the adjusted value to the vector ********

		// If the cut is valid
		if(giltig)
		{
			justKLVec->add(nyFirstPos);
			
			justKLVec->add(nySecondPos);
			oldFirstPos = nyFirstPos;
			oldSecondPos = nySecondPos;

		}
		klVecIndex++;
	}
	return justKLVec;
}

//////////////////////////////////////////////////////////////////////////////
// cycleCrossFade
//
// Even if we do everything we can to make sure that the cutpoints are as good
// as possible there will always be points where a cut would introduce a click-sound
// that would irritate the listener. This function crossfades the ends of the 
// sound that remains after a cut. The crossFade is performed before the cut is made.
//
void CVupp::cycleCrossFade(
    short*  psIn,   // pointer at outdata/ utdata
    long    sizeIn, // The length of indata / outdata
	intvec* klVec)	
{
	int klVecIndex = 0;

	// Creating a window for cut-smoothing. The width of the window is
	// adjusted to be of equal length measured in milliseconds no 
	// matter what sample-rate is used.
	// It is based on 20 samples at 44.1 kHz.
	int winLength = (int)(20 * m_pwfxIn->nSamplesPerSec / 44100); 

	int firstPos, secondPos;
	double sampleVal;
	double *window = new double[winLength]; // Creating halv a sinc window
	for(int windex = 0; windex < winLength; windex++) // sinc window
		window[windex]  = sin(3.14159265358*windex/(float)winLength-3.14159265358/2.0)/2.0+0.5;


	for(int iter=0; iter < klVec->getSize()/2; iter++)
	{
		firstPos = klVec->getAtIndex(klVecIndex);
		klVecIndex++;

		secondPos = klVec->getAtIndex(klVecIndex);
		klVecIndex++;

		if((secondPos - firstPos) < winLength)
		{
			char buf[300];
			sprintf(buf, "Smoothing overlap.\nwinLength:%d\nsec-first:%d\nklVecIndex:%d", winLength, secondPos - firstPos, klVecIndex);
			//MessageBox(NULL, buf, "VUPP", MB_OK);
			klVec->printvec();
		}

		// Making sure we are not smotthing samples outside our allocated memory.
		if(secondPos + winLength < sizeIn)
		{
			// Crossfading 20 samples after the cut.
			for(int smpl = 0; smpl < winLength; smpl++)
			{
				sampleVal = (double)(psIn[firstPos+smpl])*window[winLength-1-smpl] + (double)(psIn[secondPos+smpl])*window[smpl];
				psIn[secondPos+smpl] = (short)sampleVal;
			}
		}
	}
	delete[] window;
}

//////////////////////////////////////////////////////////////////////////////
// findAndCutCycles
//
// This is the main loop for cutting glottal pulses. A glottal pulse is defined
// as one vibration cycle of the vocal folds.
//
// First the average length of these pulses is estimated by harmonicPowerSpectrum.
// The next step is to find glottal pulses and mark them for cutting.
// Then we find cutpoints inbetween the cutpoints, so that we have a minimal distance
// between any two cutpoints
// Then we adjust the cutpoints to decrease aliasing
// Then a crossfade is made to decrease aliasing even more
// Lastly the cuts are made. 
// The new sound data is returned,along with it's new length
//
void CVupp::findAndCutCycles(
    short* psIn,	 // pointer at outdata/ utdata
    short* psOut,	 // pointer at outdata/ utdata
    long   sizeIn,   // the length of indata / outdata
    long   &sizeOut)  // the length of indata / outdata
{
	sizeOut = sizeIn;
    short* psOutBackup = psOut; // pointer at outdata/ utdata

	int speed = cycleSpeed;
	//ke20030626
	if (speed < 1) speed = 1;
	int ratio = 10000 / speed;

	// If no speed change is requested we just copy all samples.
	if (speed == 100) 
	{
		for (int smpl= 0 ; smpl < sizeIn; smpl++)
		{
			psOut[0] = psIn[0];
			psOut++;
			psIn++;			
		}
	}
	else
	{

		// Gör lågpass och hämta alla zeroCross-positioner.
		// Kompenserar för att fEsamma effekt pEolika samplerate.
		//ke20030625
//		intvec* zeroPos = new intvec();
		intvec* zeroPos;
		intvec* klippLista = new intvec();
		int zeroPosIndex;
		int klippListaIndex;
		long periodLength, smpl;
		// Determining the average vowel period length in the block.

		
		periodLength = harmonicPowerSpectrum(psIn, sizeIn);

		// Finding good cutpoints
		zeroPos = faltningFindCutPoints(psIn, sizeIn, periodLength);

		// In order to regulate speed we ignore an amount of possible
		// cutpoints after every cut we make.		
		int firstPos, closePos, hopp;
		if(speed >= 200)
			hopp = 0;
		if(speed >= 167 && speed < 200)
			hopp = 1;
		if(speed >= 152 && speed < 167)
			hopp = 2;
		if(speed >= 141 && speed < 152)
			hopp = 3;
		if(speed >= 133 && speed < 141)
			hopp = 4;
		if(speed >= 116 && speed < 133)
			hopp = 5;
		if(speed >= 100 && speed < 116)
			hopp = 6;

		//ke20030625 [
		intvec *zeroPos2;
		zeroPos2= adderaKlippTillKlippLista(sizeIn, zeroPos);
		if (zeroPos != zeroPos2) {
			delete zeroPos;
			zeroPos = zeroPos2;
			zeroPos2 = NULL;
		}
		// ]

		// Making a new list that has some spacing between cuts.
		for(int aktuellPosition = 0; aktuellPosition+1+hopp < zeroPos->getSize(); aktuellPosition +=(2+hopp))
		{

			firstPos = aktuellPosition;			
			closePos = aktuellPosition + 1;
			
			// Add the new positions in the new list
			// left cutpoint
			zeroPosIndex = firstPos;
			klippLista->add(zeroPos->getAtIndex(zeroPosIndex));
			//klippLista.push_back(*zeroPosIter);

			// right cutpoint
			zeroPosIndex = closePos;
			klippLista->add(zeroPos->getAtIndex(zeroPosIndex));			
		}

		// Adjusts the cuts to reduce audible artifacts

		//ke20030625 [
		intvec *klippLista2;
		klippLista2 = finjusteraKlippLista(psIn, sizeIn, klippLista);
		if (klippLista2 != klippLista) {
			delete klippLista;
			klippLista = klippLista2;
			klippLista2 = NULL;
		}
		// ]

		klippLista->checkIncrease();
		// Adjusts the signal after the cut to reduce audible artifacts
		cycleCrossFade(psIn, sizeIn, klippLista);

		klippListaIndex = 0;

		int klippStorlek, leftKlippPos, rightKlippPos, bortklipptSize;
		klippStorlek = klippLista->getSize();
		bortklipptSize = 0;
		
		if(klippStorlek > 0)
			leftKlippPos = klippLista->getAtIndex(klippListaIndex);

		for (smpl= 0 ; smpl < sizeIn; smpl++)
		{
			if(klippStorlek > 0 && klippListaIndex+2 <= klippStorlek)
			{
				if (smpl >= leftKlippPos)
				{
					klippListaIndex++;

					rightKlippPos = klippLista->getAtIndex(klippListaIndex);

					// This is the amount of samples that are cut away
					bortklipptSize =(rightKlippPos - smpl); 
					sizeOut = sizeOut - bortklipptSize;
					psIn += bortklipptSize;
					smpl += bortklipptSize;
					klippListaIndex++;
					leftKlippPos = klippLista->getAtIndex(klippListaIndex);
				}
			}
			
			// Straightforward copying of the samples that remain
			// when the rest is cut away.
			psOut[0] = psIn[0];
			psOut++;
			psIn++;
		}

		delete klippLista;
		delete zeroPos;
	}
}

//////////////////////////////////////////////////////////////////////////////
// smsFFT
//
// This function is used to calculate a frequency spectrum for analysis.
//
#define M_PI 3.14159265358979323846 
void CVupp::smsFft(double *fftBuffer, long fftFrameSize, long sign)
{
	/* 
	FFT routine, (C)1996 S.M.Sprenger. Sign = -1 is FFT, 1 is iFFT (inverse) 
	Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the 
	time domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes 
	and returns the cosine and sine parts in an interleaved manner, 
	ie. fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
	must be a power of 2. It expects a complex input signal (see footnote 2), 
	ie. when working with 'common' audio signals our input signal has to be 
	passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform 
	of the frequencies of interest is in fftBuffer[0...fftFrameSize].
	*/ 
 
	double wr, wi, arg, *p1, *p2, temp; 
	double tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i; 
	long i, bitm, j, le, le2, k; 
  
	for (i = 2; i < 2*fftFrameSize-2; i += 2) 
	{ 
		for (bitm = 2, j = 0; bitm < 2*fftFrameSize; bitm <<= 1) 
		{ 
			if (i & bitm) j++; 
			j <<= 1;
		} 
		if (i < j) 
		{ 
			p1 = fftBuffer+i; p2 = fftBuffer+j; 
			temp = *p1; *(p1++) = *p2; 
			*(p2++) = temp; temp = *p1; 
			*p1 = *p2; *p2 = temp;
		}
	} 
	for (k = 0, le = 2; k < log((double)fftFrameSize)/log(2.); k++) 
	{ 
		le <<= 1; 
		le2 = le>>1; 
		ur = 1.0; 
		ui = 0.0; 
		arg = M_PI / (le2>>1); 
		wr = cos(arg); 
		wi = sign*sin(arg); 
		for (j = 0; j < le2; j += 2) 
		{ 
			p1r = fftBuffer+j; p1i = p1r+1; 
			p2r = p1r+le2; p2i = p2r+1; 
			for (i = j; i < 2*fftFrameSize; i += le) 
			{ 
				tr = *p2r * ur - *p2i * ui; 
				ti = *p2r * ui + *p2i * ur; 
				*p2r = *p1r - tr; *p2i = *p1i - ti; 
				*p1r += tr; *p1i += ti; 
				p1r += le; p1i += le; 
				p2r += le; p2i += le;
			} 
			tr = ur*wr - ui*wi; 
			ui = ur*wi + ui*wr; 
			ur = tr;
		}
	}
}


