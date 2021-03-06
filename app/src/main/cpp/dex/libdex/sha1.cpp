/*
 * Tweaked in various ways for Google/Android:
 *  - Changed from .cpp to .c.
 *  - Made argument to SHA1Update a const pointer, and enabled
 *    SHA1HANDSOFF.  This incurs a speed penalty but prevents us from
 *    trashing the input.
 *  - Include <endian.h> to get endian info.
 *  - Split a small piece into a header file.
 */

/*
sha1sum: inspired by md5sum.

SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain

-----------------
Modified 7/98
By James H. Brown <jbrown@burgoyne.com>
Still 100% Public Domain

bit machines
Routine SHA1Update changed from
    void SHA1Update(SHA1_CTX* context, unsigned char* data,
      unsigned int len)
to
    void SHA1Update(SHA1_CTX* context, unsigned char* data,
      unsigned long len)

The 'len' parameter was declared an int which works fine on 32
bit machines. However, on 16 bit machines an int is too small
for the shifts being done against it.  This caused the hash
function to generate incorrect values if len was greater than
8191 (8K - 1) due to the 'len << 3' on line 3 of SHA1Update().

Since the file IO in main() reads 16K at a time, any file 8K or
larger would be guaranteed to generate the wrong hash (e.g.
Test Vector #3, a million "a"s).

I also changed the declaration of variables i & j in SHA1Update
to unsigned long from unsigned int for the same reason.

These changes should make no difference to any 32 bit
implementations since an int and a long are the same size in
those environments.

--
I also corrected a few compiler warnings generated by Borland
C.
1. Added #include <process.h> for exit() prototype
2. Removed unused variable 'j' in SHA1Final
3. Changed exit(0) to return(0) at end of main.

ALL changes I made can be located by searching for comments
containing 'JHB'

-----------------
Modified 13 August 2000
By Michael Paul Johnson <mpj@cryptography.org>
Still 100% Public Domain

Changed command line syntax, added feature to automatically
check files against their previous SHA-1 check values, kind of
like md5sum does. Added functions hexval, verifyfile,
and sha1file. Rewrote main().
-----------------

Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

#define SHA1HANDSOFF    /*Copies data before messing with it.*/

/*#define CMDLINE        * include main() and file processing */

#include "sha1.h"

#include <stdio.h>
#include <string.h>

#ifdef __BORLANDC__
#include <dir.h>
#include <dos.h>
#include <process.h>   /*  prototype for exit() - JHB
needed for Win32, but chokes Linux - MPJ */
#define X_LITTLE_ENDIAN /* This should be #define'd if true.*/
#else

# include <unistd.h>
# include <stdlib.h>
//# include <endian.h>
//# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define X_LITTLE_ENDIAN
//# endif
#endif

#include <ctype.h>

#define LINESIZE 2048

static void SHA1Transform(unsigned long state[5],
                          const unsigned char buffer[64]);

