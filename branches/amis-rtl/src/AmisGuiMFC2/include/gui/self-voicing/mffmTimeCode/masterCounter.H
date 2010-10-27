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
#ifndef MASTERCOUNTER_H_
#define MASTERCOUNTER_H_

#include <stdarg.h>
#include "field.H"

//#define DEBUG_MC

template <class FIELDTYPE, int FIELDCOUNT>
class MasterCounter : public counter {
protected:
  FIELDTYPE **fields;
public:
  int fieldCount(void){return FIELDCOUNT;}

  MasterCounter(int startVal, ...) : counter(0, startVal, 1){
#ifdef DEBUG_MC
    std::cout<<"MasterCounter::MasterCounter(startVal, ...)"<<std::endl;
#endif
    int maxC=1;

    if (FIELDCOUNT){
      if (!(fields=new FIELDTYPE*[FIELDCOUNT])){
	std::cerr<<"MasterCounter: Out of memory error"<<std::endl;
	exit(-1);
      }
      for (int i=0;i<FIELDCOUNT;i++)
	fields[i]=NULL;

      va_list ap;
      va_start(ap, startVal);
      for (i=0;i<FIELDCOUNT;i++){
	int temp=va_arg(ap, int);
	if (i==0)
	  maxC*=temp;
	maxC*=temp;
	if (!(fields[i]=new FIELDTYPE(0, 0,temp))){
	  std::cerr<<"MasterCounter: Out of memory error 1"<<std::endl;
	  exit(-1);
	}
      }
      va_end(ap);
      setMaxCount(maxC);
      updateFields();
    }
#ifdef DEBUG_MC
    std::cout<<"MasterCounter::MasterCounter(startVal, ...) EXIT"<<std::endl;
#endif
  }

  MasterCounter(void) : counter(0, 0, 1){
#ifdef DEBUG_MC
    std::cout<<"MasterCounter::MasterCounter()"<<std::endl;
#endif
    if (FIELDCOUNT){
      fields=new FIELDTYPE*[FIELDCOUNT];
      if (!fields){
	std::cerr<<"MasterCounter: Out of memory error"<<std::endl;
	exit(-1);
      }
      for (int i=0;i<FIELDCOUNT;i++)
	fields[i]=NULL;
    }
  }

  void init(int c, ...){
#ifdef DEBUG_MC
    std::cout<<"MasterCount: init "<<c<<" ..."<<std::endl;
#endif
    va_list ap;
    va_start(ap, c);
    init(c, ap);
    va_end(ap);
  }

  void init(int c, va_list& ap){
#ifdef DEBUG_MC
    std::cout<<"MasterCount: init "<<c<<std::endl;
#endif
    count=c;
    int maxC=1;
    for (int i=0;i<FIELDCOUNT;i++){
      int temp=va_arg(ap, int);
      maxC*=temp;
      //std::cout<<i<<" after "<<temp<<'\t'<<maxC<<std::endl;
      fields[i]=new FIELDTYPE(0,0,temp);
      if (!fields[i]){
	std::cerr<<"MasterCounter: Out of memory error 1"<<std::endl;
	exit(-1);
      }
      //      std::cout<<FIELDCOUNT<< " looping"<<std::endl;
    }
    setMaxCount(maxC);
    updateFields();
  }

  ~MasterCounter(void){
#ifdef DEBUG_MC
    std::cout<<"MasterCounter::~MasterCounter"<<std::endl;
#endif
    if (fields) {
      for (int i=0;i<FIELDCOUNT;i++){
	if (fields[i]) delete fields[i];
	fields[i]=NULL;
      }
      delete [] fields;
      fields=NULL;
    }
    //    std::cout<<"~MasterCounter: out"<<std::endl;
  }

  /// Sets the minimum count
  int setMinCount(counter mc){return setMinCount(mc.getCount());}
  int setMinCount(int mc){
#ifdef DEBUG_MC
    std::cout<<"MasterCounter::setMinCount(mc="<<mc<<")"<<std::endl;
#endif
    int temp=counter::setMinCount(mc);
    updateFields();
    return temp;
  }

  /// Sets the minimum count
  int setMaxCount(counter mc){return setMaxCount(mc.getCount());}
  int setMaxCount(int mc){
#ifdef DEBUG_MC
    std::cout<<"MasterCounter::setMaxCount(mc="<<mc<<")"<<std::endl;
#endif
    int temp=counter::setMaxCount(mc);
    updateFields();
    return temp;
  }

  // syncs the field with the new counter value
  void updateFields(void){
    //std::cout<<"masterCounter: updateFields"<<std::endl;
    int temp=count, tempCarry;
    for (int i=0;i<FIELDCOUNT;i++){
      //std::cout<<i<<std::endl;
      int which =FIELDCOUNT-i-1;
      tempCarry=temp%fields[which]->getMaxCount();
      temp=temp/fields[which]->getMaxCount();
      *fields[which]=(int)tempCarry;
    }
    //std::cout<<"masterCounter: updateFields EXIT"<<std::endl;
  }

  // Logic operators
  /*  char operator==(counter& m) {return count == m.getCount();}
  char operator!=(counter& m) {return count != m.getCount();}
  char operator> (counter& m) {return count > m.getCount();}
  char operator>=(counter& m) {return count >= m.getCount();}
  char operator< (counter& m) {return count < m.getCount();}
  char operator<=(counter& m) {return count <= m.getCount();}
  */

  // Evaluational operators
  MasterCounter& operator =(counter c){counter::operator=(c); updateFields();return *this;}
  MasterCounter& operator =(int c){counter::operator=(c);updateFields();return *this;}
  MasterCounter& operator+=(counter c){counter::operator+=(c); updateFields();return *this;}
  MasterCounter& operator+=(int c){counter::operator+=(c);updateFields();return *this;}
  MasterCounter& operator-=(counter c){counter::operator-=(c); updateFields();return *this;}
  MasterCounter& operator-=(int c){counter::operator-=(c); updateFields();return *this;}
  MasterCounter& operator*=(counter c){counter::operator*=(c); updateFields();return *this;}
  MasterCounter& operator*=(int c){counter::operator*=(c); updateFields();return *this;}
  MasterCounter& operator/=(counter c){counter::operator/=(c); updateFields();return *this;}
  MasterCounter& operator/=(int c){counter::operator/=(c); updateFields();return *this;}

  friend std::ostream& operator <<(std::ostream& o, MasterCounter<FIELDTYPE, FIELDCOUNT>& c) {
    o << c.count<<"\tMinCount="<<c.minCount<<"\tcarry="<<c.carry<<"\tMaxCount="<<c.maxCount<<'\t';
    return o;
  }
};
#endif //MASTERCOUNTER_H_

