#include <math.h>
#include "Vupp1.h"


void smsFft(double *fftBuffer, long fftFrameSize, long sign);

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
void smsFft(double *fftBuffer, long fftFrameSize, long sign)
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