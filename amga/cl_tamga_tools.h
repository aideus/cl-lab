//cl_tamga_tools.h
/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/

//some tools for tamga algorithms (alex multi genetic algorithm)

#ifndef __CL_TAMGA_TOOLS_H__
#define __CL_TAMGA_TOOLS_H__
#include "cl_base.h"
#include <set>


class cl_tamga_member
{
 public:
   cl_term* term;                   //member in form of cl_term
   string   term_str;               //term in form of string
   vector< vector<string> > rez;    //computation rezult (till maximal length) for each member of ansamble
   double   penalty;                //quality assesment
   
   double   part_LP, part_Lgood, part_Lbad; //different parts of penalty
   int      generation;
 public:
   void calc_penalty(string valuator_type, double pen_progsize, double pen_rezgood, double pen_rezbad);
   ~cl_tamga_member() {if (term != NULL) delete term;};
};

void cl_tamga_rez_analysis(const vector< vector<string> >& rez,
			   size_t& ngood, size_t& lgood,
			   size_t& nbad,  size_t& lbad );
#endif
