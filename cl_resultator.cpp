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


