//cl_strga.cpp

#include "cl_strga.h"
#include "cl_basa.h" //for random for windows...
#include <iostream>
#include <cstdlib>

//                                                                  
void cl_str_crossover(string& s1, string& s2)
{
   size_t pos_s1, len_s1;
   size_t pos_s2, len_s2;
   
   cl_str_random_substr(s1, pos_s1, len_s1);
   cl_str_random_substr(s2, pos_s2, len_s2);
   
   string sub_s1    = s1.substr(pos_s1, len_s1);
   string sub_s2    = s2.substr(pos_s2, len_s2);
   
   s1.replace(pos_s1, len_s1, sub_s2);
   s2.replace(pos_s2, len_s2, sub_s1);
}
//                                                                  
void cl_str_echange_mutation(string& s, double p, string alphabet)
{
   for (size_t i = 0 ; i < s.size() ; i++)
     {
	if ( random()/(double)RAND_MAX < p )
	  {
	     s[i] = alphabet[random() % alphabet.size()];
	  }
     }
}
//                                                                  
void cl_str_trim_mutation(string&s, double p)
{
   if ( random()/(double)RAND_MAX < p )
     {
	size_t p, len;
	cl_str_random_substr(s, p, len);
	s.erase(p, len);
     }
}
//                                                                  
void cl_str_random_substr(string s, size_t& p, size_t& len)
{
   if (s.empty())
     {
	p   = 0;
	len = 0;
	return;
//	cerr<<"Error | cl_random_substr | empty string"<<endl;
//	exit(EXIT_FAILURE);
     }
   p   = random() % s.size();
   len = random() % (s.size() - p) + 1;
   if (p + len > s.size())
     {
	cerr<<"Error | cl_strga::random_substr | logic error"<<endl;
	exit(EXIT_FAILURE);
     }
}
//                                                    
string cl_str_make_random(string alphabet, size_t size)
{
   string s;
   s.resize(size);
   for (size_t i = 0 ; i < s.size() ; i++)
     s[i] = alphabet[random() % alphabet.size()];
   return s;
}

