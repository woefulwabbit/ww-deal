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

#include <string.h>

#include "wwdeal.h"

const deal_uid_t WW_DEAL_UID_MAX = ((deal_uid_t)0xAD55E315 << 64) + 0x634DDA658BF49200;

/**
 * Richard Pavlicek's Mapping Bridge Deals algorithm
 * http://www.rpbridge.net/7z68.htm
 */
void ww_deal_map_pavlicek(deal_uid_t I, ww_deal_t *deal)
{
   if (!deal) return;

   memset(deal, 0, sizeof(ww_deal_t));
   if (I >= WW_DEAL_UID_MAX) return;

   int N = 13, E = 13, S = 13, W = 13;
   deal_uid_t K = WW_DEAL_UID_MAX;

   for (int C = 52; C > 0; C--) {
      deal_uid_t X = K * N / C;
      if (I < X) {
         // Deal card to North
         deal->N += 1ULL << (C-1);
         N--;
      }
      else {
         I = I - X;
         X = K * E / C;
         if (I < X) {
            // Deal card to East
            deal->E += 1ULL << (C-1);
            E--;
         }
         else {
            I = I - X;
            X = K * S / C;
            if (I < X) {
               // Deal card to South
               deal->S += 1ULL << (C-1);
               S--;
            }
            else {
               // Deal card to West
               I = I - X;
               X = K * W / C;
               deal->W += 1ULL << (C-1);
               W--;
            }
         }
      }
      K = X;
   }
}

