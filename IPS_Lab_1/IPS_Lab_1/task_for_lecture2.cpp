#include <iostream>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <chrono>


using namespace std::chrono;
using namespace std;

/// ‘ункци€ ReducerMinTest() определ€ет минимальный элемент массива,
/// переданного ей в качестве аргумента, и его позицию
/// mass_pointer - указатель исходный массив целых чисел
/// size - количество элементов в массиве
void ReducerMinTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_min_index<long, int>> minimum;
	cilk_for(long i = 0; i < size; ++i)
	{
		minimum->calc_min(i, mass_pointer[i]);
	}
	printf("Minimal element = %d has index = %d\n\n",
		minimum->get_reference(), minimum->get_index_reference());
}

/// ‘ункци€ ReducerMaxTest() определ€ет максимальный элемент массива,
/// переданного ей в качестве аргумента, и его позицию
/// mass_pointer - указатель исходный массив целых чисел
/// size - количество элементов в массиве
void ReducerMaxTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_max_index<long, int>> maximum;
	cilk_for(long i = 0; i < size; ++i)
	{
		maximum->calc_max(i, mass_pointer[i]);
	}
	printf("Maximal element = %d has index = %d\n\n",
		maximum->get_reference(), maximum->get_index_reference());
}

/// ‘ункци€ ParallelSort() сортирует массив в пор€дке возрастани€
/// begin - указатель на первый элемент исходного массива
/// end - указатель на последний элемент исходного массива
void ParallelSort(int *begin, int *end)
{
	if (begin != end) 
	{
		--end;
		int *middle = std::partition(begin, end, std::bind2nd(std::less<int>(), *end));
		std::swap(*end, *middle); 
		cilk_spawn ParallelSort(begin, middle);
		ParallelSort(++middle, ++end);
		cilk_sync;
	}
}

void CompareForAndCilk_For(size_t size) 
{
	cout << "Vectors size is: " << size << " elements\n" << endl;

	srand((unsigned)time(0));

	vector<int> vector;
	cilk::reducer<cilk::op_vector<int>>red_vector;

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	for (int i = 0; i < size;  ++i)
	{
		vector.push_back(rand() % 20000 + 1);
	}
	
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	cilk_for(int i = 0; i < size; ++i)
	{
		red_vector->push_back(rand() % 20000 + 1);
	}

	high_resolution_clock::time_point t3 = high_resolution_clock::now();

	duration<double> for_duration = (t2 - t1);
	duration<double> cilk_for_duration = (t3 - t2);

	cout << "For duration is: " << for_duration.count() << " seconds\n" << endl;
	cout << "Cilk_for duration is: " << cilk_for_duration.count() << " seconds\n" << endl;
	cout << endl; // форматирование вывода
}


int main()
{
	

	//  од дл€ выполнени€ пунктов 1,2 и 3 -------------------------------------------
	
	/*

	srand((unsigned)time(0));

	// устанавливаем количество работающих потоков = 4
	__cilkrts_set_param("nworkers", "4");

	long i;
	const long mass_size = 500000;
	int *mass_begin, *mass_end;
	int *mass = new int[mass_size]; 

	for(i = 0; i < mass_size; ++i)
	{
		mass[i] = (rand() % 25000) + 1;
	}
	
	mass_begin = mass;
	mass_end = mass_begin + mass_size;

	cout << "Massive size is: " << mass_size << " elements\n" << endl;

	ReducerMinTest(mass, mass_size);
	ReducerMaxTest(mass, mass_size);

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	ParallelSort(mass_begin, mass_end);

	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	duration<double> duration = (t2 - t1);

	cout << "Duration is: " << duration.count() << " seconds\n" << endl;

	ReducerMinTest(mass, mass_size);
	ReducerMaxTest(mass, mass_size);

	delete[]mass;

	*/

	//  од дл€ выполнени€ пункта 4 ------------------------------------------------
	
	__cilkrts_set_param("nworkers", "4");

	CompareForAndCilk_For(1000000);
	CompareForAndCilk_For(100000);
	CompareForAndCilk_For(10000);
	CompareForAndCilk_For(1000);
	CompareForAndCilk_For(500);
	CompareForAndCilk_For(100);
	CompareForAndCilk_For(50);
	CompareForAndCilk_For(10);

 	return 0;
}