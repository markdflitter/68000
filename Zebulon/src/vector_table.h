class vector_table
{
public:
	vector_table (unsigned char* base_address);
	~vector_table ();

	typedef void (*function_pointer) (void);
	void set_vector (unsigned int vector, function_pointer function);
private:
	struct __attribute__((__packed__)) table
	{
		function_pointer vectors [255];
	};

	table* _table;
};
