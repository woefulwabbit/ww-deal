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
#include <stdint.h>

#include "wwdeal.h"

#if defined(__clang__) || defined(__GNUC__)
#define _hydro_attr_(X) __attribute__(X)
#else
#define _hydro_attr_(X)
#endif

#if defined(__INTEL_COMPILER) || defined(_MSC_VER)
#define _hydro_attr_aligned_(X) __declspec(align(X))
#elif defined(__clang__) || defined(__GNUC__)
#define _hydro_attr_aligned_(X) _hydro_attr_((aligned(X)))
#else
#define _hydro_attr_aligned_(X)
#endif

#define ROTL32(x, b) (uint32_t)(((x) << (b)) | ((x) >> (32 - (b))))
#define gimli_BLOCKBYTES (384/8)
#include "gimli-core/portable.h"
//#include "gimli-core/sse2.h"

deal_uid_t ww_deal(const uint8_t key[32], uint64_t event_num, uint32_t session_num, uint32_t board_num)
{
   /* Fill in the Gimli state
    * 0x0000  256 bit key
    * 0x0100  64 bit event number
    * 0x0140  32 bit session number
    * 0x0160  32 bit board number
    */
   uint32_t state[12];
   memcpy(state, key, 32);
   state[8] = event_num >> 32;
   state[9] = event_num & 0xFFFF;
   state[10] = session_num;
   state[11] = board_num;
   deal_uid_t dealid = 0;

   do {
      gimli_core(state);
      memcpy(&dealid, state, 12); // Read 96 bits deal uid
   } while (dealid >= WW_DEAL_UID_MAX); // Re-permutate if overflow

   return dealid;
}

