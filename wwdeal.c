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

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "wwdeal.h"

#define ROTL32(x, b) (uint32_t)(((x) << (b)) | ((x) >> (32 - (b))))
#define gimli_BLOCKBYTES (384/8)
#include "gimli-core/portable.h"

const deal_uid_t WW_DEAL_UID_MAX = ((deal_uid_t)0xAD55E315 << 64) + 0x634DDA658BF49200;


/**
 * Richard Pavlicek's Mapping Bridge Deals algorithm
 * http://www.rpbridge.net/7z68.htm
 */
void ww_deal_map(deal_uid_t I, deal_t *deal)
{
   if (!deal) return;

   memset(deal, 0, sizeof(deal_t));
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

deal_uid_t ww_deal(deal_t *deal, const uint8_t key[32], uint64_t event_num, uint32_t session_num, uint32_t board_num)
{
   /*
    * Fill in the Gimli state
    *
    * 0x0000  256 bit key
    * 0x0100  64 bit event number
    * 0x0140  32 bit session number
    * 0x0160  32 bit board number
    *
    * deal uid is read from first 96 bits of output state
    */
   uint32_t S[12];
   memcpy(S, key, 32);
   S[8] = event_num >> 32;
   S[9] = event_num & 0xFFFF;
   S[10] = session_num;
   S[11] = board_num;
   deal_uid_t dealid = 0;

   do {
      gimli_core(S);
      memcpy(&dealid, S, 12); // Read 96 bits deal uid
   } while (dealid >= WW_DEAL_UID_MAX); // Re-permutate if overflow

   if (deal) ww_deal_map(dealid, deal);

   return dealid;
}

