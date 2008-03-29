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
#ifndef FIELD_H_
#define FIELD_H_

#include <math.h>

#include "counter.H"

//template <class FTYPE>
//#define FTYPE int
class field : public counter {
  unsigned int digitCount;

  /// Sets the digit count and display array
  void countDigits(void){
    // Work out the required digit count ...
    double mVal=maxCount;
    digitCount=0;
    while (mVal>pow(10.0,(int) ++digitCount)-1.0);
    // Set up the digit array ...
    digits=NULL;
    if (!(digits=new unsigned char[digitCount])){
      std::cerr <<"field: digit malloc error"<<std::endl;
      exit(-1);
    }
  }

  /// Sets the digits array to the digit value of the counter
  void syncDigits(void){
    //std::cout<<"syncdigits :: "<<std::endl;
    if (count==maxCount) count=0;
    int cnt = count;
    register double temp;
    for (int i=digitCount-1;i>=0;i--){
      temp=pow(10.0, i);
      digits[i]=(char)((double)cnt/temp);
      //std::cout<<(int)digits[i]<<std::endl;
      cnt-=digits[i]*(int)temp;
    }
  }

public:
  unsigned char *digits;

  field(int minimumVal, int startVal, int maximumVal) : counter(minimumVal, startVal, maximumVal){
//#if DEBUG > 1
//    std::cout<<"field::field(minimumVal, startVal, maximumVal)"<<std::endl;
//#endif
    digits=NULL;
    // Set up the digit array and count
    countDigits();
    // Sync the digits with the counter value
    syncDigits();
  }

  ~field(void){
//#if DEBUG > 1
//    std::cout<<"field::~field"<<std::endl;
//#endif
    //    std::cout<<"~field: in"<<std::endl;
    if (digits) delete [] digits;
    //    std::cout<<"~field: out"<<std::endl;
  }
  
  // The following alter the counter and must be overloaded
  field& operator =(const counter& f){counter::operator =(f); syncDigits(); return *this;}
  field& operator =(const int f){
    //  std::cout<<"equate"<<std::endl;
counter::operator =(f);
//std::cout<<"about to sync"<<std::endl;
 syncDigits(); return *this;}
  field& operator+=(const counter& f){counter::operator+=(f); syncDigits(); return *this;}
  field& operator+=(const int& f){counter::operator+=(f); syncDigits(); return *this;}
  field& operator-=(const counter& f){counter::operator-=(f); syncDigits(); return *this;}
  field& operator-=(const int& f){counter::operator-=(f); syncDigits(); return *this;}
  field& operator*=(const counter& f){counter::operator*=(f); syncDigits(); return *this;}
  field& operator*=(const int& f){counter::operator*=(f); syncDigits(); return *this;}
  field& operator/=(const counter& f){counter::operator/=(f); syncDigits(); return *this;}
  field& operator/=(const int& f){counter::operator/=(f); syncDigits(); return *this;}

  friend std::ostream& operator <<(std::ostream& o, field& f) {
    std::cout<<(counter)f<<":\t";
    for (int i=f.digitCount-1; i>=0; i--)
      o << (long)f.digits[i];
    return o;
  }

  int digitCnt(void){return digitCount;}
};
#endif //FIELD_H_
