#ifndef LABELLIST_H
#define LABELLIST_H

#include "Label.h"


namespace amis
{
namespace gui
{
namespace spoken
{
class LabelList
{
public:
	LabelList();
	~LabelList();

	int getNumberOfLabels();
	Label* getLabel(int);

	bool doesListRepeat();

	void testPrint(std::string, int);
	void setRepeat(bool);
	void addLabel(Label*);

private:
	vector <Label*> mLabels;
	bool mbRepeat;
	
	friend class DataReader;
};
}}}

#endif