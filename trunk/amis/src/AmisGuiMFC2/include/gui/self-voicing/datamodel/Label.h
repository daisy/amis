#ifndef LABEL_H
#define LABEL_H

#include "gui/self-voicing/Prompt.h"


namespace amis
{
namespace gui
{
namespace spoken
{
enum LabelType {CAPTION, DESCRIPTION, ACCELERATOR, MNEMONIC};

class Label
{
public:
	Label();
	~Label();

	TextAudioPair* getContents();
	
	void setContents(TextAudioPair*);
	
	int getMfcId(); //std::string
	Prompt* getPrompt();
	std::string getCondition();

	LabelType getType();

	void testPrint(std::string outfile, int numTabs, std::string prefix = "");

	Label* clone();

	
protected:
	void setMfcId(int); //std::string
	void setPrompt(Prompt*);
	void setType(LabelType);
	void setCondition(std::string);

private:
	TextAudioPair* mpContents;
	//std::string mMfcId;
	int mMfcId;
	std::string mCondition;

	Prompt* mpPrompt;
	
	LabelType mLabelType;

	friend class DataReader;
};
}}}
#endif