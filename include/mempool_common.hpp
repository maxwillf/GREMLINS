#ifndef _MEMPOOL_COMMON_H_
#define _MEMPOOL_COMMON_H_

#include <stdlib.h>
#include <iostream> 
// #include <new>

#define debug_constructor true
#define debug_allocate true

using size_type = size_t;

/** The minimal structure of other storage management classes. */ 
class StoragePool
{
	public:
		virtual ~StoragePool() { /* empty */ };
		/** Reserves size_t bytes and returns as a pointer. */
		virtual void * Allocate( size_t ) = 0;
		/** Recieve a pointer to a reserved region and free's it. */
		virtual void Free( void * ) = 0;
};

class SLPool : public StoragePool
{
	using size_type = size_t;
	protected: 
		/** Enumeration for status values. */
		enum status
		{
			m_free,
			m_used
		};
		
		/** Where the info about the block is located. */
		struct Header{
			unsigned int m_length;
			Header() : m_length(0u) {};
			Header( unsigned int _size ) : m_length(_size) {};
		};


		/** Tells which SLPool this block is from. */
		struct Tag
		{
			SLPool *pool;
		};

		/** Main struct for the data. */
		struct Block : public Header
		{
			enum { BlockSize = 16 };
			union{
				Block *m_next;
				char m_raw[ BlockSize - sizeof(Header) ];
			};
			
			/* Default constructor */
			Block(): Header() ,m_next(nullptr) {}
		};

		unsigned int m_n_blocks;
		Block *m_pool;
		Block *m_sentinel;


	public:
		explicit SLPool( size_type );
		~SLPool();

		void * Allocate ( size_type );
		void Release( Tag * );
		void Free (void *);

		/* Debug functions */
		/** Prints the Pool. */
		void print();
};

void * operator new( size_t bytes, SLPool & p );
void * operator new( size_type bytes );
void operator delete( void * arg ) noexcept;

#endif
