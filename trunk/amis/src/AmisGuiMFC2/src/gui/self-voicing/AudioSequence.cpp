#include "gui/self-voicing/AudioSequence.h"
#include "gui/self-voicing/datamodel/DataTree.h"

using namespace amis::gui::spoken;

AudioSequence::AudioSequence(){}

AudioSequence::~AudioSequence(){
	if (m_AudioSequence.GetCount() > 0)
	{
		for (size_t i = 0; i < m_AudioSequence.GetCount(); i++) {
			AudioSequenceComponent* comp = m_AudioSequence.GetAt(m_AudioSequence.FindIndex(i));
			delete comp;
		}

		m_AudioSequence.RemoveAll();
	}
}

UINT AudioSequence::GetCount() {
	return m_AudioSequence.GetCount();
}
bool AudioSequence::IsEmpty() {
	return m_AudioSequence.GetCount() == 0;
}
AudioSequenceComponent* AudioSequence::RemoveTail() {
	return m_AudioSequence.RemoveTail();
};
void AudioSequence::AddTail(AudioSequenceComponent* comp) {
	m_AudioSequence.AddTail(comp);
}

void AudioSequence::append(CString strTTS)
{
	append_prepend(strTTS, false);
}

void AudioSequence::append_prepend(CString strTTS, bool prepend) {
	if (strTTS.IsEmpty()) {
		return;
	}

	string strPrompt = "";
	if (strTTS.Compare(L"1") == 0) {
		strPrompt = "numeric_one";
	} else if (strTTS.Compare(L"2") == 0) {
		strPrompt = "numeric_two";
	} else if (strTTS.Compare(L"3") == 0) {
		strPrompt = "numeric_three";
	} else if (strTTS.Compare(L"4") == 0) {
		strPrompt = "numeric_four";
	} else if (strTTS.Compare(L"5") == 0) {
		strPrompt = "numeric_five";
	} else if (strTTS.Compare(L"6") == 0) {
		strPrompt = "numeric_six";
	} else if (strTTS.Compare(L"7") == 0) {
		strPrompt = "numeric_seven";
	} else if (strTTS.Compare(L"8") == 0) {
		strPrompt = "numeric_eight";
	} else if (strTTS.Compare(L"9") == 0) {
		strPrompt = "numeric_nine";
	} else if (strTTS.Compare(L"0") == 0) {
		strPrompt = "numeric_zero";
	}
	if (strPrompt.length() != 0) {
		PromptItem* pi = DataTree::Instance()->findPromptItem(strPrompt);

		if (pi != NULL)
		{

			TextAudioPair* pair = pi->getContents();
			if (pair != NULL) {
				amis::AudioNode* audio = pair->getAudio();
				TextNodeSV * textN = pair->getText();

				if (audio != NULL && audio->getSrc().length() != 0) 
				{
					//seq->append(audio->clone(), (textN != NULL ? textN->getTextString().c_str() : L""));

					AudioSequenceComponent* comp = new AudioSequenceComponent;
					comp->m_isAudioClip = true;
					comp->m_AudioClip = audio->clone();
					comp->m_String = strTTS;
					m_AudioSequence.AddHead(comp);
					return;
				}
			}
		}
	}

	AudioSequenceComponent* comp = new AudioSequenceComponent;
	comp->m_isAudioClip = false;
	comp->m_AudioClip = 0;
	comp->m_String = strTTS;
	if (prepend) m_AudioSequence.AddTail(comp); else m_AudioSequence.AddHead(comp);

}

void AudioSequence::appendAll(AudioSequence * seq)
{
	for (size_t i = 0; i < seq->m_AudioSequence.GetCount(); i++) {
		AudioSequenceComponent* comp = seq->m_AudioSequence.GetAt(seq->m_AudioSequence.FindIndex(i))->clone();
		m_AudioSequence.AddHead(comp);
	}
}

void AudioSequence::prepend(CString strTTS) {
	
	append_prepend(strTTS, true);
}
void AudioSequence::append(amis::AudioNode* audioClip, CString strTTS) {
	if (audioClip->getSrc().length() == 0) {
		int here = 1;
	}
	AudioSequenceComponent* comp = new AudioSequenceComponent;
	comp->m_isAudioClip = TRUE;
	comp->m_AudioClip = audioClip;
	comp->m_String = strTTS;
	m_AudioSequence.AddHead(comp);
}
AudioSequence* AudioSequence::clone() {
	AudioSequence* seq = new AudioSequence;
	for (size_t i = 0; i < m_AudioSequence.GetCount(); i++) {
		AudioSequenceComponent* comp = m_AudioSequence.GetAt(m_AudioSequence.FindIndex(i))->clone();
		seq->AddTail(comp);
	}

	return seq;
}