#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

#include "skein.h"

#define ITERS_PER_HUNK 1000
#define MAX_MSG_LEN (ITERS_PER_HUNK + 1)
#define HASHLEN 128
#define THRESHOLD 411
#define SENDIT_CMD "../sendit.sh"

const u08b_t target[HASHLEN] = {0x5b, 0x4d, 0xa9, 0x5f, 0x5f, 0xa0, 0x82, 0x80, 0xfc, 0x98, 0x79, 0xdf, 0x44, 0xf4, 0x18, 0xc8, 0xf9, 0xf1, 0x2b, 0xa4, 0x24, 0xb7, 0x75, 0x7d, 0xe0, 0x2b, 0xbd, 0xfb, 0xae, 0x0d, 0x4c, 0x4f, 0xdf, 0x93, 0x17, 0xc8, 0x0c, 0xc5, 0xfe, 0x04, 0xc6, 0x42, 0x90, 0x73, 0x46, 0x6c, 0xf2, 0x97, 0x06, 0xb8, 0xc2, 0x59, 0x99, 0xdd, 0xd2, 0xf6, 0x54, 0x0d, 0x44, 0x75, 0xcc, 0x97, 0x7b, 0x87, 0xf4, 0x75, 0x7b, 0xe0, 0x23, 0xf1, 0x9b, 0x8f, 0x40, 0x35, 0xd7, 0x72, 0x28, 0x86, 0xb7, 0x88, 0x69, 0x82, 0x6d, 0xe9, 0x16, 0xa7, 0x9c, 0xf9, 0xc9, 0x4c, 0xc7, 0x9c, 0xd4, 0x34, 0x7d, 0x24, 0xb5, 0x67, 0xaa, 0x3e, 0x23, 0x90, 0xa5, 0x73, 0xa3, 0x73, 0xa4, 0x8a, 0x5e, 0x67, 0x66, 0x40, 0xc7, 0x9c, 0xc7, 0x01, 0x97, 0xe1, 0xc5, 0xe7, 0xf9, 0x02, 0xfb, 0x53, 0xca, 0x18, 0x58, 0xb6};

/**
 * @buf - [in] msg input
 * @outbuf - [out] hash output
 */
static void get_hash(u08b_t *inbuf, u08b_t *outbuf)
{
        Skein1024_Ctxt_t ctx;

        Skein1024_Init(&ctx, 1024);
        Skein1024_Update(&ctx, inbuf, strlen((char *)inbuf));
        Skein1024_Final(&ctx, outbuf);
}

static void print_hash_hex_digest(const u08b_t *hash, size_t hashlen)
{
        int i;
        for (i = 0; i < hashlen; ++i)
                printf("%02x", hash[i]);
        putchar('\n');
}

static const char * const candidate_chars =
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ";
static const int candidate_chars_len = 97 + 1; /* 1 for NULL */

static const char get_random_char()
{
        return candidate_chars[rand() % candidate_chars_len];
}

static int get_bit_diff(const u08b_t * const Y, const u08b_t * const Z,
                        size_t len)
{
        int i, nbits = 0;
        u08b_t X;
        for (i = 0; i < len; ++i) {
                X = Y[i] ^ Z[i];
                for (; X ;) {
                        X &= X - 1;
                        nbits += 1;
                }
        }
        return nbits;
}

/**
 * shellquote - Escape all ' chars and wrap the whole thing in ' chars
 */
static void shellquote(const u08b_t * const inbuf, u08b_t *outbuf)
{
        const u08b_t *iter;
        *outbuf++ = '\'';
        for (iter = inbuf; *iter; ) {
                if (*iter == '\'') {
                        *outbuf++ = '\'';
                        *outbuf++ = '\\';
                        *outbuf++ = '\'';
                }
                *outbuf++ = *iter++;
        }
        *outbuf++ = '\'';
        *outbuf = '\0';
}

static void sendit(const u08b_t * const buf)
{
        char cmd[2000];
        u08b_t msg[MAX_MSG_LEN];
        shellquote(buf, &msg[0]);
        sprintf(cmd, "%s %s", SENDIT_CMD, msg);
        puts("will run:");
        puts(cmd);
        system(cmd);
}

int main(int argc, char *argv[])
{
        int nattempts = 0, n, best = 1025, threshold = THRESHOLD;
        u08b_t inbuf[MAX_MSG_LEN], hashbuf[HASHLEN];

        u08b_t *iter;

        srand(time(NULL));

        for (;;) {
                iter = &inbuf[0];
                *iter++ = get_random_char();
                *iter = '\0';
                for (n = 0; n < ITERS_PER_HUNK; ++n) {
                        int nbits;
                        get_hash(inbuf, hashbuf);

                        nbits = get_bit_diff(hashbuf, target, HASHLEN);
                        if (nbits < best) {
                                best = nbits;
                                iter = &inbuf[0];

                                if (nbits < threshold) {
                                        puts("\n\nNew best hash found!");
                                        puts("---BEGIN STRING:---");
                                        puts((char *)inbuf);
                                        puts("---END STRING---");
                                        printf("(should be off by %d)\n", nbits);
                                        puts("hex digest of computed hash:");
                                        print_hash_hex_digest(hashbuf, HASHLEN);
                                        threshold = nbits;
                                        puts("Now attempting to upload to the xkcd server...");
                                        sendit(inbuf);
                                        puts("Done!");
                                }
                        }

                        *iter++ = get_random_char();
                        *iter = '\0';
                        nattempts++;
                }
                printf("\r%d hashes compared. Best so far is %d       ",
                       nattempts, best);
        }

        return 0;
}
