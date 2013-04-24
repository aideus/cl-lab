//cl_termstorage.h
//store two or more identical part of expression (which can for example be created by S combinator)

#ifndef __SEGER__CL_TERMSTORAGE_H__
#define __SEGER__CL_TERMSTORAGE_H__

#include "cl_base.h"
#include <set>

class cl_termstorage
{
 public:
   cl_termstorage(cl_term& cl);
     
   ~cl_termstorage();
   
   //can be called only from constructor of cl_term
   void add_new_origin(cl_term* new_cl);
        
 public:
   cl_term* t;
   int norigins; //number of origins
   bool is_reduced;
};

const char CL_TS_TERM = '#';

#endif
