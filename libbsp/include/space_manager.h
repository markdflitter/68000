class space_manager
{
public:
	space_manager ();	
	space_manager (unsigned int size);	
	~space_manager ();	

	struct chunk {
		unsigned int _start;
		unsigned int _length;
	};

	chunk* allocate (unsigned int size);

	void print ();
private:
	void format (unsigned int size);

	void load ();
	void save ();
	
	chunk* _free;
};
