#include <size_t.h>

class SpaceManager
{
public:
	SpaceManager ();	
	SpaceManager (size_t size);	
	~SpaceManager ();	

	struct Chunk {
		unsigned int m_start;
		size_t m_length;
	};

	Chunk* allocate (size_t size);

	void print () const;
private:
	void format (size_t size);

	void load ();
	void save () const;
	
	Chunk* m_free;
};
