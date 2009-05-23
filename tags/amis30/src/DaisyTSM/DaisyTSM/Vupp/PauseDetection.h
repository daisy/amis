#include "streams.h"
#include <math.h>

class CPauseDetection  
{
public:
	CPauseDetection();
	virtual ~CPauseDetection();

void ShortenSilence16(		//static void WINAPI 
    signed short *psIn,				// pointer at indata
    unsigned long    ccIn,		// length of indata
    signed short *psOut,				// pointer at outdata
    unsigned long    *ccOut,	// length of ourdata
    unsigned int    cChannels,			// number of channels (mono = 1, stereo = 2)
	WAVEFORMATEX * m_pwfxIn,	// sound properties, such as samplerate
	long ljudStyrka)			// local sound volume
{
	// We start by searching for periods of silence that are long enough.
	// We then remove some of them, leaving a set length that is crossfaded
	// to reduce audible aliasing.
	short silenceTroskel; // 0 - 32768 (sound volume for 16 bit sound)
	short silenceThresholdPercent; // 0 - 100% of the sound volume
	long silenceRatio;
	
    silenceRatio = (long)(10000.0 / getSilenceSpeedFromRegistry());   
	
	silenceThresholdPercent = getSilenceThresholdFromRegistry();

	silenceTroskel = ljudStyrka * silenceThresholdPercent / 100;

	signed short *pssIn;
	int endLength;

	// Compensating so that the amount of time that is left after a cut
	// is the same, no matter what samplerate is used.
    endLength = getSilenceRemainderLengthFromRegistry() * m_pwfxIn->nSamplesPerSec / 44100;



	int smpl, ssmpl, silenceLength;
	double *window = new double[endLength]; // Creating half a sinc window
	for(int windex = 0; windex < endLength; windex++) 
		window[windex]  = sin(3.14159265358*windex/(float)endLength-3.14159265358/2.0)/2.0+0.5;

	short ss;
	int shortenLength=0;
	
	// Search for silence
	for(smpl = 0; smpl < ccIn; true)
	{
		ss = psIn[0];

		silenceLength = 0;
		ssmpl=smpl;
		pssIn = psIn;

		// If the samplevalue is less than the threshold value and we're
		// still within the allocated memory block
		while(abs(ss) < silenceTroskel    && ssmpl < (int)(ccIn)-1)
		{
			pssIn += cChannels;
			ss = pssIn[0];
			silenceLength++;
			ssmpl++;
		}

		// Now we know how long this silence is. The next thing to determine 
		// is how much to cut away. We have to consider both remainderlength
		// and silence_ratio
		long tempEndLength;
		tempEndLength = silenceLength * (silenceRatio / 100.0) * (silenceRatio / 100.0);
		endLength = max(endLength,tempEndLength);


		if(silenceLength > endLength * 2)
		{
			delete[] window;
			window = new double[endLength]; // Creating half a sinc window
			for(int windex = 0; windex < endLength; windex++) 
				window[windex]  = sin(3.14159265358*windex/(float)endLength-3.14159265358/2.0)/2.0+0.5;
						
			// Copy the left part of the silence and fade it out with the window
			for(ssmpl = 0; ssmpl < endLength; ssmpl++)
			{
				psOut[0] = (short)(psIn[0] * window[endLength -1 -ssmpl]);	// mirroring window
				psIn += cChannels;
				psOut += cChannels;
			}
			// Skip the middle part of the silence
			psIn += cChannels * (silenceLength - endLength * 2);
			shortenLength += (silenceLength - endLength);
			psOut = psOut - cChannels * endLength;

			// Fade in the right part and add it to the left part.
			for(ssmpl = 0; ssmpl < endLength; ssmpl++)
			{
				psOut[0] += (short)(psIn[0] * window[ssmpl]);	
				psIn += cChannels;
				psOut += cChannels;
			}
			smpl += silenceLength;
			
		
		}
		else
		{
			if (silenceLength == 0)
				silenceLength = 1; // Only copying one sample

			// straightforward copying of samples, that are not judged to be silent.
			for(ssmpl = 0; ssmpl < silenceLength; ssmpl++)
			{
				psOut[0] = psIn[0];		
				psIn += cChannels;
				psOut += cChannels;
				smpl++;
			}
		}
	}
	delete[] window;

	*ccOut = *ccOut - shortenLength;

	return;
}

};
