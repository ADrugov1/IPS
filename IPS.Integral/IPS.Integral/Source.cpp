#include <math.h>
#include <stdio.h>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <iostream>
#include <chrono>

using namespace std::chrono;

//// Функиця function, принимающая в качестве параметра точку x, 
////и вычисляющая значение функции 6.0 / sqrt(x*(2.0 - x)) в данной точке.
double function(double x)
{
	return 6.0 / sqrt(x*(2.0 - x));
}

//// Функиця integral, принимающая в качестве параметров
//// n - количество отрезков разбиения,
//// a - нижняя граница интеграла,
//// b - верхняя граница интеграла.

double Integral(int n, double a, double b)
{
	double integral = 0;
	double h = (b - a) /(double)n; //// Вычисление шага;

	//// Вычисление времени начала работы алгоритма
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	//// Вычисление значений функции в точках отрезка (a,b) можно выполнять параллельно.
	//// Поэтому в данном участке кода использую функцию cilk_for().
	cilk::reducer_opadd<double> sum(0.0);
	cilk_for(int i = 1; i < n; ++i)
	{
		double x = a + i*h;
		sum += function(x);
	}

	//// Для вычисления интеграла буду использовать метод трапeций
	integral = h*((function(a) + function(b)) / 2.0 + sum.get_value());

	//// Вычисление времени конца работы алгоритма
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	////Вычисление общего времени работы алгоритма
	duration<double> direct_motion_duration = (t2 - t1);

	printf("Time : %.10f ", direct_motion_duration.count());

	return integral;
}

void main()
{
	double a = 0.5; //// Нижняя граница интеграла.
	double b = 1.0; //// Верхняя граница интеграла.
	int n = 1000000; //// Количество участков разбиения. 

	//// Вывод результатов для разных значений параметра n;
	for ( n = 10; n <= 1000000000; n = n*10)
	{
	printf("Integral: %.20f , n: %d \n", Integral(n,a,b) ,n);
	}

 }