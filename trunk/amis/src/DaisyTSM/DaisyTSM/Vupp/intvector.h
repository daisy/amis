

class CIntVector  
{


public:
	// Functions
	CIntVector(int vecValuei);
	virtual ~CIntVector();
	long size();
	void push_back(int vali); // Lägg på ett värde längst bak i listan
	void eraseAll(); // Ta bort hela listan
	int getValue(int index); // Spottar ut värdet på den platsen i listan
	

private:
	long vectorSize;
	int vecValue;
//	CIntVector newer;
//	CIntVector older;

};
