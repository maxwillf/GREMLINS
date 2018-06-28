#ifndef _MEMPOOL_COMMON_H_
#define _MEMPOOL_COMMON_H_

/**
 *	\file	mempool_common.hpp
 *	\author	Felipe Ramos e Max William
 */

#include <stdlib.h>
#include <iostream> 

#define debug_constructor false
#define debug_allocate false

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

/** An actual implementation of a Memory Pool. */
class SLPool : public StoragePool
{
	using size_type = size_t;
	protected: 
		/** Where the info about the block is located. */
		struct Header{
			unsigned int m_length;
			Header() : m_length(0u) {};
			Header( unsigned int _size ) : m_length(_size) {};
		};


		/** Tells which SLPool this block is from. */
		struct Tag { SLPool *pool; };

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

		unsigned int m_n_blocks;		//!< Tells the total blocks in the Pool
		Block *m_pool;					//!< The pool itself
		Block *m_sentinel;				//!< The sentinel, it's located on the end

		/** Function that will insert a Block on best position. */
		void insert_ord( Block * ptr )
		{
			Block * iter = m_sentinel;

			while(iter->m_next != nullptr)
			{
				if(ptr < iter->m_next){
					ptr->m_next;
					iter->m_next = ptr;
					return;
				} else {
					iter = iter->m_next;
				}
			}

			if(iter->m_next == nullptr)
			{
				iter->m_next = ptr;
				ptr->m_next = nullptr;
				return;
			}
		}
	public:
		/** Constructs the pool with the necessary amount of bytes converted
		 * into blocks. */
		explicit SLPool( size_type );

		/** The SLPool destructor */
		~SLPool();

		/** Responsable for allocating the elements on the pool.
		 * \return	Returns a void pointer to the client_data address */
		void * Allocate ( size_type );

		/** Responsible for dealocatting elements of the pool.*/
		void Free (void *);

		/** Prints the pool used and free blocks */
		void print();
};

/** New operator (with args) overloaded for provide a easy method to allocate
 * things. */
void * operator new( size_t bytes, SLPool & p );

/** New operator overloaded for provide a easy method to allocate things. */
void * operator new( size_type bytes );

/** New operator overloaded for provide a easy method to allocate things. */
void operator delete( void * arg ) noexcept;

#endif
