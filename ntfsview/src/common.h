#ifndef COMMON_H
#define COMMON_H

typedef unsigned char  uchar;
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned long  ulong;
typedef long long      llong; // 8 байт
typedef unsigned long long  ullong;

#define PACKED __attribute__((packed))

void init (int argc, char *argv[]);
int findarg (const char *arg);

char *decode_ustring (int length, uchar *buf);

#endif

