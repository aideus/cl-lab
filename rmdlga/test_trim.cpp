
#include "cl_strga.h"
#include <iostream>
#include <cstdlib>

int main()
{
   srandom(time(NULL));
   string s1 = "qwerty";
   
   for (int i = 0 ; i < 10 ; i++)
     {
	string ns1 = s1;
	
	cl_str_trim_mutation(ns1, 0.5);
	
	cout<<ns1<<endl;
     }
}
