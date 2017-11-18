#include <math.h>
#include <stdio.h>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <iostream>
#include <chrono>

using namespace std::chrono;

//// ������� function, ����������� � �������� ��������� ����� x, 
////� ����������� �������� ������� 6.0 / sqrt(x*(2.0 - x)) � ������ �����.
double function(double x)
{
	return 6.0 / sqrt(x*(2.0 - x));
}

//// ������� integral, ����������� � �������� ����������
//// n - ���������� �������� ���������,
//// a - ������ ������� ���������,
//// b - ������� ������� ���������.

double Integral(int n, double a, double b)
{
	double integral = 0;
	double h = (b - a) /(double)n; //// ���������� ����;

	//// ���������� ������� ������ ������ ���������
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	//// ���������� �������� ������� � ������ ������� (a,b) ����� ��������� �����������.
	//// ������� � ������ ������� ���� ��������� ������� cilk_for().
	cilk::reducer_opadd<double> sum(0.0);
	cilk_for(int i = 1; i < n; ++i)
	{
		double x = a + i*h;
		sum += function(x);
	}

	//// ��� ���������� ��������� ���� ������������ ����� ����e���
	integral = h*((function(a) + function(b)) / 2.0 + sum.get_value());

	//// ���������� ������� ����� ������ ���������
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	////���������� ������ ������� ������ ���������
	duration<double> direct_motion_duration = (t2 - t1);

	printf("Time : %.10f ", direct_motion_duration.count());

	return integral;
}

void main()
{
	double a = 0.5; //// ������ ������� ���������.
	double b = 1.0; //// ������� ������� ���������.
	int n = 1000000; //// ���������� �������� ���������. 

	//// ����� ����������� ��� ������ �������� ��������� n;
	for ( n = 10; n <= 1000000000; n = n*10)
	{
	printf("Integral: %.20f , n: %d \n", Integral(n,a,b) ,n);
	}

 }