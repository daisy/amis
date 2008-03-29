#ifndef PROMPT_H
#define PROMPT_H


#include "UiMediaTypes.h"



namespace amis
{
namespace gui
{
namespace spoken
{
enum PromptItemType {PROMPT_VARIABLE, PROMPT_ITEM};

class PromptItemBase 
{
public:
	PromptItemBase();
	~PromptItemBase();

	PromptItemType getPromptItemType();
	TextAudioPair* getContents();

	virtual void testPrint(std::string, int) = 0;
	void setContents(std::wstring, std::string);
	void setContents(std::wstring textString, amis::AudioNode* p_audio);

	void setPromptItemType(PromptItemType);
	void setContents(TextAudioPair*);
	void setContents(amis::MediaGroup*);

private:
	PromptItemType mType;
	TextAudioPair* mpContents;

};

//both promptItems and promptVars should resolve to a PromptItemBase
class PromptItem : public PromptItemBase
{
public:
	PromptItem();
	~PromptItem();

	std::string getRefId();
	std::string getId();
	void testPrint(std::string, int);
	PromptItem* clone();
	void setReferenceContents(PromptItem*);

protected:
	void setRefId(std::string);
	void setId(std::string);

private:
	std::string mRefId;
	std::string mId;

	friend class DataReader;
};

class PromptVar : public PromptItemBase
{
public:
	PromptVar();
	~PromptVar();

	//the variable parameter
	std::string getParam();

	//the variable name
	std::string getName();

	void testPrint(std::string, int);

	PromptVar* clone();

protected:
	void setParam(std::string);
	void setName(std::string);

private:
	std::string mParam;
	std::string mName;

	friend class DataReader;
};



class Prompt
{
public:
	Prompt();
	~Prompt();

	int getNumberOfItems();
	PromptItemBase* getItem(int);

	std::string getType();
	std::string getId();

	void testPrint(std::string, int);
	std::wstring concatAllText();
	Prompt* clone();


protected:
	void addItem(PromptItemBase*);
	void setType(std::string);
	void setId(std::string);

private:
	vector <PromptItemBase*> mItems;
	std::string mType;
	std::string mId;

	friend class DataReader;
};


}}}
#endif