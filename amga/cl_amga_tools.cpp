//cl_amga_tools.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#include "cl_amga_tools.h"
#include <cmath>
#include <map>

//                                                                                                  
double cl_amga_valuator::evaluate(const vector<string>& rez, size_t prog_len, double ac_K, double ac_Kxi)
{
   double val = ac_K * prog_len;
   for (size_t i = 0 ; i < rez.size() ; i++ )
     val += ac_Kxi * rez[i].size();
   part_L = val;
   //now we calculate frequences
   map<string, size_t> freq_map;
   for (size_t i = 0 ; i < rez.size() ; i++)
     {
	map<string, size_t>::iterator it = freq_map.find(rez[i]);
	if (it == freq_map.end())
	  {
	     freq_map[rez[i]] = 1;
	  }
	else
	  it->second++;
     }
   double H = 0;
   for (map<string, size_t>::iterator it = freq_map.begin() ; it != freq_map.end() ; it++)
     {
	double Pi = double(it->second) / double(rez.size());
//	cout<<"Pi="<<Pi<<endl;
	H -= Pi * log(Pi) / M_LN2; //Pi * log2(Pi);
     }
//   cout<<"H="<<H<<endl;
   val -= double(rez.size()) * H;
   part_H = -(rez.size() * H);
//   cout<<"val="<<val<<endl;
   return val;
}
