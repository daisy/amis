
#include <stdio.h>

//windows header containing charset conversion macros
#include <afxpriv.h>

#include "streams.h"
class intvec
{
public:
	intvec()
	{
		size = 0;
		vec = new int[0];
	}
	~intvec()
	{
		delete[] vec;
	}
	// Adds a new value at the end of the vector
	void add(int newval)
	{
		int i;
		// Creating backup vector
		int * tempvec = new int[size];
		// Backing up data in original vector
		for(i=0; i < size; i++)
			tempvec[i] = vec[i];
		// Deleting original vector to create a bigger
		delete[] vec;
		// Increasing size of vector
		size++;
		// Creating new and bigger vector
		vec = new int[size];
		// Copying from backup to new bigger vector
		for(i=0; i < size-1; i++)
			vec[i] = tempvec[i];
		// Adding the new value lat in the vector
		vec[size-1] = newval;	

		//ke20030625
		delete[] tempvec;
	}
	// This function returns a value at a certain index
	int getAtIndex(int index)
	{
		// Check to ensure that we don't get index out of bounds

		if(index < size)
			return vec[index];
		else
		{
		}
		return 0;
	}
	// This function returns the size of the vector
	int getSize()
	{
		return size;
	}

	// This function sets a value at a certain index
	void setAtIndex(int index, int newVal)
	{
		// Check to ensure that we don't get index out of bounds
		if(index < size)
			vec[index] = newVal;
		else
		{
		}
	}

	// This function prints the content of the vector to screen
	void printvec()
	{
		USES_CONVERSION;

		char buf[300];
		sprintf(buf, "Storleken på vektorn är:%d\nInnehållet i vektorn är\n", size);
		for(int i=0; i < size; i++)
		{
			sprintf(buf,"%s%d, ", buf, vec[i]);
		}
		MessageBox(NULL, A2T(buf), _T("VUPP"), MB_OK);
	}

	void checkIncrease()
	{
		bool inc = true;
		for(int i = 0; i < size-1; i++)
			if(vec[i] >= vec[i+1])
				inc = false;
		if(!inc)
			printvec();
	}

private:
	int size;
	int *vec;
};

