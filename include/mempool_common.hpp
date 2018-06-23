#ifndef _MEMPOOL_COMMON_H_
#define _MEMPOOL_COMMON_H_

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
		}

		struct Pool
		{
			status m_stat;
			void * m_mem;
		}

		struct Tag
		{
			int m_id;		// stub
			size_type m_size;
		}

		struct Block
		{
			size_type m_lenght;
			void * m_data;
		}

		/* class members */
		Pool * m_pool;

	public:
		SLPPool( size_type );
		~SLPPool( void );
};

#endif
