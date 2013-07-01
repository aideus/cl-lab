//cl_rmdlga2_tools.h
//with yi --- cl expression
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

#ifndef __CL_RMDLGA2_TOOLS_H__
#define __CL_RMDLGA2_TOOLS_H__
#include "cl_base.h"
#include <set>

class cl_rmdlga2_member
{
 public:
   cl_term* term;            //member in form of cl_term
   string   term_str;        //term in form of string
   vector<cl_term*> yi;      //yi in form of cl_term
   vector<string>   yi_str;  //yi in form of string
   
   vector<string> rez; //xi = term yi!
   double   penalty;  //quality assesment
   double   part_L1, part_L2, part_R;
   int      generation;
 public:
   //valuate, term_str, yi_str, rez must be setted
   void calc_penalty(string valuator_type, const vector<string>& req_rez, 
		     double pen_progsize, double pen_wrongrez, double pen_absentrez);
   
   string get_termyi(size_t i);//make tempyi string   
   
   ~cl_rmdlga2_member();
};



#endif
