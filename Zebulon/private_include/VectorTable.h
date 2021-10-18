#ifndef VECTOR_TABLE_H
#define VECTOR_TABLE_H

class VectorTable
{
public:
	VectorTable (unsigned char* baseAddress);
	~VectorTable ();

	typedef void (*fnPtr) (void);
	void setVector (unsigned int vector, fnPtr fn);
private:
	struct __attribute__((__packed__)) Table
	{
		fnPtr m_vectors [255];
	};

	Table* m_table;
};

#endif
