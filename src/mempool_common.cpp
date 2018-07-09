#include "mempool_common.hpp"
#include <cmath>
SLPool::SLPool( size_type bytes ){
	/* an useful alias */
	long int n_blocks = ceil(((double)bytes + sizeof(Header))/sizeof(Block));

	this->m_pool = (Block *) malloc (n_blocks * sizeof(Block) + sizeof(Block));
	this->m_pool->m_length = n_blocks;
	this->m_n_blocks = n_blocks;
	this->m_pool->m_next = nullptr;

	this->m_sentinel = this->m_pool;
	this->m_sentinel += n_blocks;
	this->m_sentinel->m_length = 0;
	this->m_sentinel->m_next = this->m_pool;


	if( debug_constructor ){
		if(false)std::cout << "=== CONSTRUCTOR DEBUG ==="
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
		std::cout << "POOL Debug:\n"
			<< "\tsizeof(block) = " << sizeof(Block)
			<< "\n\tm_pool size (in bytes): "
			<< ceil(bytes/sizeof(Block)) * sizeof(Block) + sizeof(Block)
			<< "\n\tm_pool size (in blocks): " 
			<< this->m_pool->m_length
			<< std::endl;
	}
}

SLPool::~SLPool(){
	free(this->m_pool);
}

void * SLPool::Allocate( size_type bytes ){
	Block * it = this->m_sentinel->m_next;
	long int n_blocks = std::ceil(double( bytes + sizeof(Header) ) / sizeof(Block));
	
	/* Iterates through free blocks */
	while( it != nullptr )
	{
		/* if the lenght is bigger than what we need... */
		if( it->m_length >= n_blocks )
		{
			/* if it's equal to what we need, it will be choosen */
			if( it->m_length == n_blocks )
			{
				this->m_sentinel->m_next = it->m_next;
				return reinterpret_cast<void*> (reinterpret_cast<Header *> (it) + (1U));
			}

			/* we will split into two pieces */
			else
			{
				Block * new_free = (Block *) it + n_blocks;		// new free area
				// old policythis->m_sentinel->m_next = new_free;
				// skips a ptr 
				this->m_sentinel->m_next = it->m_next; 		// update m_sentinel
				new_free->m_length = it->m_length - n_blocks;	// update lenght
				//	new_free->m_next = it->m_next;
				insert_ord( new_free );
				it->m_length = n_blocks;			// set new lenght

				/* Return void pointer to the raw data */
				return reinterpret_cast<void*> ( reinterpret_cast <Header *> (it) + (1U));
			}
		}

		/* updates values */
		this->m_sentinel = it;
		it = it->m_next;
	}

	/* if get's on this far, it's probably something wrong */
	throw std::bad_alloc();
}


void SLPool::Free( void * f_block ){

	f_block = reinterpret_cast<Block *> (reinterpret_cast <Header *> (f_block) - (1U));
	
	Block * free_p = m_sentinel->m_next;		// Pointer to next free area
	Block * orig_p = (Block *) f_block;			// Pointer to block that will be removed
	Block * pref_free_p = m_sentinel;			// slow pointer from free_p	
	
	/* iterates on the free areas, stops if finds the last free area */
	while(free_p != nullptr)
	{
		/* set's the current free area as the first one after the orig_p */
		if(free_p > orig_p)	break;		

		/* updates values */
		pref_free_p = free_p;
		free_p = free_p->m_next;
	}


    /* The area is directly between two free areas */
	/* ...[free][occp][free]... */
    if (pref_free_p + pref_free_p->m_length == orig_p
        and orig_p + orig_p->m_length == free_p)
    {
        pref_free_p->m_length += free_p->m_length + orig_p->m_length;
        pref_free_p->m_next = free_p->m_next;
    }
    /* The area is not directly between any other free areas */
	/* ...[free]...[occp]...[free]... */
    else if (pref_free_p + pref_free_p->m_length != orig_p
            and orig_p + orig_p->m_length != free_p)
    {
        pref_free_p->m_next = orig_p;
        orig_p->m_next = free_p;
    }
    /* There is a free area directly before and no other */
	/* ...[free][occp]...[free]... */
    else if (pref_free_p + pref_free_p->m_length == orig_p
            and orig_p + orig_p->m_length != free_p)
    {
        pref_free_p->m_length += orig_p->m_length;
        pref_free_p->m_next = free_p;
    }
    /* The area is a free area directly after and no other */
	/* ...[free]...[occp][free]... */
    else
    {
        orig_p->m_length += free_p->m_length;
        pref_free_p->m_next = orig_p;
        orig_p->m_next = free_p->m_next;
    }
	
}

void SLPool::print( void ){
	/* gets sentinel->m_next to start printing the free areas */	
	Block * it = this->m_sentinel->m_next;
	if(false){
		std::cout << "it->m_length = " << it->m_length << std::endl;
		std::cout << "m_pool->m_length = " << m_pool->m_length << std::endl;
		std::cout << "m_n_blocks = " << m_n_blocks << std::endl;
	}

	/* aliases to make life easier */
	std::string occp_b = "\e[1;35m[ used ]\e[0m ";
	std::string free_b = "\e[2m[ free ]\e[0m ";

	/* sum of the free members, useful for getting occupied percentage */
	long int sum = 0;

	/* print free areas */
	while( it != nullptr )
	{
		sum += it->m_length;
		for( int i = 0; i < it->m_length; i++ ){
			std::cout << free_b;
		}
		it = it->m_next;
	}

	/* print occupied areas */
	for( int i = 0; i < m_n_blocks - sum; i++ ){
		std::cout << occp_b;
	}
	std::cout << std::endl;

	/* print pool info */
	std::cout << "\n\e[1;36mPOOL info's:\e[0m\n"
			<< "\tUsed percentage: "
			<< (double)(m_n_blocks - sum) / m_n_blocks * 100 << "%\t|\t"
			<< "Block size = " << sizeof(Block) << " bytes"
			<< "\n\tCapacity of the Pool: "
			<< m_n_blocks * sizeof(Block) - sizeof(Header)
			<< " bytes\t|"
			<< "\tBlocks on the pool: " 
			<< this->m_n_blocks;

	for( int i = 0; i < 3; i++ ) std::cout << std::endl;
}

struct Tag { SLPool * pool; };
void * operator new( size_t bytes, SLPool & p ) /* throw (std::bad_alloc) */
{
	Tag * const m_tag = 
		reinterpret_cast<Tag *> (p.Allocate( bytes + sizeof(Tag) ));
	m_tag->pool = &p;
	return reinterpret_cast<void *>( m_tag + 1 );
}

void * operator new[] ( size_type bytes, SLPool & p ){
	size_type new_size = bytes + sizeof(Tag);
	Tag * const m_tag = reinterpret_cast<Tag *const>( p.Allocate(new_size) );
	m_tag->pool = &p;
	return reinterpret_cast<void *>( m_tag + 1 );
}


void * operator new( size_type bytes ) 
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
