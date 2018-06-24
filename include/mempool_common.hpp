#ifndef _MEMPOOL_COMMON_H_
#define _MEMPOOL_COMMON_H_

#include <stdlib.h>

using size_type = size_t;

/** The minimal structure of other storage management classes. */ 
class StoragePool
{
	public:
		virtual ~StoragePool();
		/** Reserves size_t bytes and returns as a pointer. */
		virtual void * Allocate( size_t ) = 0;
		/** Recieve a pointer to a reserved region and free's it. */
		virtual void Free( void * ) = 0;
};

class SLPool : public StoragePool
{
	using size_type = size_t;
	private: 
		enum status
		{
			m_free,
			m_used
		};
		
		struct Header{
			unsigned int m_length;
			Header(size_type sz = 0) : m_length(sz) {};
		};


		struct Tag
		{
			int m_id;		// stub
			size_type m_size;
			SLPool *pool;
		};

		struct Block : public Header
		{
		//	Header head;

			enum { BlockSize = 16 };
			union{
				Header head;
				union{
					Block *m_next;
					char m_raw[BlockSize - sizeof(Header) ];
				};
				char m_raw2[BlockSize];
			};
			
			Block(size_type sz): Header(sz) ,m_next(nullptr) {}

		};

		unsigned int m_n_blocks;
		Block *m_pool;
	//	Block &m_sentinel;


	public:
		SLPool(size_type bytes){
			m_pool = (Block * ) malloc(bytes);
		}

		~SLPool( void );
		void * Allocate (size_type );
		void Release (Tag *);
		void Free (void *);
		
		void * operator new( size_type bytes, SLPool & p ){
			Tag* const tag = reinterpret_cast<Tag *> (p.Allocate( bytes * sizeof(Tag)) ); 
			tag->pool = &p;

			return (reinterpret_cast<void *> (tag + 1U) );
		}
		void* operator new(size_t bytes){//Regula rnew
			Tag* const tag = reinterpret_cast <Tag *> (malloc(bytes+sizeof(Tag) ) );
			tag->pool=nullptr;
			
			return(reinterpret_cast<void*>(tag+1U) );
		}
		void operator delete(void*arg)noexcept{
			//We need to subtract 1U(infact,pointer arithmetics)because arg
			//points to the raw data(second block of information).
			//The pool id(tag) is located ‘sizeof(Tag)’bytes before.
			
			Tag* const tag = reinterpret_cast<Tag*> (arg)-1U;
			if(nullptr!=tag->pool)//Memory block belongs to a particularGM.
				tag->pool->Release(tag);
			else
				free(tag);//Memory block belongs to the operational system.
		}
};

#endif
