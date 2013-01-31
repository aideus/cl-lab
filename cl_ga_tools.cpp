//cl_ga_tools.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#include "cl_ga_tools.h"

cl_ga_valuator::cl_ga_valuator(double pen_progsize_, double pen_wrongrez_, double pen_absentrez_)
{
   pen_progsize = pen_progsize_;
   pen_wrongrez   = pen_wrongrez_;
   pen_absentrez  = pen_absentrez_;
   if (pen_absentrez < pen_wrongrez)
     {
	cerr<<"Error | cl_ga_valuator::cl_ga_valuator | pen_absentrez shoundn't be less then pen_wrongrez"<<endl;
	exit(EXIT_FAILURE);
     }
}
//                                                                                                  
double cl_ga_valuator::evaluate(size_t prog_length, string rez, string req_rez)
{
   double p = 0;
   p += prog_length * pen_progsize;  //penalty for program length
   
   if (rez.size() < req_rez.size())
     p += (req_rez.size() - rez.size()) * pen_absentrez; //penalty for absent bits in result
   
   if (rez.size() > req_rez.size())
     {
	cerr<<"error | cl_ga_valuator::evaluate | rez.size()="<<rez.size();
	cerr<<" > req_rez.size()="<<req_rez.size()<<endl;
	exit(EXIT_FAILURE);
     }
   for (size_t i = 0 ; i < rez.size(); i++)
     if (rez[i] != req_rez[i])
       p += pen_wrongrez;    //penalty for wrong bit in the result
   
   return p;
}
//                                                                                                 
bool cl_ga_uniqchecker::try_to_add(string s)
{
   if (find(s) != end()) //already inside
     return false;
   insert(s);
   return true;
}
//                                                                                  
void cl_ga_uniqchecker::delete_string(string s)
{
   if (erase(s) != 1)
     {
	cerr<<"Error | cl_ga_uniqchecker::delete_string | can't find "<<s<<endl;
	exit(EXIT_FAILURE);
     }
}
//                                                                                  
