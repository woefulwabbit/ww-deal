/*
 * Copyright (c) 2020  Gerald Tan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "wwdeal.h"

static char denomination[] = "AKQJT98765432";
#define SUIT_MASK 0b1111111111111

void print_deal(deal_t *deal)
{
   for (int i = 0; i < 4; i++) {
      hand_t H = deal->H[i];
      for (int j = 0; j < 4; j++) {
         int shift = 13 * (3 - j);
         uint16_t S = (H & ((uint64_t)SUIT_MASK << shift)) >> shift;
         while (S) {
            int c = __builtin_clz(S);
            printf("%c", denomination[c-19]);
            S ^= 1 << (31-c);
         }
         if (j < 3) printf(".");
      }
      printf(" ");
   }
   printf("\n");
}


int main(int argc, char **argv)
{
   uint8_t KEY[32];
   unsigned session = 0, boards = 32;

   FILE *f = fopen("wwdeal.state", "rb");

   if (f) {
      // File found, read key and session number
      fread(KEY, 1, 32, f);
      fread(&session, 4, 1, f);
   }
   else {
      // Obtain a key from /dev/urandom
      f = fopen("/dev/urandom", "rb");
      fread(KEY, 1, 32, f);
   }
   fclose(f);

   if (argc > 1) {
      boards = strtoul(argv[1], NULL, 0);
   }

   printf("Generating %d boards for session number %d...\n", boards, session);

   uint64_t tic = clock();
   for (int i = 0; i < boards; i++) {
      deal_t deal;
      deal_uid_t dealid = ww_deal(&deal, KEY, 0, session, i);
      printf("%02d %08lx%016lx ", i+1, (uint64_t)(dealid >> 64), (uint64_t)dealid);
      print_deal(&deal);
   }
   uint64_t toc = clock();
   double elapsed = (toc - tic)/(double)CLOCKS_PER_SEC;

   printf("%fs (%f deals/s)\n", elapsed, boards / elapsed);

   // Save the key and session number
   session++;
   f = fopen("wwdeal.state", "wb");
   fwrite(KEY, 1, 32, f);
   fwrite(&session, 4, 1, f);
   fclose(f);
}

