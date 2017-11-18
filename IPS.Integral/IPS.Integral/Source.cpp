#include <math.h>
#include <stdio.h>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <iostream>

double function(double x)
{
	return 6.0 / sqrt(x*(2.0 - x));
}

double Integral(int n, double a, double b)
{
	double integral = 0;
	double h = (b - a) / static_cast<double>(n);

	cilk::reducer_opadd<double> sum(0.0);
	cilk_for(int i = 1; i < n; ++i)
	{
		double x = a + i*h;
		sum += function(x);
	}
	////Для вычисления интеграла буду использовать метод трапeций
	integral = h*((function(a) + function(b)) / 2.0 + sum.get_value());

	return integral;
}

void main()
{
	double a = 0.5;
	double b = 1.0;
	int n = 10000000;

	double res = Integral(n, a, b);

	printf("Integral: %.10f\n", res);

}