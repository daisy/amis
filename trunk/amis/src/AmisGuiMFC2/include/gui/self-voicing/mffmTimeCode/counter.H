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
#ifndef COUNTER_H_
#define COUNTER_H_

//#include <iomanip.h>
#include <iostream>

#ifndef MAXINT
#include "./win32fix.h"
#endif

//#define C_DEBUG

//#define CTYPE int
class counter {
protected:
  inline void cond(void){
#ifdef C_DEBUG
    std::cout<<"counter::cond enter"<<std::endl;
#endif
    if (minCount<(int)0.0)
      std::cerr<<"Warning: min Count < 0 and may cause problems"<<std::endl;
    if (maxCount<=minCount)
      std::cerr<<"Warning: max Count <= min Count and may cause problems"<<std::endl;
    if (maxCount==(int)0.0)
      std::cerr<<"Warning: max Count = 0 and may cause problems"<<std::endl;
    int temp=(int)0.0;
    if (--temp>0){
      std::cerr<<"counter: You are using an unsigned type ... error ... exiting"<<std::endl;
      exit(-1);
    }
#ifdef C_DEBUG
    std::cout<<"counter::cond exit"<<std::endl;
#endif
  }

  inline void clean(void){
#ifdef C_DEBUG
    std::cout<<"counter::clean count = "<<count<<std::endl;
#endif
    int tempCount=count-minCount, tempMaxCount=maxCount-minCount;
    //      std::cout<<tempCount<<'\t'<<tempMaxCount<<std::endl;
    //    if (count<0.0){
      //      carry=count/maxCount-(int)1.0;
      //count+=maxCount;
    if (tempCount<0.0){
      carry=tempCount/tempMaxCount-(int)1.0;
      tempCount+=tempMaxCount;
      //      std::cout<<carry<<'\t'<<count<<std::endl;
    }
    if (tempCount>=tempMaxCount){
      carry=tempCount/tempMaxCount;
      tempCount%=tempMaxCount;
    }
    count=tempCount+minCount;
    //std::cout<<"clean exit"<<std::endl;
#ifdef C_DEBUG
    std::cout<<"counter::clean exit "<<count<<std::endl;
#endif
  }

  int count, maxCount, minCount;
  int carry;
public:
  counter (){
#ifdef C_DEBUG
    std::cout<<"counter::counter()"<<std::endl;
#endif
    maxCount=MAXINT; minCount=0; count=(int)0;carry=0; cond();
    
  }
  counter (int minc, int maxc){
#ifdef C_DEBUG
    std::cout<<"counter::counter(minc, maxc)"<<std::endl;
#endif
    maxCount=maxc; minCount=minc; count=(int)0;carry=0; cond();
  }
  counter (int minc, int c, int maxc){
#ifdef C_DEBUG
    std::cout<<"counter::counter(minc="<<minc<<", c="<<c<<", maxc="<<maxc<<")"<<std::endl;
#endif
    //    if (c<0.0) std::cout <<"counter: init vals must be >=0 ... this may cause a problem"<<std::endl;
    count=c; maxCount=maxc; minCount=minc; carry=0; cond(); clean();
  }

  /// returns the current count
  int getCount(void){return count;}
  /// returns the maximum count
  int getMaxCount(void){return maxCount;}
  /// Sets the maximum count
  int setMaxCount(counter mc){return setMaxCount(mc.count);}
  int setMaxCount(int mc){
    if (count>mc) count=mc-1;
    return maxCount=mc;
  }
  /// returns the minimum count
  int getMinCount(void){return minCount;}
  /// Sets the minimum count
  int setMinCount(counter mc){return setMinCount(mc.count);}
  int setMinCount(int mc){
#ifdef C_DEBUG
    std::cout<<"counter::setMinCount(mc="<<mc<<")"<<std::endl;
#endif
    if (count<mc) count=mc;
    return minCount=mc;
  }
  /// returns the carry
  int getCarry(void){return carry;}
  /// resets the carry to zero
  void resetCarry(void){carry=0;}

  // Logic operators
  char operator!=(counter& c) {return count != c.count;}
  char operator!=(int& c) {return count != c;}
  char operator==(counter& c) {return count == c.count;}
  char operator==(int& c) {return count == c;}
  char operator> (counter& c) {return count >  c.count;}
  char operator> (int& c) {return count >  c;}
  char operator>=(counter& c) {return count >= c.count;}
  char operator>=(int& c) {return count >= c;}
  char operator< (counter& c) {return count <  c.count;}
  char operator< (int& c) {return count <  c;}
  char operator<=(counter& c) {return count <= c.count;}
  char operator<=(int& c) {return count <= c;}

  // Evaluational operators
  counter& operator =(counter c){
    //std::cout<<"count::yoyo"<<std::endl;
    count  = c.count; clean(); return *this;}
  counter& operator =(int c){
    //std::cout<<"count::yo "<<count<<std::endl;
    count  = c;clean(); return *this;}
  counter& operator+=(counter c){count += c.count; clean(); return *this;}
  counter& operator+=(int c){count += c; clean(); return *this;}
  counter& operator-=(counter c){count -= c.count; clean(); return *this;}
  counter& operator-=(int c){count -= c; clean(); return *this;}
  counter& operator*=(counter c){count *= c.count; clean(); return *this;}
  counter& operator*=(int c){count *= c; clean(); return *this;}
  counter& operator/=(counter c){count /= c.count; clean(); return *this;}
  counter& operator/=(int c){count /= c; clean(); return *this;}

  int operator+(counter& c){return count+c.count;}
  int operator+(int c){return count+c;}
  int operator-(counter& c){return count-c.count;}
  int operator-(int c){return count-c;}
  double operator*(counter& c){return (double)count*(double)c.count;}
  double operator*(int c){return (double)count*(double)c;}
  double operator/(counter& c){return (double)count/(double)c.count;}
  double operator/(int c){return (double)count/(double)c;}

  friend std::ostream& operator <<(std::ostream& o, counter c) {o << c.count<<"\tcarry="<<c.carry<<"\tMinCount="<<c.minCount<<"\tMaxCount="<<c.maxCount; return o;}
};
#endif //COUNTER_H_

