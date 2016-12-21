#pragma once
#include<iostream>
#include<string>
#include<map>
#include<vector>
#include "soa.hpp"
#include "products.hpp"
#include<set>
#include<fstream>
#include<algorithm>
#include"productservice.h"
using namespace std;

string Price_To_Sring(double s) {
	string ss;
	ss += to_string(int(s));
	ss += "-";
	int a = int(s * 32);
	if (a<10) ss += "0";
	ss += to_string(a);
	s -= a / 32.0;
	ss += to_string(int(s*256.0));
	return ss;
}

double String_To_Price(string s)
{
	double a = stod(s.substr(0, s.size() - 4));
	a += 1.0 / 32.0*stod(s.substr(s.size() - 3, 2));
	a += 1.0 / 256.0*stod(s.substr(s.size() - 1, 1));
	return a;
}