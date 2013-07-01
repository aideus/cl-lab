//cl_rmdlga_tools.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#include "cl_rmdlga_tools.h"
double cl_rmdlga_valuate_rez(string rez, string req_rez, double pen_wrongrez, double pen_absentrez)
{
   double p = 0;
   if (rez.size() < req_rez.size())
     p += (req_rez.size() - rez.size()) * pen_absentrez; //penalty for absent bits in result
   
   if (rez.size() > req_rez.size())
     {
	cerr<<"error | cl_rmdlga_valuate_rez | rez.size()="<<rez.size();
	cerr<<" > req_rez.size()="<<req_rez.size()<<endl;
	exit(EXIT_FAILURE);
     }
   for (size_t i = 0 ; i < rez.size(); i++)
     if (rez[i] != req_rez[i])
       p += pen_wrongrez;    //penalty for wrong bit in the result
   
   return p;
}
//                                                            
void cl_rmdlga_member::calc_penalty(string valuator_type, 
				    const vector<string>& req_rez,
				    double pen_progsize, double pen_wrongrez, double pen_absentrez)
{
   part_L1 = 0;
   part_L2 = 0;
   part_R  = 0;
   if (valuator_type == "s")
     {
	part_L1 += term_str.size() * pen_progsize;  //penalty for program length
	for (size_t i = 0 ; i < yi.size() ; i++)
	  part_L2 += yi[i].size() * pen_progsize / double(yi.size());
	
	if (rez.size() != req_rez.size())
	  {
	     cerr<<"error | cl_rmdlga_valuate | rez.size() != req_rez.size()"<<endl;
	     exit(EXIT_FAILURE);
	  }
	for (size_t i = 0 ; i < rez.size() ; i++)
	  part_R += cl_rmdlga_valuate_rez(rez[i], req_rez[i], pen_wrongrez, pen_absentrez);
     }
   else if (valuator_type == "a")
     {
	//we don't devide by yi.size in part_L2
	part_L1 += term_str.size() * pen_progsize;  //penalty for program length
	for (size_t i = 0 ; i < yi.size() ; i++)
	  part_L2 += yi[i].size() * pen_progsize;
	
	if (rez.size() != req_rez.size())
	  {
	     cerr<<"error | cl_rmdlga_valuate | rez.size() != req_rez.size()"<<endl;
	     exit(EXIT_FAILURE);
	  }
	for (size_t i = 0 ; i < rez.size() ; i++)
	  part_R += cl_rmdlga_valuate_rez(rez[i], req_rez[i], pen_wrongrez, pen_absentrez);
     }
   else 
     {
	cerr<<"error | cl_rmdlga_member::calc_penalty | unknown valuator type="<<valuator_type<<endl;
	exit(EXIT_FAILURE);
     }
   penalty = part_L1 + part_L2 + part_R;
}
//                                                                                                 
//                                                                                                 
bool cl_rmdlga_uniqchecker::try_to_add(string term, const vector<string>& yi)
{
   string s = create(term, yi);
   if (find(s) != end()) //already inside
     return false;
   insert(s);
   return true;
}
//                                                                                  
void cl_rmdlga_uniqchecker::delete_term(string term, const vector<string>& yi)
{
   string s = create(term, yi);
   if (erase(s) != 1)
     {
	cerr<<"Error | cl_ga_uniqchecker::delete_string | can't find "<<s<<endl;
	exit(EXIT_FAILURE);
     }
}
//                                                                                     
string cl_rmdlga_uniqchecker::create(string term, const vector<string>& yi)
{
   string s = term;
   for (size_t i = 0 ; i < yi.size() ; i++)
     {
	s += "#";
	s += '\0';
	s += yi[i]; 
     }
   return s;
}
//                                                                                  
