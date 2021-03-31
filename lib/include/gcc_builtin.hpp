#ifndef GCC_BUILTIN_HPP_INCLUDED
#define GCC_BUILTIN_HPP_INCLUDED

// make a function incline/non-inline
#define FORCE_INLINE __attribute__((always_inline)) inline
#define FORCE_NOINLINE __attribute__((noinline))
#define mbarrier asm volatile("": : :"memory")

#endif
