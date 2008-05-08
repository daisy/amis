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

  Copyright (C) 2000, 2001 Matt R. Flax <flatmax@ieee.org>
  
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
#ifndef TIMECODE_H_
#define TIMECODE_H_

#include <stdarg.h>
#include "masterCounter.H"
#include "masterCounterArray.H"

/** Time Code, inherits the current code
 */
template <class MASTERCOUNTERTYPE, class ARRAYTYPE>
class TimeCode : public MASTERCOUNTERTYPE {
  int beginning, finish;
protected:
  MASTERCOUNTERTYPE start, end;
public:
  // The window count
  MasterCounterArray<MASTERCOUNTERTYPE, ARRAYTYPE> *window;
  //void zeroArray(void) {window->zeroArray();}
  //ARRAYTYPE* getDataPtr(void){return window->getDataPtr();}
  //  ARRAYTYPE& operator[](int i){return window[i];}

  TimeCode(int startVal, ...) : MASTERCOUNTERTYPE (){
//#if DEBUG > 1
//    std::cout<<"TimeCode::TimeCode(startVal, ...)"<<std::endl;
//#endif
    window=NULL;
 	va_list ap, ap1, ap2, ap3;
    va_start(ap, startVal);
    va_start(ap1, startVal);
    va_start(ap2, startVal);
    va_start(ap3, startVal);
    init(startVal, ap, ap1, ap2, ap3);
    va_end(ap);
    va_end(ap1);
    va_end(ap2);
    va_end(ap3);
  }

  TimeCode(void) : MASTERCOUNTERTYPE (){
//#if DEBUG > 1
//    std::cout<<"TimeCode::TimeCode()"<<std::endl;
//#endif
    window=NULL;
  }

  void init(int startVal, ...) {
      va_list ap, ap1, ap2, ap3;
      va_start(ap, startVal);
      va_start(ap1, startVal);
      va_start(ap2, startVal);
      va_start(ap3, startVal);
      init(startVal, ap, ap1, ap2, ap3);
      va_end(ap);
      va_end(ap1);
      va_end(ap2);
      va_end(ap3);
  }

 void init(int c, va_list& ap, va_list& apStart, va_list& apEnd, va_list& apWindow){
//#if DEBUG > 1
//   std::cout<<"TimeCode::init"<<std::endl;
//#endif
    beginning=0;
    start.init(beginning, apStart);

    MASTERCOUNTERTYPE::init(c, ap);
    finish=this->getMaxCount();

    // Ensure end points to the last window
    end.init(finish-1, apEnd);

    if (window) delete window;
    if (!(window=new MasterCounterArray<MASTERCOUNTERTYPE, ARRAYTYPE>)) std::cerr <<"TimeCode::init : window malloc error"<<std::endl;
    window->init(this->fields[this->fieldCount()-1]->getMaxCount(), apWindow);
  }

  ~TimeCode(void){
//#if DEBUG > 1
//    std::cout<<"TimeCode::~TimeCode"<<std::endl;
//#endif
    if (window) delete window;
  }

  int getBeginning(void){return beginning;}
  void setBeginning(counter c){setBeginning(c.getCount());}
  void setBeginning(int c){
    if (c>=finish)
      c=finish-1;
    beginning=c;
    setStart(beginning);
  }

  void nothing(){}

  int getStart(){return start.getCount();};
  void setStart(counter c){setStart(c.getCount());}
  void setStart(int c){
//#if DEBUG > 1
//    std::cout<<"TimeCode::setStart "<<c<<std::endl;
//#endif
    //Check that the beginning is not underrun
    // the end minimum is not before the start
    // the end minimum matches the start point
    if (c<beginning) c=beginning;
    if (c>end.getCount()) c=end.getCount();
    //    if (c<end->getMinCount()) end->setMinCount(c);
    end.setMinCount(c);
    nothing();
    start.MASTERCOUNTERTYPE::operator=(c);// set the start point
    //start=c;// set the start point
    //    std::cout<<start<<std::endl;
    setMinCount(start); // reset the minimum of the current time
  }

