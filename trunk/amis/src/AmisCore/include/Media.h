/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2007  DAISY for All Project

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef MEDIA_H
#define MEDIA_H

#include <string>
#include <vector>
using namespace std;

namespace amis
{
//!Media node types
enum MediaNodeType
{
	AUDIO,
	TEXT,
	IMAGE
};

//!text direction: "left to right" or "right to left"
enum TextDirection
{
	LTR,
	RTL
};


//!Media Node class
/*!
	MediaNode is the base class for specific media objects
*/
class MediaNode
{
public:
	MediaNode();
	~MediaNode();

	void cloneContents(MediaNode* p_new);
	//virtual MediaNode* clone() = 0;

	string getId();
	void setId(string);

	string getSrc();
	void setSrc(string);

	string getSrcExpanded();
	void setSrcExpanded(string);

	string getHref();
	void setHref(string);

	string getLangCode();
	void setLangCode(string);

	MediaNodeType getMediaNodeType();
	void setMediaNodeType(MediaNodeType);

	void print(bool verbose = true);

private:
	string mId;
	string mSrc;
	string mSrcExpanded;
	string mHref;
	string mLangCode;
	MediaNodeType mMediaNodeType;
};

//!A text node is a type of media node
/*!
	a text node represents raw text or a text reference (via MediaNode::href).  
*/
class TextNode : public MediaNode
{
public:
	TextNode();
	~TextNode();

	void cloneContents(TextNode* p_new);

	wstring getTextString();
	void setTextString(wstring);

	TextDirection getLangDir();
	void setLangDir(TextDirection);

	TextNode* clone();

private:
	wstring mText;
	TextDirection mLangDir;
};

//!AudioNode is a type of MediaNode
class AudioNode : public MediaNode
{
public:
	AudioNode();
	~AudioNode();
	AudioNode* clone();

	string getClipBegin();
	void setClipBegin(string);

	string getClipEnd();
	void setClipEnd(string);

	void print();

private:
	string mClipBegin;
	string mClipEnd;
};

//!ImageNode is a type of media node
class ImageNode : public MediaNode
{
public:
	ImageNode();
	~ImageNode();

	void cloneContents(ImageNode* p_new);

	ImageNode* clone();
};

//!Media group represents a group of media nodes to be rendered in parallel
/*!
  The reason for having multiple audio nodes is if a sequence needs to be formed
  for example to prepend a media group with an audio resource label
*/
class MediaGroup
{
public:
	MediaGroup();
	~MediaGroup();

	//!destroy the node data pointed to by this media group
	void destroyContents();

	void setText(TextNode*);
	void setImage(ImageNode*);
	void addAudioClip(AudioNode*);

	bool hasText();
	bool hasAudio();
	bool hasImage();

	TextNode* getText();
	ImageNode* getImage();
	AudioNode* getAudio(unsigned int);

	//!get the number of audio clips
	unsigned int getNumberOfAudioClips();

	string getId();
	void setId(string);

	void print();

private:
	//!the text node pointer
	TextNode* mpTextNode;
	//!the image node pointer
	ImageNode* mpImageNode;
	//!the audio node pointer collection
	vector <AudioNode*> mpAudioNodes;
	//the id
	string mId;
};
}
#endif
