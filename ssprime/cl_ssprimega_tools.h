//cl_ssprimega_tools.h

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

#ifndef __CL_SSPRIMEGA_TOOLS_H__
#define __CL_SSPRIMEGA_TOOLS_H__
#include "cl_base.h"
#include <set>

double cl_ssprimega_valuate_rez(string rez, string req_rez, double pen_wrongrez, double pen_absentrez);


class cl_ssprimega_member
{
 public:
   cl_term* Sp;     //S prime in form of cl_term
   string   Sp_str; //S prime in form of string
   cl_term* S;       //S in form of cl_term
   string   S_str;   //S in form of string
   
   
   vector<string> rez_Sp; //rez_Sp =  Sp . ans[i] 
   vector<string> rez_S;  //rez_S  = S . rez_Sp[i]
   
   double   penalty;  //quality assesment
   double   part_L1, part_L2, part_R;
   int      generation;
 public:
   //Sp_str, S_str, rez_Sp, rez_S must be setted
   void calc_penalty(string valuator_type, const vector<string>& ansamble, 
		     double pen_progsize, double pen_wrongrez, double pen_absentrez);
   
   ~cl_ssprimega_member();
};

class cl_ssprimega_uniqchecker: private set<string>
{
 public:
   //if string is new        --> return true and add string
   //if string is not new    --> return false
   bool try_to_add(string Sp, string S);
   void delete_term(string Sp, string S);
 private:
   string create(string Sp, string S);
};


#endif
