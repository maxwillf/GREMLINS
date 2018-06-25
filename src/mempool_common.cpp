#include "mempool_common.hpp"


SLPool::SLPool(size_type bytes){
	m_pool = (Block * ) malloc(bytes+sizeof(Block));
	m_pool->m_length = bytes;
	m_pool->m_next = nullptr;

	m_sentinel = m_pool;
	m_sentinel += bytes/sizeof(Block);
	m_sentinel->m_length = 0;
	m_sentinel->m_next = m_pool;


	std::cout << "Size of block char: " << sizeof(char[Block::BlockSize]) << std::endl;
	std::cout << "Size of block *: " << sizeof(Block *) << std::endl;
	std::cout << "Size of block char minus header: " << sizeof(char[Block::BlockSize-sizeof(Header)]) << std::endl;
	std::cout << "Size of block: " << sizeof(Block) << std::endl;
	std::cout << "Size of header: "<< sizeof(Header) << std::endl;
	std::cout << m_pool->m_length << std::endl;
	std::cout << m_sentinel->m_next->m_length << std::endl;
}


SLPool::~SLPool(){

	free(m_pool);

}

void * SLPool::Allocate(size_type bytes){
	
	auto ptr = m_sentinel->m_next;
	Block * before_begin = m_sentinel;
	auto bytes_to_alloc =  (bytes+sizeof(Header))/sizeof(Block);

	while(ptr != nullptr){
		if(ptr->m_length >= bytes_to_alloc )
		{
			if(ptr->m_length == bytes_to_alloc){
				before_begin->m_next = ptr->m_next;
				return reinterpret_cast<Header *> (ptr + 1U);
			}
			
			else{
				before_begin->m_next = (Block *) (ptr+ptr->m_length);
				return reinterpret_cast<Header *> (ptr + 1U);
			}

			before_begin = ptr;
			ptr = ptr->m_next;
		}
	}

	throw std::bad_alloc();
}

void SLPool::Free(void * ptr){

	ptr = reinterpret_cast<Block *> (reinterpret_cast < Header *> (ptr - 1U);
	
	auto ptr_aux = m_sentinel->m_next;
	Block * ptr_prev = nullptr;
	Block * ptr_post = nullptr;

	while( ptr_aux != nullptr)
	{
		if(ptr_aux < ptr){
			if(ptr_aux + ptr_aux->m_length == ptr)
			ptr_prev = ptr_aux + ptr->aux->m_length;
		}
		else if(ptr_aux - ptr_aux->m_length == ptr){
			ptr_post = ptr_aux + ptr->aux->m_length;
		}

		ptr_aux = ptr_aux->m_next;
	}
	/* TODO */
	if(ptr_prev != nullptr and ptr_post != nullptr){
		ptr_prev->m_next = ptr_post->m_next;
	}
}



		void * SLPool::operator new( size_type bytes, SLPool & p ){
			Tag* const tag = reinterpret_cast<Tag *> (p.Allocate( bytes * sizeof(Tag)) ); 
			tag->pool = &p;

			return (reinterpret_cast<void *> (tag + 1U) );
		}
		void* SLPool::operator new(size_t bytes){//Regula rnew
			Tag* const tag = reinterpret_cast <Tag *> (malloc(bytes+sizeof(Tag) ) );
			tag->pool=nullptr;
			
			return(reinterpret_cast<void*>(tag+1U) );
		}
/*
		void SLPool::operator delete(void*arg)noexcept{
			//We need to subtract 1U(infact,pointer arithmetics)because arg
			//points to the raw data(second block of information).
			//The pool id(tag) is located ‘sizeof(Tag)’bytes before.
			
			Tag* const tag = reinterpret_cast<Tag*> (arg)-1U;
			if(nullptr!=tag->pool)//Memory block belongs to a particularGM.
				tag->pool->Release(tag);
			else
				free(tag);//Memory block belongs to the operational system.
		}*/
