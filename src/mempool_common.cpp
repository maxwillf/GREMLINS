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
		std::cout << "=== DEBUG ==="
			<< "\nSize of block char: " 
			<< sizeof( char[Block::BlockSize] )
			<< "\nSize of block *: " << sizeof(Block *)
			<< "\nSize of block char minus header: " 
			<< sizeof( char[Block::BlockSize-sizeof(Header)] )
			<< "\nSize of block: " << sizeof(Block)
			<< "\nSize of header: "<< sizeof(Header)
			<< "\n" << this->m_pool->m_length
			<< "\n" << this->m_sentinel->m_next->m_length << std::endl;
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
				return reinterpret_cast<Header *> (m_ptr) + (1U);
			}
			else
			{
				before_begin->m_next = (Block *) ( m_ptr + m_ptr->m_length );
				return reinterpret_cast<Header *> (m_ptr) + (1U);
			}

			before_begin = m_ptr;
			m_ptr = m_ptr->m_next;
		}
	}

	throw std::bad_alloc();
}

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

/*

void * SLPool::operator new( size_type bytes, SLPool & p ){
	Tag* const tag =
		reinterpret_cast<Tag *> (p.Allocate( bytes * sizeof(Tag)) ); 
	tag->pool = &p;

	return (reinterpret_cast<void *> (tag + 1U) );
}
void* SLPool::operator new(size_t bytes){//Regula rnew
	Tag* const tag =
		reinterpret_cast <Tag *> (malloc(bytes+sizeof(Tag)));
	tag->pool=nullptr;
	
	return(reinterpret_cast<void*>(tag+1U) );
}
void SLPool::operator delete(void*arg)noexcept{
	//We need to subtract 1U(infact,pointer arithmetics)because arg
	//points to the raw data(second block of information).
	//The pool id(tag) is located ‘sizeof(Tag)’bytes before.
	
	Tag* const tag = reinterpret_cast<Tag*> (arg)-1U;
	if(nullptr!=tag->pool)//Memory block belongs to a particularGM.
		tag->pool->Release(tag);
	else
		free(tag);//Memory block belongs to the operational system.
}

*/
