# ww-deal Contract Bridge Tournament Deal Generator

ww-deal is a Contract Bridge Tournament Deal Generator. Like [Hans van Staveren's Big Deal](https://sater.home.xs4all.nl/), this deal generator satisfies the following requirements that are necessary for a tournament deal generator:

1. The software should be able to generate every possible bridge deal, since that is also possible with manual dealing
2. The software should generate every deal with the same probability, without being influenced by the board number, previous hands or any other circumstance
3. It should be impossible to predict deals, even after having seen all other deals in the session

# Usage

The ww-deal library provides one main function to use:

```c
deal_uid_t ww_deal(const uint8_t key[32], uint64_t event_num, uint32_t session_num, uint32_t board_num);
```

The function accepts a 256 bit seed/key, an event number, a session number, and a board number.
The function then deals one board, and returns a 96 bit number representing the deal's unique id

Utility functions are provided to convert the deal uid:

- ``ww_deal_map_pavlicek`` converts the deal uid to a ww_deal_t structure
- ``ww_deal_to_pbn`` converts the deal uid to a PBN compatible string
- ``ww_deal_to_uuid`` converts the deal uid to a hex string representing a Richard Pavlicek's deal uuid http://www.rpbridge.net/7z68.htm 

# How ww-deal works

Being a modern reimplementation of Big Deal, ww-deal works in similar ways

1. The 256 bit seed/key, event number, session number and board number is fed into a CSPRNG, which returns a 96 bit value representing a particular deal's unique id
2. The deal unique id is fed into a mapping algorithm to map the deal into 4 hands of 13 cards

## Generating a seed

A function to generate a 256 bit seed is not provided, but this can easily be achieved by reading 32 bytes from /dev/urandom on a Unix platform or CryptGenRandom() on Windows. Using python, os.urandom() should be used.

Big deal provides a method to obtain a 160 bit seed by obtaining entropy from user keyboard or mouse interactions. This is typically not necessary now with modern operating systems providing methods to obtain crytographically suitable entropy.

## The Crytographically Secure Psuedo Random Number Generator (CSPRNG)

ww-deal uses the Gimli permutation function instead of Big Deal's RIPEMD-160 hash function as the CSPRNG.

### Gimli Permutation Function

[Gimli](https://gimli.cr.yp.to/) is a 384-bit permutation function that has advanced to the [2nd round](https://csrc.nist.gov/projects/lightweight-cryptography/round-2-candidates) of the [NIST Lightweight Cryptography](https://csrc.nist.gov/Projects/lightweight-cryptography) standardization process.

The Gimli permutation function is used by loading 384 bit state with the 256 bit seed/key, followed by 64 bit event number, 32 bit session number and 32 bit board number. The Gimli permutation is then performed and the first 96 bits is extracted from the resultant state as the deal unique id. If this id exceeds 0xAD55E315634DDA658BF49200 (total number of possible deals) then the permutation function is called again until the unique id falls within the range.

### Big Deal and RIPEMD-160

Big Deal document acknowledges that its use of RIPEMD-160 hash introduces an issue of the Birthday Paradox, where after 2^80 seeds are generated there is a large risk (>50%) chance of 2 different seeds generating the same series of deals. Gimli as a permutation function does not suffer from the Birthday Paradox, therefore all 2^256 possible seeds generate a different series of deals.

## Mapping bridge deals

ww-deal uses [Richard Pavlicek's method of mapping bridge deals](http://www.rpbridge.net/7z68.htm). This was chosen over Big Deal's use of [Thomas Andrew's](http://bridge.thomasoandrews.com/bridge/impossible/) not because of any one's technical merits over another, but simply because Pavlicek's algorithm was very well explained and simple to implement whereas Thomas Andrew's method existed only as source code.

## Multiprecision Library

ww-deal uses gcc/clang builtin support for 128 bit integers using standard mathematical operators (+-*/ and comparators), so it does not depend on a multiprecision library. Internally, gcc/clang uses 64 bit operations to provide the support for the 128 bit integers.

Big Deal uses its own "mp96" library for 96 bit integer operations, operating on 8 bits at a time.

# Comparison against Redeal and Thomas Andrews's Deal

Redeal and Thomas Andrew's Deal do not satisfy the 3 requirements necessary for a tournament deal generator, however they are very fast and suitable for statistical analysis, especially when used with Smartstacking which is not supported by ww-deal. Theoretically it would be possible to modify Redeal/Deal to use ww-deal as the backend deal generator, sacrificing speed for better theoretical qualify of generated deals.

