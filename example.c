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
      deal_uid_t dealid = ww_deal(KEY, 0, session, i);
      char pbn_deal[58], deal_uuid[25] = {0};
      ww_deal_to_uuid(dealid, deal_uuid);
      ww_deal_to_pbn(dealid, pbn_deal, i+1);
      printf("%02d %s %s\n", i+1, deal_uuid, pbn_deal);
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

