//#include "config.h"
#include <iostream>
#include "MathLib.h"

void Print() { std::cout << std::endl; }

void Print(const char* text) { std::cout << text << std::endl; }

void Print(float v) { std::cout << round(v) << std::endl; }

void Print(V4 v)
{
	std::cout << '(';
	for (char i = 0; i < 4; i++)
		std::cout << round(v.data[i]) << (i == 3 ? ")\n" : ", ");
}

void Print(M4 m)
{
	for (char i = 0; i < 4; i++)
		Print(m[i]);
}

int main()
{
	Print(Rotation(V4(0, 0, 1), m_pi / 2));
	
	return 0;
}