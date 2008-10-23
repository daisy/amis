This folder contains a set of scripts and instructions to generate the audio for the prompts in an amis accessible ui XML file.
These scripts probably just run well on Marisa's computer and are expected to be obsolete soon...

But if you're interested, here's how it works.

Prerequisites:
1. amis svn
2. lion svn
3. pipeline
4. perl
5. audacity

With the directories set up like this:
Top-level
	amis/
		amisapp/
			trunk/
				amis/
					pipeline/
	lion/
		lionapp/
			trunk/
				lion/
					
Run in this order to generate the narrated prompts for AMIS:

1. init

2. Edit amisPrompts.html to remove the xmlns attribute from the top
vi: d 36 right

3. Run the following Pipeline jobs (make sure you're using the latest pipeline release).

a. XHTML to DTBook
Input ./amisPrompts.html
Output as ./amisPrompts.xml

b. DTBook fixer
Input ./amisPrompts.xml
Output ./amisPromptsFixed.xml

c. Narrator
Input ./amisPromptsFixed.xml
Output into ./book/

4. finish

a. Make sure the audio files match up with the prompts.

5. Then use Audacity to trim the silences.  Make a new "chain" and add these actions to it:
a. Truncate Silence
b. Export MP3
c. End

Apply the chain to the files in ./audio/

5. Copy ./audio/cleaned/*.mp3 and amisAccessibleUi.xml into the DefaultLangpack directory of AMIS and commit them to SVN
