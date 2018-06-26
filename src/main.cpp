#include <iostream>
#include "mempool_common.hpp"
#include "debug.hpp"
#include "vector.hpp"

//!< \file main.cpp
//!< \brief Client code for testing the GREMLINS
//!< \author Felipe Ramos and Max William

struct Tag { SLPool * pool; };
void * operator new( size_t bytes, SLPool & p ) /* throw (std::bad_alloc) */
{
	Tag * const m_tag = 
		reinterpret_cast<Tag *> (p.Allocate( bytes + sizeof(Tag) ));
	m_tag->pool = &p;
	std::cout << "Returned void *\n";
	return reinterpret_cast<void *>( m_tag + 1 );
}

void * operator new( size_type bytes ) throw (std::bad_alloc)
{
	Tag * const m_tag = 
		reinterpret_cast<Tag *>( std::malloc( bytes + sizeof(Tag)) );
	m_tag->pool = nullptr;
	return reinterpret_cast<void *>( m_tag + 1 );
}

void operator delete( void * arg ) noexcept {
	Tag * const m_tag = reinterpret_cast<Tag *>( arg ) - 1U;
	if( nullptr != m_tag->pool )
		// this means it belongs to a particular GM.
		m_tag->pool->Free( m_tag );
	else
		// this means it's a SO memory block
		std::free( m_tag );
}

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
	SLPool p(30);
	std::cout << "SLPool add: " << &p << std::endl;
	int * some_int = new(p) int;

	std::cout << "Stub\n";

	// delete some_int;
	return 0;
}
