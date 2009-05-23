#include "intvec.h"
#include "VuppSample.h"

class CVupp  
{

public:
	
	void smsFft(double *fftBuffer, long fftFrameSize, long sign);

	// Public functions
	CVupp(int vuppBufferSize, WAVEFORMATEX *m_pwfxInInit);
	virtual ~CVupp();
	void fillBigBuffer(CVuppSample *vSample, long nbrOfvSamples);
	void copyFromBigBuffer(CVuppSample *vSample);
	void transform();
	// Public data
	double crossfadeSpeed;
	short * bufferIn;
	short * bufferOut;
	short * bufferTemp1;
	short * bufferTemp2;
	long bufferSizeIn;
	long bufferSizeOut;
	long bufferSizeTemp1;
	long bufferSizeTemp2;
	//ke20030611 added static keyword
	short silenceThresholdPercent;
	short silenceSpeed;
	short silenceRemainderLength;
	short cycleSpeed;

	long nbrOfVSamplesIn;
	long nbrOfVSamplesOut;
	long soundVolume;

	//ke20030611
	int windowLength;

private:
//	void dbgCpy(IMediaSample *pMediaSampleIn, IMediaSample *pMediaSampleOut);
//	void copyTime(IMediaSample *pMediaSampleIn, IMediaSample *pMediaSampleOut);
	// Crossfade function
	void crossFadeBigBuffer(const short *psIn, short *psOut, long sizeIn, long &sizeOut);
	// Pause functions
	void shortenPauses(const short *psIn, short *psOut, long sizeIn, long &sizeOut);
	void copyWavBlock(const short **psInOrig, short **psOutOrig, int antalSamples);
	long getSoundVolume(short *psIn, long ccIn);
	// Cycle functions
	void findAndCutCycles(short* psIn, short* psOut,long sizeIn, long &sizeOut);
	long harmonicPowerSpectrum(const short* psIn,long sizeIn);
	void normalizeToOne(double * data, long len);
	intvec *faltningFindCutPoints(short* psIn, long sizeIn, int size);
	intvec *adderaKlippTillKlippLista(long sizeIn, intvec* klVec);
	intvec *finjusteraKlippLista(const short* psIn, long sizeIn, intvec* klVec);
	short sign(short tal);
	void cycleCrossFade(short* psIn, long sizeIn, intvec* klVec);

	// Private data
	WAVEFORMATEX *m_pwfxIn;
};
