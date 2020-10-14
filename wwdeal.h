/*
 * MIT License
 *
 * Copyright (c) 2020
 * Gerald Tan <woefulwabbit at gmail dot com>
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

/**
 * Bitmask of the cards held by a player
 * Bit 51 = SA, Bit 50 = SK, ...., Bit 0 = C2
 */
typedef uint64_t hand_t;

typedef union
{
   struct { hand_t N, E, S, W; };
   hand_t H[4];
} ww_deal_t;

/**
 * 96 bit deal unique id, using Richard Pavlicek's Mapping Bridge Deals algorithm
 * http://www.rpbridge.net/7z68.htm
 */
typedef __int128 deal_uid_t;

extern const deal_uid_t WW_DEAL_UID_MAX;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Use Richard Pavlicek's Mapping Bridge Deals algorithm to map a 96 bit deal uid
 * to our internal ww_deal_t format
 *
 * @param deal_uid   Pavlicek's deal uid
 * @param deal       Pointer to deal_t struct
 */
void ww_deal_map_pavlicek(deal_uid_t deal_uid, ww_deal_t *deal);

/**
 * Generates the deal for one board
 *
 * @param deal          Pointer to deal_t struct
 * @param key           256 bit (32 bytes) seed/key
 * @param event_num     Event number
 * @param session_num   Session number
 * @param board_num     Board number
 *
 * @return The deal uid
 */
deal_uid_t ww_deal(const uint8_t key[32], uint64_t event_num, uint32_t session_num, uint32_t board_num);

/**
 * Convert the deal uid to PBN format string
 *
 * @param deal_uid   96 bit deal_uid
 * @param pbn_deal   String buffer to write to (must be at least 58 bytes)
 * @param board_num  Board number (because PBN deal string requires the dealer)
 */
void ww_deal_to_pbn(deal_uid_t deal_uid, char pbn_deal[58], unsigned board_num);

/**
 * Convert the deal uid to Pavlicek uuid hex string
 *
 * @param deal_uid   96 bit deal_uid
 * @param deal_uuid  String buffer to write to (at least 25 bytes)
 */
void ww_deal_to_uuid(deal_uid_t deal_uid, char deal_uuid[25]);

#ifdef __cplusplus
}
#endif
