#ifndef DISPLAY_H
#define DISPLAY_H

class Display
{
public:
	Display (unsigned int baseAddress = 0xE00001);

	void setNumber (unsigned int num);
private:
	unsigned char* m_display;
};

#endif
