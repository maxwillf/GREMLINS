#ifndef _MEMPOOL_COMMON_H_
#define _MEMPOOL_COMMON_H_

#include <stdlib.h>
#include <iostream> 
#include <new>
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

class SLPool/* : public StoragePool*/
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
			Header() : m_length(0u) {};
		};


		struct Tag
		{
			SLPool *pool;
		};

		struct Block : public Header
		{

			enum { BlockSize = 16 };
		
			union{

				Block *m_next;
				char m_raw[ BlockSize - sizeof(Header) ];
			};
			
			Block( ): Header() ,m_next(nullptr) {}

		};

		unsigned int m_n_blocks;
		Block *m_pool;
		Block *m_sentinel;


	public:
		explicit SLPool(size_type bytes);

		~SLPool();

		void * Allocate (size_type );
	//	void Release (Tag *);
		void Free (void *);
		
	//	void * operator new( size_type bytes, SLPool & p );
		
	//	void* operator new(size_t bytes);
	//	void operator delete(void*arg)noexcept;
};

#endif
