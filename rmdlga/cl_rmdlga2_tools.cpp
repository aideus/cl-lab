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
#include "cl_rmdlga2_tools.h"
//                                                            
void cl_rmdlga2_member::calc_penalty(string valuator_type, 
				     const vector<string>& req_rez,
				     double pen_progsize, double pen_wrongrez, double pen_absentrez)
{
   part_L1 = 0;
   part_L2 = 0;
   part_R  = 0;
   if (valuator_type == "s")
     {
	part_L1 += term_str.size() * pen_progsize;  //penalty for program length
	for (size_t i = 0 ; i < yi_str.size() ; i++)
	  part_L2 += yi_str[i].size() * pen_progsize / double(yi_str.size());
	
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
	for (size_t i = 0 ; i < yi_str.size() ; i++)
	  part_L2 += yi_str[i].size() * pen_progsize;
	
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
string cl_rmdlga2_member::get_termyi(size_t i)
{
   return term_str + "(" + yi_str.at(i) + ")";
}
//                                                                                                 
cl_rmdlga2_member::~cl_rmdlga2_member() 
{
   if (term != NULL) delete term;
   for (size_t i = 0 ; i < yi.size() ; i++)
     delete yi[i];
}
