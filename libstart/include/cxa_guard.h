typedef int cxa_guard;

extern "C" int __cxa_guard_acquire (cxa_guard* g);
extern "C" void __cxa_guard_release (cxa_guard* g);
