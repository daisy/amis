// crossFade.cpp: implementation of the CcrossFade class.
//
//////////////////////////////////////////////////////////////////////

#include <DShow.h>
#include <math.h>
#include "crossFade.h"


typedef signed short  *LPSHORT; 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CcrossFade::CcrossFade()
{

}

CcrossFade::~CcrossFade()
{

}


//////////////////////////////////////////////////////////////////////////////
// floor
//
// This function truncates a floating point number.
// Ex1: A number of 6,768 will become 6,0. 
// Ex2: A number of 6,231 will become 6,0.

int CcrossFade::floor(double flyttal)
{
	int heltal = (int)flyttal;
	if(heltal > flyttal)
		heltal--;
	return heltal;
}


//////////////////////////////////////////////////////////////////////////////
// copyWavBlock
//
// This function takes two pointers to sample-data. It copies a number of them.
// The number of samples copied is controlled by antalSamples.
// In any event only one channel is copied, but the information is neccessary
// since the left and right channel are interleaved in stereo data.

void CcrossFade::copyWavBlock(short **psInOrig, short **psOutOrig, int antalSamples)
{
	short *psIn = *psInOrig;
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

























void CcrossFade::transformData(IMediaSample *pMediaSampleIn, IMediaSample *pMediaSampleOut, WAVEFORMATEX *m_pwfxIn)
{

	short *psIn, *psOut;
	unsigned int ccIn, ccOutOrig; // Number of samples in buffer	



	// Get pointer to sound buffers. Both input and output
	if(S_OK  != pMediaSampleIn->GetPointer((unsigned char**) &psIn))
		MessageBox(NULL, L"Error getPointer", L"VUPP", MB_OK);

	if(S_OK  != pMediaSampleOut->GetPointer((unsigned char**) &psOut))
		MessageBox(NULL, L"Error getPointer", L"VUPP", MB_OK);

	// Converting from bytes to number of samples.
	// We assume the audio stream to have mono 16 bit, which 
	// means 2 bytes per sample.

	ccIn  = pMediaSampleIn->GetActualDataLength()  / 2; // (ljud är 16 bitar mono)
	ccOutOrig = pMediaSampleOut->GetActualDataLength() / 2; // 



	// crossfade cannot change the size to more than 200% or less than 50%	
	ccOutOrig = (UINT)(ccIn / 1.9);
	ccOutOrig = min((UINT)(ccIn*1.8-1), ccOutOrig);
	ccOutOrig = max((UINT)(ccIn/1.8+1), ccOutOrig);

	
//	char buf[300];
//	sprintf(buf, "ccIn:%d, ccOutOrig:%d",ccIn, ccOutOrig);
//	MessageBox(NULL, buf, "VUPP", MB_OK);


		
		

    
	unsigned int ccOut = ccOutOrig;
	short *psInBackup = psIn;
	short *psOutBackup = psOut;
	
/*
	char buf[300];
	sprintf(buf, "Längd på in: %d\nLängd på ut: %d", ccIn, ccOut);
	MessageBox(NULL, buf, "", MB_OK);
*/
	int maxBlockSize;

	// Here I retrieve the windowlength from the registry.
	// I also compensate for other samplerates than 44100.
	//maxBlockSize = getCrossfadeWindowlengthFromRegistry() * m_pwfxIn->nSamplesPerSec / 44100;
	
	// OBS FULHACK, hårdkodning
	maxBlockSize = 2000;

    if (ccIn > ccOut) // If the speed is larger than 100
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
		
		numberOfBlocks = floor(ccIn / (float)maxBlockSize);
		
		
		// There must be at least one block. Usually the number of blocks
		// is bigger than zero.
		if(numberOfBlocks != 0 )
			blockSize = ccIn / numberOfBlocks;
		else
		{
			blockSize = ccIn;
			numberOfBlocks = 1;
		}

		
		int sideBlockSize,  centerBlockSize, shortBlockSize, blocknr, ss, smpl;

		shortBlockSize  = (int)(blockSize * ccOut / (float)ccIn);      // New smaller sound block
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

				blockSize = ccIn -(psIn - psInBackup);
				shortBlockSize = (ccOutOrig) - (psOut - psOutBackup);
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
			copyWavBlock(&psIn, &psOut, sideBlockSize);

			// Copying faded part of left half of original block
			for(smpl = centerBlockSize-1; smpl >= 0; smpl--)
			{
				ss = psIn[0];				
				// Multiplicate with window to fade out.
				ss = (int)(ss * window[smpl]);
				psOut[0] = (short)ss;

				psIn++;  // Jump to next sample
				psOut++; 
			}
			
			// Moving outPointer back in time to be able to add more faded sound to the same place
			psOut -= centerBlockSize;

			// Adding faded part of right half of original block
			for(smpl = 0; smpl < centerBlockSize; smpl++)
			{
				ss = psIn[0];
				// Multiplicate with window to fade in.
				ss = (int)(ss*window[smpl]);
				psOut[0] += (short)ss;

				psIn++;  // Jump to next sample
				psOut++; 
			}


			// Copying unfaded part of right half of original block
			copyWavBlock(&psIn, &psOut, sideBlockSize);			

		}
		ccOut--;
		ccOutOrig = ccOut;
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
		numberOfBlocks = floor(ccOut / (float)maxBlockSize);

		
		
		// There must be at least one block. Usually the number of blocks
		// is bigger than zero.
		if(numberOfBlocks != 0 )
			blockSize = floor(ccOut / (float)numberOfBlocks);
		else
		{
			blockSize = ccOut;
			numberOfBlocks = 1;
		}
		

		
		int sideBlockSize, centerBlockSize, shortBlockSize, smpl, blocknr, ss;
		double sinc;
				
		// Some filtergraphs doesn't allow buffers to grow. Instead you have to use
		// less of the input buffer and report that. This method does just that.		
		shortBlockSize  = (int)(blockSize * ccIn / (float)ccOut); // Smaller block than original size
		sideBlockSize   = blockSize - shortBlockSize;             // Unfaded area
		centerBlockSize = blockSize - sideBlockSize * 2;          // Crossfade area

		for (blocknr = 0; blocknr < numberOfBlocks; blocknr++)
		{
			// Due to round off errors, all blocks will not be equal in size.
			// The size of the last block will compensate.
			if(blocknr == numberOfBlocks-1)
			{
				blockSize = ccOut - (psOut - psOutBackup); // fönsterlängd minus avverkad längd
				shortBlockSize = ccIn - (psIn - psInBackup);

				sideBlockSize = (int)(blockSize - shortBlockSize);		
				centerBlockSize = (int)(blockSize - sideBlockSize * 2);
			}

			
			// Copying left side of the unfaded area of the original block
			copyWavBlock(&psIn, &psOut, sideBlockSize);

			
			// Copying the first faded area of the original block
			for(smpl = centerBlockSize; smpl > 0; smpl--)
			{
				ss = psIn[0];				
				// multiplicating with sinc window
				sinc = sin(3.14159265358*(smpl-1)/(float)centerBlockSize-3.14159265358/2.0)/2.0+0.5;
				ss = (int)(ss * sinc);

				psOut[0] = (short)ss;
				

				psIn++; // Jump to the next sample position
				psOut++; 
			}
			// Here we have copied the entire original block.
			// Moving back in-pointer to the beginning. 
			// Moving back out-pointer to where the second faded block starts
			psIn -= sideBlockSize;
			psIn -= centerBlockSize;
			psOut -= centerBlockSize;

			
			// Adding second faded block to sound.
			for(smpl = 0; smpl < centerBlockSize; smpl++)
			{
				ss = psIn[0];
				// multiplicate with sinc window
				sinc = sin(3.14159265358*(smpl)/(float)centerBlockSize-3.14159265358/2.0)/2.0+0.5;
				ss = (int)(ss * sinc);

				psOut[0] += (short)ss;

				psIn++; // Jump to next sample position
				psOut++; 
			}

			// Copying right side of the unfaded area of the original block
			copyWavBlock(&psIn, &psOut, sideBlockSize);
		}    
	}


	// Converting from number of samples to bytes.
	// We assume the audio stream to have mono 16 bit, which 
	// means 2 bytes per sample.

	pMediaSampleOut->SetActualDataLength(ccOut * 2); // Ny längd på output


	

}