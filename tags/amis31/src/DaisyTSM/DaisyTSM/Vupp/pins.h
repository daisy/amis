class CInputPin : public CTransformInputPin
{
public:
	CInputPin (TCHAR *pObjectName, CTransformFilter * pFilter,
         HRESULT          * phr,
         LPCWSTR            pName)
      : CTransformInputPin (pObjectName, pFilter, phr, pName)
      {
      };

private:


};

class COutputPin : public CTransformOutputPin
{
public:

private:


};
