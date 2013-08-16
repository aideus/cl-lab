//cl_ssprimega_tools.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#include "cl_ssprimega_tools.h"
double cl_ssprimega_valuate_rez(string rez, string req_rez, double pen_wrongrez, double pen_absentrez)
{
   double p = 0;
   
   //penalty for absent or extra bits in result
   p += abs((int)req_rez.size() - (int)rez.size()) * pen_absentrez;
   
   for (size_t i = 0 ; i < rez.size(); i++)
     if (rez[i] != req_rez[i])
       p += pen_wrongrez;    //penalty for wrong bit in the result
   
   return p;
}
//                                                            
void cl_ssprimega_member::calc_penalty(string valuator_type, 
				       const vector<string>& ans,
				       double pen_progsize, 
				       double pen_wrongrez, 
				       double pen_absentrez)
{
   part_L1 = 0;
   part_L2 = 0;
   part_R  = 0;
   if (valuator_type == "s")
     {
	part_L1 +=  ( S_str.size() + Sp_str.size()) * pen_progsize;  //penalty for "S" length
	//there is no penalty for "Sp" length
	
	for (size_t i = 0 ; i < rez_Sp.size() ; i++)
	  part_L2 += rez_Sp[i].size() * pen_progsize / double(rez_Sp.size());
	
	if (rez_S.size() != ans.size())
	  {
	     cerr<<"error | cl_ssprimega_valuate | rez.size() != req_rez.size()"<<endl;
	     exit(EXIT_FAILURE);
	  }
	for (size_t i = 0 ; i < rez_S.size() ; i++)
	  part_R += cl_ssprimega_valuate_rez(rez_S[i], ans[i], pen_wrongrez, pen_absentrez);
     }
   else if (valuator_type == "a")
     {
	part_L1 += ( S_str.size() + Sp_str.size()) * pen_progsize;  //penalty for "S" length
	//there is no penalty for "Sp" length
	
	for (size_t i = 0 ; i < rez_Sp.size() ; i++)
	  part_L2 += rez_Sp[i].size() * pen_progsize;
	
	if (rez_S.size() != ans.size())
	  {
	     cerr<<"error | cl_ssprimega_valuate | rez.size() != req_rez.size()"<<endl;
	     exit(EXIT_FAILURE);
	  }
	for (size_t i = 0 ; i < rez_S.size() ; i++)
	  part_R += cl_ssprimega_valuate_rez(rez_S[i], ans[i], pen_wrongrez, pen_absentrez);
     }
   else 
     {
	cerr<<"error | cl_rmdlga_member::calc_penalty | unknown valuator type="<<valuator_type<<endl;
	exit(EXIT_FAILURE);
     }
   penalty = part_L1 + part_L2 + part_R;
}
//                                            
cl_ssprimega_member::~cl_ssprimega_member()
{
   if (Sp != NULL)
     delete Sp;
   if (S != NULL)
     delete S;
}
//                                                                                                 
//                                                                                                 
bool cl_ssprimega_uniqchecker::try_to_add(string t1, string t2)
{
   string s = create(t1, t2);
   if (find(s) != end()) //already inside
     return false;
   insert(s);
   return true;
}
//                                                                                  
void cl_ssprimega_uniqchecker::delete_term(string t1, string t2)
{
   string s = create(t1, t2);
   if (erase(s) != 1)
     {
	cerr<<"Error | cl_ga_uniqchecker::delete_string | can't find "<<s<<endl;
	exit(EXIT_FAILURE);
     }
}
//                                                                                     
string cl_ssprimega_uniqchecker::create(string t1, string t2)
{
   return t1 + "#" + "\0" + t2;
}
