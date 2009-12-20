/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2004-2009  DAISY Consortium

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


				void setText(TextNodeSV*);
				void setAudio(amis::AudioNode*);
				TextAudioPair* clone();
				amis::MediaGroup* getAsAmisMediaGroup();


			private:
				TextNodeSV* mpText;
				amis::AudioNode* mpAudio;
				amis::MediaGroup* mpMediaGroupVersion;

			};
		}
	}
}
#endif