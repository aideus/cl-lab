//cl_ga.cpp
/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#include "sp_parmap.h"
#include "cl_amga_tools.h"
#include "cl_ga_tools.h"
#include "cl_brute.h"
#include <algorithm>
#include <fstream>

const char* defv[] = {
   "ansrez=\n                   Given ansamble + result",
   "prog_length=\n              Length of fake program",
   "alphabet=SKI\n              Alphabet",
   "ac_K=-1\n                   ac_K (if < 0 then log2(N_uniq_combinator + 0.5))",
   "ac_Kxi=-1\n                 ac_Kxi (if < 0 then H(symbols in ansamble))",   
   NULL
};

vector<string> ansamble;
vector<string> given_rez;
string alphabet;
double ac_K, ac_Kxi;
size_t prog_length;

void init(int argc, char*argv[]);
void calc_for_prog();
void read_ansrez(string file); //read ansamble and given_rez
string make_fake_program();

int main(int argc, char*argv[])
{
   init(argc, argv);   
   calc_for_prog();
}
//                                                                          
void init(int argc, char*argv[])
{
   sp_parmap p(argc, argv, defv);
   prog_length = p.get_i("prog_length");
   alphabet    = p.get_s("alphabet");
   ac_K        = p.get_d("ac_K");
   ac_Kxi      = p.get_d("ac_Kxi");
   
   
   read_ansrez(p.get_s("ansrez"));
}
//                                                                           
void calc_for_prog()
{ 
   
   for (size_t i = 0 ; i < ansamble.size() ; i++)     
     cout<<ansamble[i]<<" ----> "<<given_rez[i]<<endl;    
   
   string fake_prog = make_fake_program();
   cl_amga_valuator valuator;
   double penalty =  valuator.evaluate(given_rez, fake_prog, ac_K, ac_Kxi);
   cout<<"penalty="<<penalty<<endl;
   cout<<"part_L1="<<valuator.part_L1<<" part_L2="<<valuator.part_L2<<" part_H="<<valuator.part_H<<endl;
   cout<<"ac_K="<<valuator.used_ac_K<<" ac_Kxi="<<valuator.used_ac_Kxi<<endl;
}
//                                                                              
void read_ansrez(string file)
{
   ifstream in(file.c_str());
   ansamble.clear();
   given_rez.clear();
   string s1,s2;
   while(in>>s1>>s2)
     {
	ansamble.push_back(s1);
	given_rez.push_back(s2);
     }
}
//                                                                              
string make_fake_program()
{
   if (alphabet.size() > prog_length)
     {
	cerr<<"Error | make_fake_program | alphabet.size() > prog_length"<<endl;
	exit(EXIT_FAILURE);
     }
   string prog(prog_length, alphabet[0]);
   for (size_t i = 0 ; i < alphabet.size() ; i++)
     prog[i] = alphabet[i];
   return prog;
}
