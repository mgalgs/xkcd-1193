#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include"SHA3api_ref.h"

#define HASH_BIT 1024 
#define SENDIT_CMD "../sendit.sh"
#define SUBMIT_THRESHOLD 425

/* the origianal hash */
const u64b_t MATCH_INPUT[] =
    {
    0x5b4da95f5fa08280,
    0xfc9879df44f418c8,
    0xf9f12ba424b7757d,
    0xe02bbdfbae0d4c4f,
    0xdf9317c80cc5fe04,
    0xc6429073466cf297,
    0x06b8c25999ddd2f6,
    0x540d4475cc977b87,
    0xf4757be023f19b8f,
    0x4035d7722886b788,
    0x69826de916a79cf9,
    0xc94cc79cd4347d24,
    0xb567aa3e2390a573,
    0xa373a48a5e676640,
    0xc79cc70197e1c5e7,
    0xf902fb53ca1858b6
    };

/* I used this function once to generate the "bit sequence" hash below
 * to help bit counting */
u64b_t swap_u64b_t(u64b_t val) {
	val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
	val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
	return (val << 32) | (val >> 32);
}

/* The hash in "bit sequence". or more like "byte sequence". */
const u64b_t MATCH_INPUT_BIT_SEQUENCE[] =
	{
	0x8082a05f5fa94d5b,
	0xc818f444df7998fc,
	0x7d75b724a42bf1f9,
	0x4f4c0daefbbd2be0,
	0x04fec50cc81793df,
	0x97f26c46739042c6,
	0xf6d2dd9959c2b806,
	0x877b97cc75440d54,
	0x8f9bf123e07b75f4,
	0x88b7862872d73540,
	0xf99ca716e96d8269,
	0x247d34d49cc74cc9,
	0x73a590233eaa67b5,
	0x4066675e8aa473a3,
	0xe7c5e19701c79cc7,
	0xb65818ca53fb02f9
	};

/* Global varibles */
static int infLoop = 1;
static size_t globalCount = 0;

void my_handler(int s) {
  if (s == SIGINT) {
    infLoop = 0;
    printf("%lu strings tried\n", globalCount);
  }
}


/* A pretty naive version of random string generator
 * Maybe a faster random generator can be helpful here.
 * A GPU version? */
#define MIN_STR_LENGTH 4
#define MAX_STR_LENGTH 10
char *rand_str(char *dst, int size)
{
   static const char text[] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
			      "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ;':\"[]{}-=_+\\/)";
   int i, len = rand() % (size - 1 - MIN_STR_LENGTH) + MIN_STR_LENGTH;
   for ( i = 0; i < len; ++i )
   {
      dst[i] = text[rand() % (sizeof text - 1)];
   }
   dst[i] = '\0';
   return dst;
}

/* Count the off bits compared to the
 * 'MATCH_INPUT'.
 * returns the off bit counts
 * Assume HASH_BIT==1024 */
const u64b_t m1 = 0x5555555555555555;
const u64b_t m2 = 0x3333333333333333;
const u64b_t m4 = 0x0f0f0f0f0f0f0f0f;
const u64b_t h01 = 0x0101010101010101;
size_t off_bit_count_1024(u64b_t * hash) {
	size_t offcount = 0;
	for (int i = 0; i < 16; i++) {
		u64b_t x = hash[i]^MATCH_INPUT_BIT_SEQUENCE[i];
		x -= (x >> 1) & m1;
		x = (x & m2) + ((x >> 2) & m2);
		x = (x + (x >> 4)) & m4;
		offcount += (x * h01)>>56;
	}
	return offcount;
}

/**
 * shellquote - Escape all ' chars and wrap the whole thing in ' chars
 */
static void shellquote(const char * const inbuf, char *outbuf)
{
        const char * iter;
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

static void sendit(char *buf)
{
        static char cmd[2000];
        static char msg[MAX_STR_LENGTH];
        shellquote(buf, &msg[0]);
        snprintf(cmd, 2000, "%s %s", SENDIT_CMD, msg);
        puts("will run:");
        puts(cmd);
        system(cmd);
}

int main(int argc, const char **argv)
{
  /* set up the signal handler */
  signal (SIGINT,my_handler);

    char best[MAX_STR_LENGTH];
    char word[MAX_STR_LENGTH];
    size_t best_off = 1024;

    srand(time(0));

    while (infLoop) {
	rand_str(word, sizeof(word));
	
	/* Call Hash */
	HashReturn success;
	BitSequence hash[HASH_BIT>>3];
	memset(hash, 0, sizeof(hash)*sizeof(BitSequence));
	Hash(HASH_BIT, (BitSequence *) word, strlen(word)<<3, hash);

	/* Print results */
	size_t current = off_bit_count_1024((u64b_t *) hash);
	globalCount++;
	if (best_off > current) {
		strcpy(best, word);
		best_off = current;
                if (best_off < SUBMIT_THRESHOLD)
                        putchar('\n');
		printf("%s : %lu\n", best, best_off);
                if (best_off < SUBMIT_THRESHOLD)
                        sendit(best);
	}
    }

	return 0;
}

