//cl_resultator.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#include "cl_resultator.h"
#include <iostream>
#include <cstdlib>

int cl_resultator_exact::put_next(char c)
{
   if (ignore.find(c) == string::npos) //we shouldn't ignore this character
     rez.push_back(c);
   if (rez.size() >= req_rez.size())
     {
	if (rez.substr(0, req_rez.size()) == req_rez) 
	  return 1; //good
	return -1;  //bad anyway
     }
   if (rez == req_rez.substr(0, rez.size()))
     return 0; //still don't know
   return -1; //bad anyway 
}
//                                                                
//                                                                 
int cl_resultator_length::put_next(char c)
{
   if (ignore.find(c) == string::npos) //we shouldn't ignore this character
     rez.push_back(c);
   if (rez.size() >= req_length)
     {
	return 1; //good
     }
   return 0;
}
//                                                                 
//                                                                 
int cl_resultator_multexact::put_next(char c)
{
   if (status) 
     return status; 
   if (ignore.find(c) != string::npos) //we should ignore this character
     return 0;   
   rez.push_back(c);      
   bool is_all_is_empty = true;
   for (size_t i = 0 ; i < req_rez.size() ; i++)
     if (req_rez[i].size() != 0)
     {
	int i_status = check_last_char(req_rez[i]);
	if (i_status == 0) //we should continue
	  {
	     is_all_is_empty = false; 
	  }
	else //we have finished with req_rez[i]
	  {
	     if (i_status == 1) //good
	       {
		  founds.push_back(req_rez[i]);	     
	       }
	     req_rez[i].clear();
	  }
     }
   if (!is_all_is_empty) //so we should continue
     return 0;
   if (founds.size() > 0)
     return status = 1; //good!
   return status = -1;  //fail
}
//                                                                          
void cl_resultator_multexact::get_not_finished(vector<string>& not_finished)
{
   not_finished.clear();
   for (size_t i = 0 ; i < req_rez.size() ; i++)
     if (req_rez[i].size() != 0)
       not_finished.push_back(req_rez[i]);
}
//                                                                          
int cl_resultator_multexact::check_last_char(string rr)
{
   if (rez.size() == 0) //shoudn't happen
     {
	cerr<<"Error | cl_resultator_multexact::check_last_char | internal error 0"<<endl;
	exit(EXIT_FAILURE);	
     }
   if (rr.size() < rez.size()) //shoudn't happen
     {
	cerr<<"Error | cl_resultator_multexact::check_last_char | internal error"<<endl;
	exit(EXIT_FAILURE);
     }   
   //first we check if result is bad 
   if (rr[rez.size() - 1] != rez[rez.size() - 1]) 
     return -1;   
   //good
   if (rr.size() == rez.size()) 
     return 1; 
   return 0;
}
//                                                                          
