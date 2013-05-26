//cl_mamga_tools.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#include "cl_mamga_tools.h"
#include <cmath>
#include <map>
#include <set>

//                                                                                                  
double cl_mamga_valuator::evaluate(const vector<string>& rez, const string& prog, double ac_K, double ac_Kxi)
{
   if (ac_K < 0) //compute automatically
     ac_K = cl_mamga_calc_ac_K(prog);
   
   if (ac_Kxi < 0)
     {
	cerr<<"Error | ac_Kxi should be given manually"<<endl;
	exit(EXIT_FAILURE);
     }
   
   part_L1 = ac_K * prog.size();
   part_L2 = 0;
   for (size_t i = 0 ; i < rez.size() ; i++ )
     part_L2 += ac_Kxi * rez[i].size();
   
   used_ac_K   = ac_K;
   used_ac_Kxi = ac_Kxi; 
   return part_L1 + part_L2;
}
//                                                                                      
double cl_mamga_calc_ac_K(string K)
{
   set<char> set_uniq;
   for (size_t i = 0 ; i < K.size() ; i++)
     if (K[i] != ')' && K[i] != '(' && K[i] != ' ')
       set_uniq.insert(K[i]);
   
   return log(double(set_uniq.size()) + 1) / log(2.0);
}
//                                                                                      
void cl_mamga_member::calc_Q()
{
   set<string> set_uniq;
   for (size_t i = 0 ; i < rez.size() ; i++)
     set_uniq.insert(rez[i]);
   Q = set_uniq.size();
}
//                                                                                      
