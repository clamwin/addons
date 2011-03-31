#ifndef _PRTYPES_H
#define _PRTYPES_H

typedef unsigned __int32 PRUint32;
typedef __int32 PRInt32;
typedef PRInt32 PROffset32;

static __inline PRUint32 htonl(PRUint32 hostlong)
{
  return (hostlong >> 24) | ((hostlong & 0xff0000) >> 8) |
      ((hostlong & 0xff00) << 8) | (hostlong << 24);
}

#define ntohl(x) htonl(x)

#endif /* _PRTYPES_H */
