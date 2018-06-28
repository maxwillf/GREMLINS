#include "mempool_common.hpp"
#include <cmath>
SLPool::SLPool( size_type bytes ){
	this->m_pool = (Block *) malloc( bytes + sizeof(Block) );
	this->m_pool->m_length = ((float)bytes+sizeof(Block))/16;
	this->m_pool->m_next = nullptr;

	this->m_sentinel = this->m_pool;
	this->m_sentinel += (bytes / sizeof(Block));
	this->m_sentinel->m_length = 0;
	this->m_sentinel->m_next = this->m_pool;


	if( debug_constructor ){
		std::cout << "=== CONSTRUCTOR DEBUG ==="
			<< "\nSize of block char: " 
			<< sizeof( char[Block::BlockSize] )
			<< "\nSize of block *: " << sizeof(Block *)
			<< "\nSize of block char minus header: " 
			<< sizeof( char[Block::BlockSize-sizeof(Header)] )
			<< "\nSize of block: " << sizeof(Block)
			<< "\nSize of header: "<< sizeof(Header)
			<< "\nthis->m_pool->m_lenght = " << this->m_pool->m_length
			<< "\nthis->m_sentinel->m_next->m_length = " 
			<< this->m_sentinel->m_next->m_length 
			<< "\n=== END OF CONSTRUCTOR DEBUG ==="<< std::endl;
	}
}

SLPool::~SLPool(){
	free(this->m_pool);
}

void * SLPool::Allocate( size_type bytes ){
	Block * m_ptr = this->m_sentinel->m_next;
	Block * before_begin = this->m_sentinel;
	int blocks_to_alloc = std::ceil(float( bytes + sizeof(Header) ) / sizeof(Block));
	
	while( m_ptr != nullptr )
	{
		if( m_ptr->m_length >= blocks_to_alloc )
		{
			if( m_ptr->m_length == blocks_to_alloc )
			{
				
				before_begin->m_next = m_ptr->m_next;
				std::cout << ".Allocate() returning #1\n";
				return reinterpret_cast<void*> (reinterpret_cast<Header *> (m_ptr) + (1U));
			}
			else
			{
				std::cout << ".Allocate() returning #2\n";
				Block * broken = (Block *) m_ptr + blocks_to_alloc;
				// old policybefore_begin->m_next = broken;
				// skips a ptr 
				before_begin->m_next = m_ptr->m_next; 
				broken->m_length = m_ptr->m_length - blocks_to_alloc;
			//	broken->m_next = m_ptr->m_next;
				insert_ord(broken);
				m_ptr->m_length = blocks_to_alloc;
				return reinterpret_cast<void*> ( reinterpret_cast <Header *> (m_ptr) + (1U));

			}

		}
			before_begin = m_ptr;
			m_ptr = m_ptr->m_next;
	}

	throw std::bad_alloc();
}


void SLPool::Free( void * ptr ){

	ptr = reinterpret_cast<Block *> (reinterpret_cast <Header *> (ptr) - (1U));
	
	Block * ptr_aux = m_sentinel->m_next;
	Block * input_ptr = (Block *) ptr;
	Block * before_begin = m_sentinel;
	
	while(ptr_aux != nullptr){

		if(ptr_aux > input_ptr){
			break;		
		}
		before_begin = ptr_aux;
		ptr_aux = ptr_aux->m_next;
	}


    // The area is directly between two free areas
    if (before_begin + before_begin->m_length == input_ptr
        and input_ptr + input_ptr->m_length == ptr_aux)
    {
        before_begin->m_length += ptr_aux->m_length + input_ptr->m_length;
        before_begin->m_next = ptr_aux->m_next;
    }
    // The area is not directly between any other free areas
    else if (before_begin + before_begin->m_length != input_ptr
            and input_ptr + input_ptr->m_length != ptr_aux)
    {
        before_begin->m_next = input_ptr;
        input_ptr->m_next = ptr_aux;
    }
    // There is a free area directly before and no other
    else if (before_begin + before_begin->m_length == input_ptr
            and input_ptr + input_ptr->m_length != ptr_aux)
    {
        before_begin->m_length += input_ptr->m_length;
        before_begin->m_next = ptr_aux;
    }
    // The area is a free area directly after and no other
    else
    {
        input_ptr->m_length += ptr_aux->m_length;
        before_begin->m_next = input_ptr;
        input_ptr->m_next = ptr_aux->m_next;
    }
	
}

void SLPool::print( void ){
	
}

struct Tag { SLPool * pool; };
void * operator new( size_t bytes, SLPool & p ) /* throw (std::bad_alloc) */
{
	Tag * const m_tag = 
		reinterpret_cast<Tag *> (p.Allocate( bytes + sizeof(Tag) ));
	m_tag->pool = &p;
	std::cout << "Returned void *\n";
	return reinterpret_cast<void *>( m_tag + 1 );
}

void * operator new( size_type bytes ) 
{
	Tag * const m_tag = 
		reinterpret_cast<Tag *>( std::malloc( bytes + sizeof(Tag)) );
	m_tag->pool = nullptr;
	std::cout << "Returning void normal!\n";
	return reinterpret_cast<void *>( m_tag + 1 );
}

void operator delete( void * arg ) noexcept {
	Tag * const m_tag = reinterpret_cast<Tag *>( arg ) - 1U;
	std::cout << "Our delete ivkd\n";
	if( nullptr != m_tag->pool )
		// this means it belongs to a particular GM.
		m_tag->pool->Free( m_tag );
	else
		// this means it's a SO memory block
		std::free( m_tag );
}
