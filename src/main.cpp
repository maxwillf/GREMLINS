#include <iostream>
#include "mempool_common.hpp"
#include "debug.hpp"
#include "vector.hpp"
#include <new>
#include <cassert>

//!< \file main.cpp
//!< \brief Client code for testing the GREMLINS
//!< \author Felipe Ramos and Max William

int main( int argc, char **argv ){
	{
		try{
			sc::vector<int> simpleVector = { 0, 2, 4, 6, 8, 10 };
			std::cout << "> Print simpleVector" << std::endl;
			std::cout << "[ ";
			for( auto &i : simpleVector ){
				std::cout << i << " ";
			}
			std::cout << "]\n";
		} catch( ... ){
			std::cout << "Some error had happened!" << std::endl;
		}
	}
	SLPool p(300);
	{
		int * some_int = new(p) int;
		int * some_int2 = new(p) int;
		int * some_int4= new(p) int;
		int * some_int3 = new(p) int;
		int * some_int5 = new(p) int;
		p.print();


		delete some_int2;
		p.print();
		delete some_int;
		p.print();
		delete some_int3;
		p.print();

		delete some_int4;
		delete some_int5;
	}
	{
		int * some_int8= new(p) int;
		some_int8[0] = 5;
		assert( *some_int8 == 5 );
		delete some_int8;
		int * some_int9 = new(p) int;
		delete some_int9;
		int * some_int7= new(p) int;
		delete some_int7;
		int * some_int6 = new(p) int;
		delete some_int6;
	}
	{
		struct teste{
			int a,b,c;
		};

		teste* testezin = new (p) teste;
		testezin->a = 3;
		testezin->b = 4;
		testezin->c = 5;

		assert(testezin->a == 3);
		assert(testezin->b == 4);
		assert(testezin->c == 5);

		delete testezin;
	}

	{
		// Testing the normal new operator
		int *a = new int;
		*a = 5;
		assert(*a == 5);
		delete a;
	}

	
	std::cout << "Execution finished successfully!\n";
	return 0;
}
