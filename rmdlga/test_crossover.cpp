
#include "cl_strga.h"
#include <iostream>
#include <cstdlib>

int main()
{
   srandom(time(NULL));
   string s1 = "qwerty";
   string s2 = "123456";
   
   for (int i = 0 ; i < 10 ; i++)
     {
	string ns1 = s1;
	string ns2 = s2;
	
	cl_str_crossover(ns1, ns2);
	
	cout<<ns1<<" "<<ns2<<endl;
     }
}
