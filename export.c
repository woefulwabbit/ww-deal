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
#include "wwdeal.h"

static char denomination[] = "AKQJT98765432";
static char dealer[] = "WNES";
#define SUIT_MASK 0b1111111111111

void ww_deal_to_pbn(deal_uid_t deal_id, char pbn_deal[58], unsigned board_num)
{
   ww_deal_t deal;
   ww_deal_map_pavlicek(deal_id, &deal);

   char *p = pbn_deal;
   *p++ = dealer[board_num % 4];
   *p++ = ':';

   for (int i = board_num; i < board_num + 4; i++) {
      hand_t H = deal.H[(i - 1) % 4];
      for (int j = 0; j < 4; j++) {
         int shift = 13 * (3 - j);
         uint16_t S = (H & ((uint64_t)SUIT_MASK << shift)) >> shift;
         while (S) {
            int c = __builtin_clz(S);
            *p++ = denomination[c-19];
            S ^= 1 << (31-c);
         }
         if (j < 3) *p++ = '.';
      }
      *p++ = ' ';
   }
   *p = 0;
}

void ww_deal_to_uuid(deal_uid_t deal_uid, char deal_uuid[25])
{
   snprintf(deal_uuid, 25, "%08lx%016lx", (uint64_t)(deal_uid >> 64), (uint64_t)deal_uid);
}
