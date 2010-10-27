class CcrossFade  
{
public:
	CcrossFade();
	virtual ~CcrossFade();
	void transformData(IMediaSample *pMediaSampleIn, IMediaSample *pMediaSampleOut, WAVEFORMATEX *m_pwfxIn);
private:
	int floor(double flyttal);
	void copyWavBlock(short **psInOrig, short **psOutOrig, int antalSamples);
};

