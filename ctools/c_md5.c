/**
 @file c_md5.c
 @brief Implementation des MD5-Message-Digest-Algorithmus.
 @details

 Der Algorithmus stammt von Ron Rivest. Der urspruengliche
 Sourcecode wurde von Colin Plumb im Jahr 1993 geschrieben,
 es wird kein Urheberrecht beansprucht.

 Entsprechender Code ist von RSA Data Security, Inc. erhältlich.
 Dieser Code wurde dagegen getestet und ist gleichwertig,
 außer dass hier nicht zwei Seiten Rechtssprache hinzugefügt
 wurde.

 -----

 fuer die zu berechnenden MD5 wird immer der ctx, eine
 MD5Context-Struktur uebergeben. Sie wird durch MD5Init
 initialisiert. MD5Update fuellt hier die passenden Felder.
 Ein MD5Final stellt dann das 16-Byte-Array mit dem MD5 parat.
 Um einen String (charfeld von 33 Byte) zu erhalten, kann
 MF5FinalString benutzt werden.

___[ Revision ]______________________________________________________________

 @date 09.07.16 HS Fuer HPSum implementiert
 @date 13.09.22 HS Ueberarbeitet fuer aktuellen Einsatz

___________________________________________________________________________*/

#include <memory.h>                     // fuer memcpy()
#include "c_md5.h"
void *memcpy_ex(void *dst, const char *src, size_t len);// in tools.c

#ifndef HIGHFIRST
#define byteReverse(buf, len)           // dummy Macro
#else
void byteReverse(unsigned char *buf, unsigned longs)
{
    md5_uint32 t;
    do {
	t = (md5_uint32) ((unsigned) buf[3] << 8 | buf[2]) << 16 |
	    ((unsigned) buf[1] << 8 | buf[0]);
	*(md5_uint32 *) buf = t;
	buf += 4;
    } while (--longs);
}
#endif
/**
@brief Funktion ist Private
@param dest   wohin
@param val    was soll geschrieben werden
@param len    wie oft schreiben
@return void*

weil der c-compiler meckert

___[ Revision ]______________________________________________________________

  @date 13.09.22 HS GCC 11.2 meckert bei memset

___________________________________________________________________________*/

void *md5_memset(void *dest, register int val, register size_t len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}

/**
@brief Start MD5 accumulation
@param ctx  Context

Setze die Bitanzahl auf 0 und den Puffer auf mysterioese Initialisierungskonstanten.

___[ Revision ]______________________________________________________________

@date 13.09.22 HS Nicht weiter bearbeitbar

___________________________________________________________________________*/

void MD5Init(struct MD5Context *ctx)
{
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
}
/**
@brief Update context
@param ctx  Context
@param buf  welchen buffer berechnen
@param len  buffer laenge

Aktualisieren des Context, um die Verkettung eines weiteren Buffers widerzuspiegeln

___[ Revision ]______________________________________________________________

@date 13.09.22 HS Komplett ueberarbeitet. Am Algorithmus nichts geaendert

___________________________________________________________________________*/

void MD5Update(struct MD5Context *ctx, unsigned char *buf, unsigned len)
{
    md5_uint32 t;

    t = ctx->bits[0];                                       // Update bitcount
    if ((ctx->bits[0] = t + ((md5_uint32) len << 3)) < t)
    ctx->bits[1]++;                                         // Carry von low nach high
    ctx->bits[1] += len >> 29;
    t = (t >> 3) & 0x3f;                                    // Bytes already in shsInfo->data
    if (t)                                                  // Handle alle fuehrenden odd-sized chunks
    {
        unsigned char *p = (unsigned char *) ctx->in + t;

/*
              void * __cdecl memcpy(void * __restrict__ _Dst,                         const void * __restrict__ _Src,size_t _Size) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _SECIMP errno_t __cdecl memcpy_s (             void *_dest,size_t _numberOfElements,const void *_src,              size_t _count);
       memcpy_ex

  void *memcpy_ex(void *dst, const char *src, size_t len)
*/
        t = 64 - t;
        if (len < t)
        {
            //memcpy_s(p, len, buf, len); =>13.11.22 HS
            memcpy_ex(p, (char*)buf, len);
            return;
        }
        // memcpy_s(p, t, buf, t);
        memcpy_ex(p, (char*)buf, t);
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (md5_uint32 *) ctx->in);
        buf += t;
        len -= t;
    }
    while (len >= 64)                                       // Process data in 64-byte chunks
    {
        //memcpy_s(ctx->in, 64, buf, 64);
        memcpy_ex(ctx->in, (char*)buf, 64);
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (md5_uint32 *) ctx->in);
        buf += 64;
        len -= 64;
    }
    memcpy_ex(ctx->in, (char*)buf, len);                       // Handle alle remaining bytes of data
}


