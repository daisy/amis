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

/*
  mffm Time Code
  Time Code for multimedia systems

  Copyright (C) 2000-2005 Matt R. Flax <flatmax@ieee.org>
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You have received a copy of the GNU Lesser General Public License
  along with this library.
*/
#ifndef SMILTIMECODE_H_
#define SMILTIMECODE_H_

#define SMILTIMECODE_VERSION 0.0

#include "timeCode.H"
#include <strstream>
//#include "win32fix.H"

//Begins the implementation of SMIL timing :
// http://www.w3.org/TR/2005/REC-SMIL2-20050107/smil-timing.html#Timing-LanguageDefinition

//The array type is char so that it can contain a smil mark up of a time reference
#define ARRAYTYPE char
//There are four fields in smil ... hours, minutes, seconds and milliseconds
#define FIELDCOUNT 4
//The master counter will not work with X under win32 so for portability, use no GUI - field
#define MASTERCOUNTERTYPE MasterCounter<field, FIELDCOUNT>
//The maximum field counts
#define MAX_HOURS 300
#define MAX_MINUTES 60
#define MAX_SECONDS 60
#define MAX_MILLISECONDS 1000

#define DEFAULTBUFLEN 512

/** This class loads a TimeCode structure with the correct
    beginning and end. A SMIL type of string is used as input for
    both times

*/
class SmilTimeCode : public TimeCode<MASTERCOUNTERTYPE, ARRAYTYPE> {
  
