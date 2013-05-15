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
#include <set>

//                                                                                                  
double cl_amga_valuator::evaluate(const vector<string>& rez, const string& prog, double ac_K, double ac_Kxi)
{
   if (ac_K < 0) //compute automatically
     ac_K = cl_amga_calc_ac_K(prog);
   
   if (ac_Kxi < 0)
     ac_Kxi = cl_amga_calc_ac_Kxi(rez);
   
   part_L1 = ac_K * prog.size();
   part_L2 = 0;
   for (size_t i = 0 ; i < rez.size() ; i++ )
     part_L2 += ac_Kxi * (rez[i].size() + 1);
   
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
	H -= Pi * log(Pi) / log(2.0); //Pi * log2(Pi);
     }
//   cout<<"H="<<H<<endl;
   part_H = -(rez.size() * H);
   used_ac_K   = ac_K;
   used_ac_Kxi = ac_Kxi; 
   return part_L1 + part_L2 + part_H;
}
//                                                                                  
double cl_amga_calc_ac_Kxi(const vector<string>& rez)
{
   set<char> set_uniq;
   for (size_t i = 0 ; i < rez.size() ; i++)
     for (size_t j = 0 ; j < rez[i].size() ; j++)
       set_uniq.insert(rez[i][j]);
   
   return log(double(set_uniq.size()) + 1.0) / log(2.0);
}
//                                                                                      
double cl_amga_calc_ac_K(string K)
{
   set<char> set_uniq;
   for (size_t i = 0 ; i < K.size() ; i++)
     if (K[i] != ')' && K[i] != '(' && K[i] != ' ')
       set_uniq.insert(K[i]);
   
   return log(double(set_uniq.size()) + 0.5) / log(2.0);
}
//                                                                                      
