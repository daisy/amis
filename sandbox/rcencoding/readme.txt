attempt to convert a resource file from the format output by the daisy lion (web translation database) to a format digestible by visual studio. currently we do this manually by using ultraedit's utf8 to unicode conversion; however, it would be much better to automate the process.

the files in this directory are:

perl script: encode.pl

start with a chinese sample (traditional chinese):
lionoutput.rc:              UTF-8 Unicode C++ program text, with CR, LF line terminators

want to produce something that matches this file:
ultraedit_utf8tounicode.rc: Little-endian UTF-16 Unicode C++ program text, with CR, LF line terminators

however, the script only mangles the characters and produces the wrong line endings.

make a copy of "lionoutput.rc" named "in", then run:
> perl encode.pl
> file out
> out: Little-endian UTF-16 Unicode C++ program text, with CRLF, CR line
> terminators

this is a work in progress...