/**
@brief Finalisiere den context
@param digest Zielbuffer
@param ctx  Context

Abschließende Zusammenfassung
Auffüllen bis zur 64-Byte-Grenze mit dem Bitmuster 1 0 * (64-Bit-count der verarbeiteten Bits, MSB-first)

___[ Revision ]______________________________________________________________

@date 13.09.22 HS Komplett ueberarbeitet. Am Algorithmus nichts geaendert

___________________________________________________________________________*/

void MD5Final(unsigned char digest[16], struct MD5Context *ctx)
{
    unsigned count;
    unsigned char *p;


    count = (ctx->bits[0] >> 3) & 0x3F;                     // Berechnen die Anzahl der Bytes mod 64
    p = ctx->in + count;                                    // Setze die erste Zeichen auf 0x80. Es ist immer mindestens ein Byte frei
    *p++ = 0x80;
    count = 64 - 1 - count;                                 // Sicherstellen das es immer 64 bytes Bytes sind

    if (count < 8)                                          // Auffuellen auf 56 mod 64
    {
        md5_memset(p, 0, count);                            // Zwei Fuellmengen: Erster Block 64 Bytes
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (md5_uint32 *) ctx->in);
        md5_memset(ctx->in, 0, 56);                         // naechsten Block mit 56 Bytes
    } else {
        md5_memset(p, 0, count - 8);                        // Auffuellen auf 56 bytes
    }
    byteReverse(ctx->in, 14);

    ctx->in32[14] = ctx->bits[0];                           // Länge in Bits anhaengen und transformieren
	ctx->in32[15] = ctx->bits[1];

    MD5Transform(ctx->buf, (md5_uint32 *) ctx->in);
    byteReverse((unsigned char *) ctx->buf, 4);
    memcpy_ex(digest, (char*)ctx->buf, 16);
    md5_memset(ctx, 0, sizeof(struct MD5Context));          // case sensitive
}

// Die vier Kernfunktionen - F1 ist etwas optimiert    #define F1(x, y, z) (x & y | ~x & z)
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

// Dies ist der zentrale STEP im MD5-Algorithmus.
#define MD5STEP(f, w, x, y, z, data, s)  ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*  */

/**
@brief Kern des MD5-Algorithmus
@param buf
@param in

Der Kern des MD5-Algorithmus, dieser veraendert einen bestehenden MD5-Hash
spiegeln die Addition von 16 longwords neuer Daten wider. MD5Update-Bloecke
die Daten und konvertiert Bytes in longwords fuer diese Routine.

___[ Revision ]______________________________________________________________

@date 13.09.22 HS Am Algorithmus nichts geaendert

___________________________________________________________________________*/

void MD5Transform(md5_uint32 buf[4], md5_uint32 in[16])
{
    register md5_uint32 a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}


void MD5FinalString(char *Src, char *dString)
{
    int p,i, v;
    char hexs[17] = "0123456789abcdef";
    dString[32]=0;
    for (i = 15; i >= 0; i--)
    {
        p = i * 2;
        v = (unsigned char)Src[i];
        dString[p + 1] = hexs[v % 16];
        dString[p + 0] = hexs[v / 16];
    }
}