  //This function performs the raw string to milliseconds conversion
  //Input is a SMIL compliant string
  //Output is a millisecond count.
  int convertStrToTimeCode(char *strSMIL){
    //Step for conversion :
    // Work out the time metric : this correcponds to the maximum specified time field
    // Work out whether the last field is a fractional part and remember that info.
    // Hold fields in an array 'counts'
    // Load a master counter with appropriate millisecond count
    // Return the master counter millisecond count
    
    //Find the metric and thus the end of the stream
    //Form an array to hold the metric digit values
    int startMetric=0;
    
    char * tempI;
    if ((tempI=strstr(strSMIL,"ms"))!=NULL){
      //we are receiving milliseconds
      tempI[0]='\0'; //Mark a new end to the string
      startMetric=0; //Indicate the start metric
    } else if ((tempI=strstr(strSMIL,"s"))!=NULL){
      //we are receiving seconds
      tempI[0]='\0'; //Mark a new end to the string
      startMetric=1; //Indicate the start metric
    } else if ((tempI=strstr(strSMIL,"min"))!=NULL){
      //we are receiving minutes
      tempI[0]='\0'; //Mark a new end to the string
      startMetric=2; //Indicate the start metric
    } else if ((tempI=strstr(strSMIL,"h"))!=NULL){
      //we are receiving hours
      tempI[0]='\0'; //Mark a new end to the string
      startMetric=3; //Indicate the start metric
    } else {
      //No specification of metric.
      // Check number of ':' in the string
      int cnt=0;
      for (int i=0;i<strlen(strSMIL);i++){
	if (strSMIL[i]==':')
	  cnt++;
      }
      if (cnt==3)//We are using hours minutes seconds milliseconds
	startMetric=3;
      else if (cnt==2)//We are using hours minutes seconds
	startMetric=3;
      else if (cnt==1)//We are using minutes seconds
	startMetric=2;
      else //We are using seconds
	startMetric=1;
    }
    
    cout<<"startMetric="<<startMetric<<endl;
    
    //Make sure '.' is translated to ':'
    float convertLastField=0; //Use this to force fractional conversion of the smallest field
    if ((tempI=strstr(strSMIL,"."))!=NULL){
      convertLastField=atof(tempI);
      tempI[0]=':';
      cout<<convertLastField<<endl;
    }
    //if (convertLastField)
    //	cout<<"Last field needs conversion"<<endl;
    
    //Form an array to hold values counts[4]={milliseconds, sec, min, hours}
    int counts[4]={-1,-1,-1,-1};
    
    //Fill up to the start metric with zeros
    for (int i=FIELDCOUNT-1;i>startMetric;i--)
      counts[i]=0;
    
    /*		for (i=0;i<FIELDCOUNT;i++)
		cout<<counts[i]<<'\t';
		cout<<endl;
    */
    
    int howMany, howManyNot, origStartMetric; //Indicates how many tokens (and non tokens)
  parseTok:{
      howMany=howManyNot=0;
      origStartMetric=startMetric;
      
      //Work out how many tokens to parse
      istrstream buf(strSMIL);
      char tempS[DEFAULTBUFLEN];
      while (buf.getline(tempS,DEFAULTBUFLEN,':')){//Fill the array from the end
	if (strlen(tempS)){ //Guard against no first token e.g. .25 (meaning 0.25 seconds)
	  howMany++;
	  if (startMetric>=0)
	    counts[startMetric]=atoi(tempS);
	  //cout<<tempS<<'\t'<<counts[startMetric]<<endl;;
	} else {
	  howManyNot++;
	  if (startMetric>=0)
	    counts[startMetric]=0;
	}
	startMetric--;
      }
      cout<<howMany<<" Fields"<<endl;
    }
    
    if (origStartMetric+1<howMany){
      cout<<"metric (h,min,s or ms) doesn't match the number of fields, autocorrecting"<<endl;
      startMetric=howMany-1;
      goto parseTok;
    }
    
    for (int i=0;i<FIELDCOUNT;i++)
      cout<<counts[i]<<'\t';
    cout<<endl;
    
    //Now load up the masterCounter
    MASTERCOUNTERTYPE tempTime (128,MAX_HOURS,MAX_MINUTES,MAX_SECONDS,MAX_MILLISECONDS);
    tempTime=0;
    int factor=1,fractionalFactor=1;//Factor to multiply by
    //tempTime.dump();
    for (int i=0;i<FIELDCOUNT;i++){
      switch (i) {
      case 0:	factor=1; fractionalFactor*=MAX_MILLISECONDS;
	break;
      case 1:	factor*=MAX_MILLISECONDS; fractionalFactor*=MAX_SECONDS;
	break;
      case 2:	factor*=MAX_SECONDS; fractionalFactor*=MAX_MINUTES;
	break;
      case 3:	factor*=MAX_MINUTES; break;
      default: cout<<"SmilTimeCode::convertStrToTimeCode field count exceeded : Error 2"<<endl; break;
      }
      if (counts[i]>=0){ //Check that a token exists
	if (convertLastField>0){ //We must use a fractional concept
	  //cout<<"fractional i="<<i<<" counts[i]="<<counts[i]<<" fractionalFactor = "<<fractionalFactor<<endl;
	  //char tempS[DEFAULTBUFLEN];
	  //sprintf(tempS,"0.%d\0",counts[i]);
	  //tempTime+=atof(tempS)*fractionalFactor;
	  //cout<<"fractional i="<<i<<" convertLastField="<<convertLastField<<" fractionalFactor = "<<fractionalFactor<<endl;
	  tempTime+=convertLastField*(fractionalFactor+1); //Check this +1 : introduced to remove 1ms error e.g. 0.120 goes to 0.119 without this +1
	  convertLastField=0; //Ensure it doesn't happen again.
	} else //Regular update
	  tempTime+=counts[i]*factor;
      }
    }
    
    //		tempTime.dump();
    
    return tempTime.getCount();
  }
  
public:
  //The time code is restricted to MAX_INT hours, 60 minutes, 60 seconds and 1000 milliseconds
  SmilTimeCode(char *SMILBegin, char *SMILCurrent, char *SMILEnd) : TimeCode<MASTERCOUNTERTYPE, ARRAYTYPE>(DEFAULTBUFLEN,MAX_HOURS,MAX_MINUTES,MAX_SECONDS,MAX_MILLISECONDS){
    //This constructor will fill in the time code for beginning, current time and end time
    
	//Set the end of time
    //setFinish(convertStrToTimeCode(SMILEnd));
    this->end=convertStrToTimeCode(SMILEnd);
    //Set the beginning of time
    //setBeginning(convertStrToTimeCode(SMILBegin));
    this->start=convertStrToTimeCode(SMILBegin);
  }
};
#endif //SMILTIMECODE_H_
