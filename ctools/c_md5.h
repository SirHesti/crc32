#ifndef MD5_H
#define MD5_H
#ifdef  __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Nachfolgend die Begruendung im Orginal fuer HIGHFIRST fuer Retro-Rechner. Auf modernen CPU's wird es dazu nur
   PreProcessing kosten, da da ein NULL-Macro erzeugt wird.
   ---
   The following tests optimise behaviour on little-endian machines, where there is no need to reverse the byte order
   of 32 bit words in the MD5 computation.  By default, HIGHFIRST is defined, which indicates we're running on a
   big-endian (most significant byte first) machine, on which the byteReverse function in md5.c must be invoked. However,
   byteReverse is coded in such a way that it is an identity function when run on a little-endian machine, so calling it
   on such a platform causes no harm apart from wasting time.
   If the platform is known to be little-endian, we speed things up by undefining HIGHFIRST, which defines byteReverse
   as a null macro.  Doing things in this manner insures we work on new platforms regardless of their byte order.
*/
#define HIGHFIRST
#if defined(__i386__) || defined(__x86_64__) || defined(__amd64__)
#undef HIGHFIRST
#endif

// On machines where "long" is 64 bits, we need to declare md5int32 uint32_t as something guaranteed to be 32 bits.
typedef uint32_t md5_uint32;
struct MD5Context
{
        md5_uint32 buf[4];
        md5_uint32 bits[2];
    union
    {
        unsigned char in[64];
        md5_uint32 in32[16];
	};
};
typedef struct MD5Context MD5_CTX;

void MD5Init(struct MD5Context *ctx);
void MD5Update(struct MD5Context *ctx, unsigned char *buf, unsigned len);
void MD5Final(unsigned char digest[16], struct MD5Context *ctx);
void MD5Transform(md5_uint32 buf[4], md5_uint32 in[16]);
void MD5FinalString(char *Src, char *dString);

#ifdef  __cplusplus
}
#endif
#endif /* !MD5_H */
