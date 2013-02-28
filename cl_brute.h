//cl_brute.h

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#ifndef __CL_BRUTE_H__
#define __CL_BRUTE_H__
#include "cl_base.h"
#include <vector>
using namespace std;

//algorithm generate all possible brackets (in CL sense!)
void cl_brute_all_brackets(int i1, int i2, vector< vector< pair<int,int> > >& allb);

void cl_brute_apply_brackets(string s, const vector< pair<int,int> >& brackets,  string& rez);

//class to generate sequence of random cl expressions of given lengt
class cl_brute_generator
{
 public:
   void init(int l, string alphabet);   
   bool get_next(string& s);
   
   //generate random string
   //(state of get_next function will be changed)
   string create_random();
   size_t get_allb_size(){return allb.size();};
 private:
   bool get_next_cs();
   void generate_string(string& s);
 private:
     
   int l;
   string alphabet;
   vector< vector< pair<int,int> > > allb; //all possible brackets for given l
   
   vector<size_t> cs; //current "string"
   size_t cb;         //current  bracket set  
};

#endif