  int getEnd(){return end.getCount();};
  void setEnd(counter c){setEnd(c.getCount());}
  void setEnd(int c){
//#if DEBUG > 1
//    std::cout<<"TimeCode::setEnd "<<c<<std::endl;
//#endif

    //Check that the finish is not overrun
    // the start maximum is not after the end
    // the start maximum matches the end point
    if (c>finish) c=finish;
    if (c<start.getCount()) c=start.getCount();
    
    start.setMaxCount(c);
    // set the end point
    end.MASTERCOUNTERTYPE::operator=(c);
    setMaxCount(end); // reset the maximum of the current time
  }

  int getFinish(void){return finish;}
  void setFinish(counter c){setEnd(c.getCount());}
  void setFinish(int c){
    finish=c;
    end.setMaxCount(c);
    window->setMaxCount(c);
  }

  /** this function will apply a type II filter using the masterArray as the 
      output.
      Filter coefficients are specified by the arrays a and b, with their
      coefficient counts represented by aCnt and bCnt.
      The input is pointed to by 'input'. The current location must be pointed
      to and there must be at least 'aCnt' elements in the array preceeding
      the pointer.
      This function will only process one output at a time.
      Coefficient notes :
      a[0]=1.0;
      input :
              input  The current value of the input signal
	      aCnt   the number of denomiator coefficients
	      a      the denominator (pole polynomial) coefficients
	      bCnt   the number of numerator coefficients
	      b      the numerator (zero polynomial) coefficients
	      memory the filter memory of dimension max(aCnt,bCnt)
  */
  inline void filter(ARRAYTYPE input, const int aCnt, const double *a, const int bCnt, const double *b, double *memory){
    int j, loc=this->getCount();

    //cout<<a<<'\t'<<b<<'\t'<<memory<<'\n';
    //cout<<aCnt<<'\t'<<bCnt<<'\n';

    // Firstly calculate the output
    (*window)[loc]=input*b[0]+memory[0];
    //cout<<(*window)[loc]<<'\t'<<input<<'\t'<<b[0]<<'\t'<<memory[0]<<'\n';
    // Manipulate the filter memory
    for (j=1;j<aCnt & j<bCnt; j++){
      memory[j-1]=memory[j]+input*b[j]-(*window)[loc]*a[j];
      //cout<<memory[j-1]<<'\t'<<memory[j]<<'\t'<<input<<'\t'<<b[j]<<'\t'<<(*window)[loc]<<'\t'<<a[j]<<'\n';
    }

    //if (bCnt>aCnt)
    for (j;j<bCnt;j++){
      memory[j-1]=memory[j]+input*b[j];
      //cout<<"take 2 b: "<<memory[j-1]<<'\t'<<memory[j]<<'\t'<<input<<'\t'<<b[j]<<'\n';
    }
    //else
    for (j;j<aCnt;j++){
      memory[j-1]=memory[j]-(*window)[loc]*a[j];
      //cout<<"take 2 a "<<memory[j-1]<<'\t'<<memory[j]<<'\t'<<(*window)[loc]<<'\t'<<a[j]<<'\n';
    }
    //cout<<'\n';
  }

  TimeCode& operator =(TimeCode& c){counter::operator=(c);return *this;}
  TimeCode& operator =(counter c){counter::operator=(c);return *this;}
  TimeCode& operator =(int c){counter::operator=(c);return *this;}

  friend std::ostream& operator <<(std::ostream& o, TimeCode& tc) {
    o<<"TimeCode: Beginning: "<<tc.beginning<<" Finish: "<<tc.finish<<'\n';
    o<<"Start: "<<tc.start<<'\n';
    o<<"Current: "<<(MASTERCOUNTERTYPE&)tc<<'\n';
    o<<"End: "<<tc.end<<'\n';
    o<<"Window Count: "<<*tc.window<<'\n';
    return o;
  }
};
#endif //TIMECODE_H_
