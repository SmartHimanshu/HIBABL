#ifndef HIBABL_CORE_SYMBOL_H
#define HIBABL_CORE_SYMBOL_H

#define LOCAL(sym) L_ ## sym

#define FUNCTION(x) \
        .globl x;    \
        .type x, @function; \
x:

#define VARIABLE(x) \
        .globl x;   \
        .type x, @object; \
x:


#endif