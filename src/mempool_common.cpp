#include "mempool_common.hpp"


SLPool::SLPool(size_type bytes){
	m_pool = (Block * ) malloc(bytes+sizeof(Block));
	m_pool->m_length = bytes;
	m_pool->m_next = nullptr;

	m_sentinel = m_pool;
	m_sentinel += bytes/sizeof(Block);
	m_sentinel->m_length = 0;
	m_sentinel->m_next = m_pool;


	std::cout << sizeof(Block) << std::endl;
	std::cout << m_pool->m_length << std::endl;
	std::cout << m_sentinel->m_next->m_length << std::endl;
}


SLPool::~SLPool(){

	free(m_pool);

}
/*
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