#define rol(value, bits) \
 (((value)<<(bits))|((value)>>(32-(bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from
   SSLeay */
#ifdef X_LITTLE_ENDIAN
#define blk0(i) (block->l[i]=(rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
#else
#define blk0(i) block->l[i]
#endif
#define blk(i) (block->l[(i)&15] = rol(block->l[((i)+13)&15]^block->l[((i)+8)&15] \
    ^block->l[((i)+2)&15]^block->l[(i)&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v, w, x, y, z, i) z+=(((w)&((x)^(y)))^(y))+blk0(i)+0x5A827999+rol(v,5);(w)=rol(w,30);
#define R1(v, w, x, y, z, i) z+=(((w)&((x)^(y)))^(y))+blk(i)+0x5A827999+rol(v,5);(w)=rol(w,30);
#define R2(v, w, x, y, z, i) z+=((w)^(x)^(y))+blk(i)+0x6ED9EBA1+rol(v,5);(w)=rol(w,30);
#define R3(v, w, x, y, z, i) z+=((((w)|(x))&(y))|((w)&(x)))+blk(i)+0x8F1BBCDC+rol(v,5);(w)=rol(w,30);
#define R4(v, w, x, y, z, i) z+=((w)^(x)^(y))+blk(i)+0xCA62C1D6+rol(v,5);(w)=rol(w,30);


/* Hash a single 512-bit block. This is the core of the algorithm. */

static void SHA1Transform(unsigned long state[5],
                          const unsigned char buffer[64]) {
    unsigned long a, b, c, d, e;
    union CHAR64LONG16 {
        unsigned char c[64];
        unsigned long l[16];
    };
    CHAR64LONG16 *block;
#ifdef SHA1HANDSOFF
    static unsigned char workspace[64];
    block = (CHAR64LONG16 *) workspace;
    memcpy(block, buffer, 64);
#else
    block = (CHAR64LONG16*)buffer;
#endif
    /* Copy context->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a, b, c, d, e, 0);
    R0(e, a, b, c, d, 1);
    R0(d, e, a, b, c, 2);
    R0(c, d, e, a, b, 3);
    R0(b, c, d, e, a, 4);
    R0(a, b, c, d, e, 5);
    R0(e, a, b, c, d, 6);
    R0(d, e, a, b, c, 7);
    R0(c, d, e, a, b, 8);
    R0(b, c, d, e, a, 9);
    R0(a, b, c, d, e, 10);
    R0(e, a, b, c, d, 11);
    R0(d, e, a, b, c, 12);
    R0(c, d, e, a, b, 13);
    R0(b, c, d, e, a, 14);
    R0(a, b, c, d, e, 15);
    R1(e, a, b, c, d, 16);
    R1(d, e, a, b, c, 17);
    R1(c, d, e, a, b, 18);
    R1(b, c, d, e, a, 19);
    R2(a, b, c, d, e, 20);
    R2(e, a, b, c, d, 21);
    R2(d, e, a, b, c, 22);
    R2(c, d, e, a, b, 23);
    R2(b, c, d, e, a, 24);
    R2(a, b, c, d, e, 25);
    R2(e, a, b, c, d, 26);
    R2(d, e, a, b, c, 27);
    R2(c, d, e, a, b, 28);
    R2(b, c, d, e, a, 29);
    R2(a, b, c, d, e, 30);
    R2(e, a, b, c, d, 31);
    R2(d, e, a, b, c, 32);
    R2(c, d, e, a, b, 33);
    R2(b, c, d, e, a, 34);
    R2(a, b, c, d, e, 35);
    R2(e, a, b, c, d, 36);
    R2(d, e, a, b, c, 37);
    R2(c, d, e, a, b, 38);
    R2(b, c, d, e, a, 39);
    R3(a, b, c, d, e, 40);
    R3(e, a, b, c, d, 41);
    R3(d, e, a, b, c, 42);
    R3(c, d, e, a, b, 43);
    R3(b, c, d, e, a, 44);
    R3(a, b, c, d, e, 45);
    R3(e, a, b, c, d, 46);
    R3(d, e, a, b, c, 47);
    R3(c, d, e, a, b, 48);
    R3(b, c, d, e, a, 49);
    R3(a, b, c, d, e, 50);
    R3(e, a, b, c, d, 51);
    R3(d, e, a, b, c, 52);
    R3(c, d, e, a, b, 53);
    R3(b, c, d, e, a, 54);
    R3(a, b, c, d, e, 55);
    R3(e, a, b, c, d, 56);
    R3(d, e, a, b, c, 57);
    R3(c, d, e, a, b, 58);
    R3(b, c, d, e, a, 59);
    R4(a, b, c, d, e, 60);
    R4(e, a, b, c, d, 61);
    R4(d, e, a, b, c, 62);
    R4(c, d, e, a, b, 63);
    R4(b, c, d, e, a, 64);
    R4(a, b, c, d, e, 65);
    R4(e, a, b, c, d, 66);
    R4(d, e, a, b, c, 67);
    R4(c, d, e, a, b, 68);
    R4(b, c, d, e, a, 69);
    R4(a, b, c, d, e, 70);
    R4(e, a, b, c, d, 71);
    R4(d, e, a, b, c, 72);
    R4(c, d, e, a, b, 73);
    R4(b, c, d, e, a, 74);
    R4(a, b, c, d, e, 75);
    R4(e, a, b, c, d, 76);
    R4(d, e, a, b, c, 77);
    R4(c, d, e, a, b, 78);
    R4(b, c, d, e, a, 79);

    /* Add the working vars back into context.state[] */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    /* Wipe variables */
/*    a = b = c = d = e = 0; Nice try, but the compiler
optimizes this out, anyway, and it produces an annoying
warning. */
}


/* SHA1Init - Initialize new context */

void SHA1Init(SHA1_CTX *context) {
    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}


/* Run your data through this. */

void SHA1Update(SHA1_CTX *context, const unsigned char *data,
                unsigned long len)  /* JHB */
{
    unsigned long i, j; /* JHB */

    j = (context->count[0] >> 3) & 63;
    if ((context->count[0] += len << 3) < (len << 3))
        context->count[1]++;
    context->count[1] += (len >> 29);
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64 - j));
        SHA1Transform(context->state, context->buffer);
        for (; i + 63 < len; i += 64) {
            SHA1Transform(context->state, &data[i]);
        }
        j = 0;
    } else
        i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);
}


