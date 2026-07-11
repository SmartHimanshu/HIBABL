#ifndef HIBABL_TYPES_H
#define HIBABL_TYPES_H

typedef unsigned char u8;
typedef signed char i8;

typedef unsigned short u16;
typedef signed short i16;

typedef unsigned int u32;
typedef signed int i32;

typedef unsigned long long u64;
typedef signed long long i64;

typedef u64 size_t;

#define NULL ((void*)0)

#define HIBABL_PACKED __attribute__((packed)) ;

#endif