#include "mempool_common.hpp"

SLPool::SLPool( size_type bytes ){
	this->m_pool = (Block *) malloc( bytes + sizeof(Block) );
	this->m_pool->m_length = bytes;
	this->m_pool->m_next = nullptr;

	this->m_sentinel = this->m_pool;
	this->m_sentinel += bytes / sizeof(Block);
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

	Block * before_begin = m_ptr;
	auto bytes_to_alloc = ( bytes + sizeof(Header) ) / sizeof(Block);

	while( m_ptr != nullptr )
	{
		if( m_ptr->m_length >= bytes_to_alloc )
		{
			if( m_ptr->m_length == bytes_to_alloc )
			{
				before_begin->m_next = m_ptr->m_next;
				std::cout << ".Allocate() returning #1\n";
				return reinterpret_cast<Header *> (m_ptr) + (1U);
			}
			else
			{
				before_begin->m_next = (Block *) ( m_ptr + m_ptr->m_length );
				std::cout << ".Allocate() returning #2\n";
				return reinterpret_cast<Header *> (m_ptr) + (1U);
			}

			before_begin = m_ptr;
			m_ptr = m_ptr->m_next;
		}
	}

	throw std::bad_alloc();
}

void SLPool::Release( Tag * m_tag ){ /* TODO */ }

void SLPool::Free( void * ptr ){

	ptr = reinterpret_cast<Block *> (reinterpret_cast <Header *> (ptr)) - 1U;
	
	Block * ptr_aux = m_sentinel->m_next;
	Block * input_ptr = (Block *) ptr;
	Block * before_begin = ptr_aux;
	Block * before_prev = nullptr;
	Block * ptr_prev = nullptr;
	Block * ptr_post = nullptr;

	while( ptr_aux != nullptr )
	{
		if(( ptr_aux < ptr ) and ( ptr_aux + ptr_aux->m_length == ptr ))
		{
			before_prev = before_begin;
			ptr_prev = ptr_aux;
		}
		else if( ptr_aux - ptr_aux->m_length == ptr )
		{
			ptr_post = ptr_aux;
			before_begin->m_next = before_begin->m_next->m_next;
			input_ptr->m_length += ptr_aux->m_length;
			ptr_aux->m_length = 0;
		}
		
		before_begin = ptr_aux;
		ptr_aux = ptr_aux->m_next;
		
		if( ptr_prev != nullptr and ptr_post != nullptr )
		{
			ptr_prev->m_length += input_ptr->m_length;
			break;
		}
	}
	
	if( ptr_aux == nullptr and ptr_prev != nullptr )
		ptr_prev->m_length += input_ptr->m_length;
}
