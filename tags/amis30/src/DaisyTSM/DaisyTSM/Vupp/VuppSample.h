// 
//#include "streams.h"
#include <dshow.h>

class CVuppSample  
{
public:
	// Functions
	CVuppSample();
	virtual ~CVuppSample();

	void saveMediaSample(IMediaSample *pMediaSampleIn);
	void loadMediaSample(IMediaSample *pMediaSampleOut);
	LONGLONG getStartTime();
	LONGLONG getEndTime();
	void setStartTime(LONGLONG sTime);
	void setEndTime(LONGLONG sTime);
	void initializeBuffer(int nbrOfSamples);
	short *buffer;
	long bufferSize;
	AM_MEDIA_TYPE *pMediaType;

//private:
	// Variables
	LONGLONG pStartTime, pEndTime;
	LONGLONG pStartMediaTime, pEndMediaTime;
	bool syncPoint;

};

