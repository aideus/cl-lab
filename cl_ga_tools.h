//cl_ga_tools.h

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (pas.aicv@gmail.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/

//some tools for genetic algorithms

#ifndef __CL_GA_TOOLS_H__
#define __CL_GA_TOOLS_H__
#include "cl_base.h"
#include <set>

class cl_ga_valuator
{
 public:
   cl_ga_valuator(double pen_progsize, double pen_wrongrez, double pen_absentrez);
   double evaluate(size_t prog_size, string rez, string req_rez);
 
 private:
   double pen_progsize, pen_wrongrez, pen_absentrez;
};


class cl_ga_member
{
 public:
   cl_term* term;     //member in form of cl_term
   string   term_str; //term in form of string
   double   penalty;  //quality assesment
   string   rez;      //computation rezult (till required lenght)
   int      generation;
 public:
   ~cl_ga_member() {if (term != NULL) delete term;};
};

class cl_ga_uniqchecker: private set<string>
{
 public:
   //if string is new        --> return true and add string
   //if string is not new    --> return false
   bool try_to_add(string s);
   void delete_string(string s);
};


#endif
