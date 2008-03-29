
#ifndef UIMEDIATYPES_H
#define UIMEDIATYPES_H


#include "Media.h"



namespace amis
{
namespace gui
{
namespace spoken
{
class TextNodeSV : public amis::TextNode
{
public:
	TextNodeSV();
	~TextNodeSV();

	std::string getMnemonic();
	void setMnemonic(std::string);

	TextNodeSV* clone();

private:
	std::string mMnemonic;

};

enum ImageType {SMALL_ICON, LARGE_ICON};

class ImageNodeSV : public amis::ImageNode
{
public:
	ImageNodeSV();
	~ImageNodeSV();

	ImageType getImageType();

	void setImageType(ImageType);

	ImageNodeSV* clone();

private:
	ImageType mImageType;
		 
};

class TextAudioPair
{
public:
	TextAudioPair();
	~TextAudioPair();

	TextNodeSV* getText();
	amis::AudioNode* getAudio();
	
	void testPrint(std::string, int);

	void setText(TextNodeSV*);
	void setAudio(amis::AudioNode*);
	TextAudioPair* clone();
	amis::MediaGroup* getAsAmisMediaGroup();


private:
	TextNodeSV* mpText;
	amis::AudioNode* mpAudio;
	amis::MediaGroup* mpMediaGroupVersion;


};
}}}
#endif