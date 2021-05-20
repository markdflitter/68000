#ifndef BSP_H
#define BSP_H

extern "C" void __init ();

extern "C" void __putch (char c);
extern "C" char __getch ();

extern "C" void __set_interrupt_vector (unsigned int vector);
extern "C" void __enable_interrupts ();
extern "C" void __disable_interrupts ();

#endif
