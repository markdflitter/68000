class Shell {
public:
	Shell (unsigned int& tick);
	void run () const;
private:
	unsigned int& m_tick;
};
