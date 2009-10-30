// TestDTBookTransform.cpp : Defines the entry point for the console application.
//

#include "util/Log.h"
#include "dtb/TransformDTBook.h"
#include <fstream>
#include <iostream>

using namespace std;

 int main()
{
	amis::util::Log::Instance()->startLog("c:\\devel\\amisdtbooktestlog.txt");
	
	string dtbstring, tmp;
	//ifstream input("c:\\daisybooks\\greatpainters_zed\\dtbook.xml");
	ifstream input("C:\\devel\\amis\\sandbox\\TestDTBookTransform\\dtbook_small.xml");
	while(!input.eof()) 
	{
		getline(input, tmp);
		dtbstring += tmp;
	}
	amis::dtb::TransformDTBook* xform = new amis::dtb::TransformDTBook();
	xform->transform(dtbstring);

	amis::util::Log::Instance()->endLog();

	//make the window stay open until we input something
	string x;
	cin>>x;
}

