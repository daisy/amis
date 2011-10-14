/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

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
#ifndef MASTERCOUNTERARRAY_H_
#define MASTERCOUNTERARRAY_H_

#include <stdarg.h>
//#include <sys/resource.h>
#include "field.H"

//#define MCA_DEBUG

template <class MASTERCOUNTERTYPE, class ARRAYTYPE>
class MasterCounterArray : public MASTERCOUNTERTYPE {
  
  /// The size in bytes of a single sample (multichannel size). i.e. 16 bit stereo => 4 bytes
  int frameSize;
  //  double countToFrames, framesToCount;

protected:
  ARRAYTYPE *array;
public:
  //  double getFramesToCount(){return framesToCount;}
  //double getCountToFrames(){return countToFrames;}

  int getFrameSize(){return frameSize;}
  int setFrameSize(int fsz){
#ifdef MCA_DEBUG
    std::cout<<"MasterCounterArray::setFrameSize"<<std::endl;
#endif
    //// Find the conversion from frames to count
    //    framesToCount=(double)frameCount/(double)rate;
    //countToFrames=(double)rate/(double)frameCount;

    if (frameSize!=fsz){
      frameSize=fsz;
      updateArray();
    }
    return frameSize;
  }

  /** Constructor, specifying a startValue for the array index and a
      list of time code fields.
  */
  MasterCounterArray(int startVal, ...) : MASTERCOUNTERTYPE(){
#ifdef MCA_DEBUG

    std::cout<<"MasterCounterArray::MasterCounterArray(startVal="<<startVal<<", ...)"<<std::endl;
#endif
    array=NULL;
    frameSize=1;
    va_list ap;
    va_start(ap, startVal);
    init(startVal, ap);
    va_end(ap);
  }
  
  /** Empty constructor, must use init to complete valid construction
   */
  MasterCounterArray(void) : MASTERCOUNTERTYPE(){
#ifdef MCA_DEBUG
    std::cout<<"MasterCounterArray::MasterCounterArray()"<<std::endl;
#endif
    array=NULL;
    frameSize=1;
  }

  /** Initialiser for the empty constructor
   */
  void init(int c, ...){
#ifdef MCA_DEBUG
    std::cout<<"MasterCounterArray::init(startVal="<<c<<", ...)"<<std::endl;
#endif    
    va_list ap;
    va_start(ap, c);
    init(c, ap);
    va_end(ap);
  }

  /** second stage init, called by 'other' init function
   */
  void init(int c, va_list& ap){
#ifdef MCA_DEBUG
    std::cout<<"MasterCounterArray::init(startVal="<<c<<", va_list)"<<std::endl;
#endif
    //cout<<"c "<<c<<endl;
    MASTERCOUNTERTYPE::init(c,ap);
    //std::cout<<*this<<std::endl;
    updateArray();
  }

  /** Deconstructor */
  ~MasterCounterArray(void){
#ifdef MCA_DEBUG
    std::cout<<"MasterCounterArray::~MasterCounterArray"<<std::endl;
#endif
    if (array) delete [] array;
    array=NULL;
    //    std::cout<<"MasterCounterArray::~MasterCounterArray exit"<<std::endl;
  }
  
  void updateArray(){
#ifdef MCA_DEBUG
    std::cout<<"MasterCounterArray::updateArray - new size "<<getCount()*frameSize<<std::endl;
#endif
    //std::cout<<"count "<<getCount()<<std::endl;
    //std::cout<<"frameSize "<<frameSize<<std::endl;
    if (array) delete [] array;
    array=NULL;
    //    std::cout<<getCount()<<std::endl;
    //Should limit the possible allocatable space here ...
    /*    struct rlimit limit;
    getrlimit(RLIMIT_MEMLOCK , &limit);
    std::cout<<"rlim_cur "<<limit.rlim_cur<<std::endl;
    std::cout<<"rlim_max "<<limit.rlim_max<<std::endl;
    */
    //cout<<"making array of size "<<this->getCount()*frameSize<<endl;
    if (!(array=new ARRAYTYPE[this->getCount()*frameSize])){
      std::cout<<"MasterCounterArray: Out of memory"<<std::endl;
      exit(-1);
    }
    //std::cout<<"MasterCounterArray::updateArray : array="<<array<<std::endl;
  }

  void zeroArray(void){
    //Check whether this needs frameSize multiplier.
    bzero(array, this->getCount()*sizeof(ARRAYTYPE));
  }

  // Evaluational operators
  MasterCounterArray& operator =(counter c){counter::operator=(c); updateArray();return *this;}
  MasterCounterArray& operator =(int c){counter::operator=(c);updateArray();return *this;}
  MasterCounterArray& operator+=(counter c){counter::operator+=(c); updateArray();return *this;}
  MasterCounterArray& operator+=(int c){counter::operator+=(c); updateArray();return *this;}
  MasterCounterArray& operator-=(counter c){counter::operator-=(c); updateArray();return *this;}
  MasterCounterArray& operator-=(int c){counter::operator-=(c); updateArray();return *this;}
  MasterCounterArray& operator*=(counter c){counter::operator*=(c); updateArray();return *this;}
  MasterCounterArray& operator*=(int c){counter::operator*=(c); updateArray();return *this;}
  MasterCounterArray& operator/=(counter c){counter::operator/=(c); updateArray();return *this;}
  MasterCounterArray& operator/=(int c){counter::operator/=(c); updateArray();return *this;}

  //  ARRAYTYPE& operator[](int i){return array[i];}
  ARRAYTYPE* getDataPtr(void){return array;}
  ARRAYTYPE& operator[](int i){return array[i];}
  //  ARRAYTYPE& operator[]=(int i, ARRAYTYPE val){return array[i]=val;}
};
#endif //MASTERCOUNTERARRAY_H_
