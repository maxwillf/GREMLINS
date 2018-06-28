#include <iostream>
#include "mempool_common.hpp"
#include "debug.hpp"
#include "vector.hpp"
#include <new>

//!< \file main.cpp
//!< \brief Client code for testing the GREMLINS
//!< \author Felipe Ramos and Max William

int main( int argc, char **argv ){
	// Driver code for testing the memory manager
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
	SLPool p(64);

	std::cout << "SLPool add: " << &p << std::endl;

	int * some_int = new(p) int;
	int * some_int2 = new(p) int;
//	int * some_int3 = new(p) int;

	std::cout << "Stub\n";

	delete some_int;
	delete some_int2;
	/*delete some_int3;*/
	return 0;
}
