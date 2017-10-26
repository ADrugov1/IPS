#include <stdio.h>
#include <iostream>
#include <ctime>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <chrono>

using namespace std::chrono;
using namespace std;
// количество строк в исходной квадратной матрице
const int MATRIX_SIZE = 1500;

/// Функция InitMatrix() заполняет переданную в качестве 
/// параметра квадратную матрицу случайными значениями
/// matrix - исходная матрица СЛАУ
void InitMatrix( double** matrix )
{
	for ( int i = 0; i < MATRIX_SIZE; ++i )
	{
		matrix[i] = new double[MATRIX_SIZE + 1];
	}

	for ( int i = 0; i < MATRIX_SIZE; ++i )
	{
		for ( int j = 0; j <= MATRIX_SIZE; ++j )
		{
			matrix[i][j] = rand() % 2500 + 1;
		}
	}
}

/// Функция SerialGaussMethod() решает СЛАУ методом Гаусса 
/// matrix - исходная матрица коэффиициентов уравнений, входящих в СЛАУ,
/// последний столбей матрицы - значения правых частей уравнений
/// rows - количество строк в исходной матрице
/// result - массив ответов СЛАУ
double SerialGaussMethod(double **matrix, const int rows, double* result)
{
	int k;
	double koef;

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	// прямой ход метода Гаусса
	for (k = 0; k < rows; ++k)
	{
		//
		for (int i = k + 1; i < rows; ++i)
		{
			koef = -matrix[i][k] / matrix[k][k];

			for (int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}
	}

	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	duration<double> direct_motion_duration = (t2 - t1);

	// обратный ход метода Гаусса
	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

	for (k = rows - 2; k >= 0; --k)
	{
		result[k] = matrix[k][rows];

		//
		for (int j = k + 1; j < rows; ++j)
		{
			result[k] -= matrix[k][j] * result[j];
		}

		result[k] /= matrix[k][k];
	}
	return direct_motion_duration.count();
}



double SerialParallelGaussMethod(double **matrix, const int rows, double* result)
{

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	// прямой ход метода Гаусса
	for (int k = 0; k < rows; ++k)
	{
		//
		cilk_for (int i = k + 1; i < rows; ++i)
		{
			double koef = -matrix[i][k] / matrix[k][k];

			for (int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}
	}

	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	duration<double> direct_motion_duration = (t2 - t1);

	// обратный ход метода Гаусса
	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

	for (int k = rows - 2; k >= 0; --k)
	{
		result[k] = matrix[k][rows];

		//
		cilk::reducer_opadd<double> add_param(0.0);
		cilk_for(int j = k + 1; j < rows; ++j)
		{
			add_param += matrix[k][j] * result[j];
		}
		result[k] -= add_param.get_value();
		result[k] /= matrix[k][k];
	}

	return direct_motion_duration.count();
}

int main()
{
	srand((unsigned)time(0));

	int i;

	double **matrix = new double*[MATRIX_SIZE];
	double *result = new double[MATRIX_SIZE];
	double *parallelResult = new double[MATRIX_SIZE];

	InitMatrix(matrix);


	double algorithmTime = SerialGaussMethod(matrix, MATRIX_SIZE, result);
	cout << "Algorithm time: " << algorithmTime << " seconds\n" << endl;

	double parallelAlgorithmTime = SerialGaussMethod(matrix, MATRIX_SIZE, parallelResult);
	cout << "Parallel algorithm time: " << parallelAlgorithmTime << " seconds\n" << endl;

	for (i = 0; i < MATRIX_SIZE; ++i)
	{
		delete[]matrix[i];

	}
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		if (fabs(result[i] - parallelResult[i]) >= 0.00000001)
		{
			printf("%lf\n", fabs(result[i] - parallelResult[i]));
		}
	}

	cout << "Acceleration: " << algorithmTime/parallelAlgorithmTime << endl;

	delete[] result;
	delete[] parallelResult;

	return 0;
}