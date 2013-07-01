//cl_ga_tools.h

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/

//some tools for genetic algorithms

#ifndef __CL_rmdlGA_TOOLS_H__
#define __CL_rmdlGA_TOOLS_H__
#include "cl_base.h"
#include <set>

double cl_rmdlga_valuate_rez(string rez, string req_rez, double pen_wrongrez, double pen_absentrez);


class cl_rmdlga_member
{
 public:
   cl_term* term;     //member in form of cl_term
   string   term_str; //term in form of string
   vector<string> yi; //term yi = xi, where xi is input
   vector<string> rez; //xi!
   double   penalty;  //quality assesment
   double   part_L1, part_L2, part_R;
   int      generation;
 public:
   //valuate, term_str, yi, rez must be setted
   void calc_penalty(string valuator_type, const vector<string>& req_rez, 
		     double pen_progsize, double pen_wrongrez, double pen_absentrez);
   
   ~cl_rmdlga_member() {if (term != NULL) delete term;};
};

class cl_rmdlga_uniqchecker: private set<string>
{
 public:
   //if string is new        --> return true and add string
   //if string is not new    --> return false
   bool try_to_add(string term, const vector<string>& yi);
   void delete_term(string term, const vector<string>& yi);
 private:
   string create(string term, const vector<string>& yi);
};


#endif