/* Add padding and return the message digest. */

void SHA1Final(unsigned char digest[HASHSIZE], SHA1_CTX *
context) {
    unsigned long i;    /* JHB */
    unsigned char finalcount[8];

    for (i = 0; i < 8; i++) {
        finalcount[i] = (unsigned char) ((context->count[(i >= 4 ?
                                                          0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
        /* Endian independent */
    }
    SHA1Update(context, (unsigned char *) "\200", 1);
    while ((context->count[0] & 504) != 448) {
        SHA1Update(context, (unsigned char *) "\0", 1);
    }
    SHA1Update(context, finalcount, 8);
    /* Should cause a SHA1Transform() */
    for (i = 0; i < HASHSIZE; i++) {
        digest[i] = (unsigned char)
                ((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
    }
    /* Wipe variables */
    memset(context->buffer, 0, 64);
    memset(context->state, 0, HASHSIZE);
    memset(context->count, 0, 8);
    memset(&finalcount, 0, 8);
#ifdef SHA1HANDSOFF
    /* make SHA1Transform overwrite it's own static vars */
    SHA1Transform(context->state, context->buffer);
#endif
}


#ifdef CMDLINE

/* sha1file computes the SHA-1 hash of the named file and puts
   it in the 20-byte array digest. If fname is NULL, stdin is
   assumed.
*/
void sha1file(char *fname, unsigned char* digest)
{
    int bytesread;
    SHA1_CTX context;
    unsigned char buffer[16384];
    FILE* f;

    if (fname)
    {
        f = fopen(fname, "rb");
        if (!f)
        {
            fprintf(stderr, "Can't open %s\n", fname);
            memset(digest, 0, HASHSIZE);
            return;
        }
    }
    else
    {
        f = stdin;
    }
    SHA1Init(&context);
    while (!feof(f))
    {
        bytesread = fread(buffer, 1, 16384, f);
        SHA1Update(&context, buffer, bytesread);
    }
    SHA1Final(digest, &context);
    if (fname)
        fclose(f);
}

/* Convert ASCII hexidecimal digit to 4-bit value. */
unsigned char hexval(char c)
{
    unsigned char h;

    c = toupper(c);
    if (c >= 'A')
        h = c - 'A' + 10;
    else
        h = c - '0';
    return h;
}

/* Verify a file created with sha1sum by redirecting output
   to a file. */
int verifyfile(char *fname)
{
    int j, k;
    int found = 0;
    unsigned char digest[HASHSIZE];
    unsigned char expected_digest[HASHSIZE];
    FILE *checkfile;
    char checkline[LINESIZE];
    char *s;
    unsigned char err;

    checkfile = fopen(fname, "rt");
    if (!checkfile)
    {
        fprintf(stderr, "Can't open %s\n", fname);
        return(0);
    }
    do
    {
        s = fgets(checkline, LINESIZE, checkfile);
        if (s)
        {
            if ((strlen(checkline)>26)&&
                1 /*(!strncmp(checkline,"SHA1=", 5))*/)
            {
                /* Overwrite newline. */
                checkline[strlen(checkline)-1]=0;
                found = 1;

                /* Read expected check value. */
                for (k=0, j=5; k < HASHSIZE; k++)
                {
                    expected_digest[k]=hexval(checkline[j++]);
                    expected_digest[k]=(expected_digest[k]<<4)
                        +hexval(checkline[j++]);
                }

                /* Compute fingerprints */
                s = checkline+46;
                sha1file(s, digest);

                /* Compare fingerprints */
                err = 0;
                for (k=0; k<HASHSIZE; k++)
                    err |= digest[k]-
                        expected_digest[k];
                if (err)
                {
                    fprintf(stderr, "FAILED: %s\n"
                        " EXPECTED: ", s);
                    for (k=0; k<HASHSIZE; k++)
                        fprintf(stderr, "%02X",
                            expected_digest[k]);
                    fprintf(stderr,"\n    FOUND: ");
                    for (k=0; k<HASHSIZE; k++)
                        fprintf(stderr, "%02X", digest[k]);
                    fprintf(stderr, "\n");
                }
                else
                {
                    printf("OK: %s\n", s);
                }
            }
        }
    } while (s);
    fclose(checkfile);
    return found;
}



void syntax(char *progname)
{
    printf("\nsyntax:\n"
     "%s [-c|-h][-q] file name[s]\n"
     "    -c = check files against previous check values\n"
     "    -g = generate SHA-1 check values (default action)\n"
     "    -h = display this help\n"
     "For example,\n"
     "sha1sum test.txt > check.txt\n"
     "generates check value for test.txt in check.txt, and\n"
     "sha1sum -c check.txt\n"
     "checks test.txt against the check value in check.txt\n",
     progname);
    exit(1);
}


/**********************************************************/

int main(int argc, char** argv)
{
    int i, j, k;
    int check = 0;
    int found = 0;
    unsigned char digest[HASHSIZE];
    unsigned char expected_digest[HASHSIZE];
    FILE *checkfile;
    char checkline[LINESIZE];
    char *s;
#ifdef __BORLANDC__
    struct ffblk f;
    int done;
    char path[MAXPATH];
    char drive[MAXDRIVE];
    char dir[MAXDIR];
    char name[MAXFILE];
    char ext[MAXEXT];
#endif
    unsigned char err;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
                case 'c':
                case 'C':
                    check = 1;
                    break;
                case 'g':
                case 'G':
                    check = 0;
                    break;
                default:
                    syntax(argv[0]);
            }
        }
    }

    for (i=1; i<argc; i++)
    {
        if (argv[i][0] != '-')
        {
#ifdef __BORLANDC__
            fnsplit(argv[i], drive, dir, name, ext);
            done = findfirst(argv[i], &f, FA_RDONLY |
                FA_HIDDEN|FA_SYSTEM|FA_ARCH);
             while (!done)
            {
                sprintf(path, "%s%s%s", drive, dir, f.ff_name);
                s = path;
#else
                s = argv[i];
#endif

                if (check)
                {   /* Check fingerprint file. */
                    found |= verifyfile(s);
                }
                else
                {   /* Generate fingerprints & write to
                       stdout. */
                    sha1file(s, digest);
                    //printf("SHA1=");
                    for (j=0; j<HASHSIZE; j++)
                        printf("%02x", digest[j]);
                    printf("  %s\n", s);
                    found = 1;
                }

#ifdef __BORLANDC__
                done = findnext(&f);
            }
#endif

        }
    }
    if (!found)
    {
        if (check)
        {
            fprintf(stderr,
                "No SHA1 lines found in %s\n",
                argv[i]);
        }
        else
        {
            fprintf(stderr, "No files checked.\n");
            syntax(argv[0]);
        }
    }
    return(0);  /* JHB */
}

#endif  /*CMDLINE*/
