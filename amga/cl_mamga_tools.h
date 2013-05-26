//cl_mamga_tools.h
/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/

//some tools for amga algorithms (alex multi genetic algorithm)

#ifndef __CL_MAMGA_TOOLS_H__
#define __CL_MAMGA_TOOLS_H__
#include "cl_base.h"
#include <set>

class cl_mamga_valuator
{
 public:
   double evaluate(const vector<string>& rez, const string& prog, double ac_K, double ac_Kxi);
 public:
   double part_L1, part_L2, used_ac_K, used_ac_Kxi;
};


class cl_mamga_member
{
 public:
   cl_term* term;      //member in form of cl_term
   string   term_str;  //term in form of string
   vector<string> rez; //computation rezult (till maximal length) for each member of ansamble
   double   penalty;   //quality assesment
   double part_L1, part_L2;  //part_L1, part_L2 of penaly
   double used_ac_K, used_ac_Kxi;  //used ac_K ac_Kxi
   int      generation;
   size_t  Q; //number of different string in rez
 public:
   void calc_Q(); //calculate number of different results
 public:
   ~cl_mamga_member() {if (term != NULL) delete term;};
};


//calculate ac_K based on program K
double cl_mamga_calc_ac_K(string K);

#endif
