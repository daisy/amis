
#include "gui/self-voicing/UiMediaTypes.h"
#include <fstream>

using namespace amis::gui::spoken;

//***********
//TextNode
TextNodeSV::TextNodeSV()
{
	mMnemonic = "";
}

TextNodeSV::~TextNodeSV()
{
	mMnemonic.clear();
	mMnemonic.erase();
}

std::string TextNodeSV::getMnemonic()
{
	return mMnemonic;
}
void TextNodeSV::setMnemonic(std::string mnemonic)
{
	mMnemonic.assign(mnemonic);
}

//just copy over the text string and the mnemonic
//not complete but will do for now
TextNodeSV* TextNodeSV::clone()
{
	TextNodeSV* p_new = new TextNodeSV();

	p_new->setMnemonic(this->getMnemonic());

	amis::TextNode::cloneContents(p_new);

	return p_new;
}

//***********
//ImageNode
ImageNodeSV::ImageNodeSV()
{
	mImageType = SMALL_ICON;
}
ImageNodeSV::~ImageNodeSV()
{
}
ImageType ImageNodeSV::getImageType()
{
	return mImageType;
}
void ImageNodeSV::setImageType(ImageType imgType)
{
	mImageType = imgType;
}
ImageNodeSV* ImageNodeSV::clone()
{
	ImageNodeSV* p_new = new ImageNodeSV();

	p_new->setImageType(this->getImageType());

	amis::ImageNode::cloneContents(p_new);

	return p_new;
}


//******************
//TextAudioPair

TextAudioPair::TextAudioPair()
{
	mpText = NULL;
	mpAudio = NULL;
	mpMediaGroupVersion = NULL;
}
TextAudioPair::~TextAudioPair()
{
	if (mpMediaGroupVersion != NULL)
		delete mpMediaGroupVersion;
	if (mpText != NULL)
		delete mpText;
	if (mpAudio != NULL)
		delete mpAudio;

}

TextNodeSV* TextAudioPair::getText()
{
	return mpText;
}

amis::AudioNode* TextAudioPair::getAudio()
{
	return mpAudio;
}

void TextAudioPair::setText(TextNodeSV* pNode)
{
	this->mpText = pNode;
}
void TextAudioPair::setAudio(amis::AudioNode* pNode)
{
	this->mpAudio = pNode;
}
TextAudioPair* TextAudioPair::clone()
{
	TextAudioPair* p_new = new TextAudioPair();
	
	amis::AudioNode* p_audio = (amis::AudioNode*)this->getAudio()->clone();
	TextNodeSV* p_text = this->getText()->clone();

	p_new->setAudio(p_audio);
	p_new->setText(p_text);

	return p_new;
}
amis::MediaGroup* TextAudioPair::getAsAmisMediaGroup()
{
	if (mpMediaGroupVersion != NULL)
		delete mpMediaGroupVersion;

	mpMediaGroupVersion = new amis::MediaGroup();

	mpMediaGroupVersion->setText(this->mpText);
	mpMediaGroupVersion->addAudioClip(this->mpAudio);

	return mpMediaGroupVersion;
}
void TextAudioPair::testPrint(std::string outfile, int numTabs)
{
	fstream out;
	out.open(outfile.c_str(), ios::app | ios::out);

	int i=0;

	for (i=0; i<numTabs; i++){out<<"\t";}
	out<<"text = ";

	out.close();

	//reopen in widechar mode to write the text string
	wfstream wout;
	wout.open(outfile.c_str(), ios::app | ios::out);

	if (mpText != NULL)
	{
		wout.write(this->mpText->getTextString().c_str(), 
			this->mpText->getTextString().size());
	}

	wout.close();
	//done with widechar mode

	out.open(outfile.c_str(), ios::app | ios::out);

	out<<endl;
	
	if (mpAudio != NULL)
	{
		for (i=0; i<numTabs; i++){out<<"\t";}
		std::string audio_src = mpAudio->getSrc();

		out<<"audio src = ";
		out<<audio_src<<endl;
	}

	out.close();

}
