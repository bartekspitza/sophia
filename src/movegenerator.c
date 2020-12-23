#include <stdlib.h>
#include <stdio.h>
#include "movegenerator.h"
#include "utils.h"

#define bit(a) (1LL<<a)
#define _3(a,b,c)           (1LL<<a)|(1LL<<b)|(1LL<<c)
#define _4(a,b,c,d)         (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)
#define _5(a,b,c,d,e)       (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)
#define _6(a,b,c,d,e,f)     (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)|(1LL<<f)
#define _7(a,b,c,d,e,f,g)   (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)|(1LL<<f)|(1LL<<g)
#define _8(a,b,c,d,e,f,g,h) (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)|(1LL<<f)|(1LL<<g)|(1LL<<h)
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? (bitboard ^= (1ULL << square)) : 0)
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))


Bitboard pawnStartWhite = 0xFF00;
Bitboard pawnStartBlack = 0x00FF000000000000;

Bitboard PAWN_W_ATTACKS_EAST[64] = {
    0,       0,       0,       0,       0,       0,       0,       0,
    0, bit(H3), bit(G3), bit(F3), bit(E3), bit(D3), bit(C3), bit(B3),
    0, bit(H4), bit(G4), bit(F4), bit(E4), bit(D4), bit(C4), bit(B4),
    0, bit(H5), bit(G5), bit(F5), bit(E5), bit(D5), bit(C5), bit(B5),
    0, bit(H6), bit(G6), bit(F6), bit(E6), bit(D6), bit(C6), bit(B6),
    0, bit(H7), bit(G7), bit(F7), bit(E7), bit(D7), bit(C7), bit(B7),
    0, bit(H8), bit(G8), bit(F8), bit(E8), bit(D8), bit(C8), bit(B8),
    0,       0,       0,       0,       0,       0,       0,       0,
};
Bitboard PAWN_W_ATTACKS_WEST[64] = {
    0,    0,       0,       0,       0,       0,       0,       0,
    bit(G3), bit(F3), bit(E3), bit(D3), bit(C3), bit(B3), bit(A3), 0,
    bit(G4), bit(F4), bit(E4), bit(D4), bit(C4), bit(B4), bit(A4), 0,
    bit(G5), bit(F5), bit(E5), bit(D5), bit(C5), bit(B5), bit(A5), 0,
    bit(G6), bit(F6), bit(E6), bit(D6), bit(C6), bit(B6), bit(A6), 0,
    bit(G7), bit(F7), bit(E7), bit(D7), bit(C7), bit(B7), bit(A7), 0,
    bit(G8), bit(F8), bit(E8), bit(D8), bit(C8), bit(B8), bit(A8), 0,
    0,    0,       0,       0,       0,       0,       0,       0,
};
Bitboard PAWN_B_ATTACKS_EAST[64] = {
    0,    0,       0,       0,       0,       0,       0,       0,
    bit(G1), bit(F1), bit(E1), bit(D1), bit(C1), bit(B1), bit(A1), 0,
    bit(G2), bit(F2), bit(E2), bit(D2), bit(C2), bit(B2), bit(A2), 0,
    bit(G3), bit(F3), bit(E3), bit(D3), bit(C3), bit(B3), bit(A3), 0,
    bit(G4), bit(F4), bit(E4), bit(D4), bit(C4), bit(B4), bit(A4), 0,
    bit(G5), bit(F5), bit(E5), bit(D5), bit(C5), bit(B5), bit(A5), 0,
    bit(G6), bit(F6), bit(E6), bit(D6), bit(C6), bit(B6), bit(A6), 0,
    0,    0,       0,       0,       0,       0,       0,        0,
};
Bitboard PAWN_B_ATTACKS_WEST[64] = {
    0,       0,       0,       0,       0,       0,       0,       0,
    0, bit(H1), bit(G1), bit(F1), bit(E1), bit(D1), bit(C1), bit(B1),
    0, bit(H2), bit(G2), bit(F2), bit(E2), bit(D2), bit(C2), bit(B2),
    0, bit(H3), bit(G3), bit(F3), bit(E3), bit(D3), bit(C3), bit(B3),
    0, bit(H4), bit(G4), bit(F4), bit(E4), bit(D4), bit(C4), bit(B4),
    0, bit(H5), bit(G5), bit(F5), bit(E5), bit(D5), bit(C5), bit(B5),
    0, bit(H6), bit(G6), bit(F6), bit(E6), bit(D6), bit(C6), bit(B6),
    0,       0,       0,       0,       0,       0,       0,       0,
};
Bitboard KING_MOVEMENT[64] = {
_3(G1,G2,H2)      ,_5(F1,F2,G2,H2,H1)         ,_5(E1,E2,F2,G2,G1)         ,_5(D1,D2,E2,F2,F1)         ,_5(C1,C2,D2,E2,E1)         ,_5(B1,B2,C2,D2,D1)         ,_5(A1,A2,B2,C2,C1)         ,_3(B1,B2,A2),
_5(H1,G1,G2,G3,H3),_8(F1,F2,F3,G1,G3,H1,H2,H3),_8(E1,E2,E3,F1,F3,G1,G2,G3),_8(D1,D2,D3,E1,E3,F1,F2,F3),_8(C1,C2,C3,D1,D3,E1,E2,E3),_8(B1,B2,B3,C1,C3,D1,D2,D3),_8(A1,A2,A3,B1,B3,C1,C2,C3),_5(A1,B1,B2,B3,A3),
_5(H2,G2,G3,G4,H4),_8(F2,F3,F4,G2,G4,H2,H3,H4),_8(E2,E3,E4,F2,F4,G2,G3,G4),_8(D2,D3,D4,E2,E4,F2,F3,F4),_8(C2,C3,C4,D2,D4,E2,E3,E4),_8(B2,B3,B4,C2,C4,D2,D3,D4),_8(A2,A3,A4,B2,B4,C2,C3,C4),_5(A2,B2,B3,B4,A4),
_5(H3,G3,G4,G5,H5),_8(F3,F4,F5,G3,G5,H3,H4,H5),_8(E3,E4,E5,F3,F5,G3,G4,G5),_8(D3,D4,D5,E3,E5,F3,F4,F5),_8(C3,C4,C5,D3,D5,E3,E4,E5),_8(B3,B4,B5,C3,C5,D3,D4,D5),_8(A3,A4,A5,B3,B5,C3,C4,C5),_5(A3,B3,B4,B5,A5),
_5(H4,G4,G5,G6,H6),_8(F4,F5,F6,G4,G6,H4,H5,H6),_8(E4,E5,E6,F4,F6,G4,G5,G6),_8(D4,D5,D6,E4,E6,F4,F5,F6),_8(C4,C5,C6,D4,D6,E4,E5,E6),_8(B4,B5,B6,C4,C6,D4,D5,D6),_8(A4,A5,A6,B4,B6,C4,C5,C6),_5(A4,B4,B5,B6,A6),
_5(H5,G5,G6,G7,H7),_8(F5,F6,F7,G5,G7,H5,H6,H7),_8(E5,E6,E7,F5,F7,G5,G6,G7),_8(D5,D6,D7,E5,E7,F5,F6,F7),_8(C5,C6,C7,D5,D7,E5,E6,E7),_8(B5,B6,B7,C5,C7,D5,D6,D7),_8(A5,A6,A7,B5,B7,C5,C6,C7),_5(A5,B5,B6,B7,A7),
_5(H6,G6,G7,G8,H8),_8(F6,F7,F8,G6,G8,H6,H7,H8),_8(E6,E7,E8,F6,F8,G6,G7,G8),_8(D6,D7,D8,E6,E8,F6,F7,F8),_8(C6,C7,C8,D6,D8,E6,E7,E8),_8(B6,B7,B8,C6,C8,D6,D7,D8),_8(A6,A7,A8,B6,B8,C6,C7,C8),_5(A6,B6,B7,B8,A8),
_3(G8,G7,H7)      ,_5(F8,F7,G7,H8,H7)         ,_5(E8,E7,F7,G8,G7)         ,_5(D8,D7,E7,F8,F7)         ,_5(C8,C7,D7,E8,E7)         ,_5(B8,B7,C7,D8,D7)         ,_5(A8,A7,B7,C8,C7)         ,_3(B8,B7,A7)
};

int ROOK_RELEVANT_BITS[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

// bishop rellevant occupancy bits
int BISHOP_RELEVANT_BITS[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

// Movement masks
Bitboard BISHOP_MOVEMENT[64];
Bitboard ROOK_MOVEMENT[64];
// Allt the different attack masks combinations (decided by blockers) for each square
Bitboard BISHOP_ATTACKS[64][512];
Bitboard ROOK_ATTACKS[64][4096];

const Bitboard ROOK_MAGICS[64] = {
    0xa8002c000108020ULL,
    0x6c00049b0002001ULL,
    0x100200010090040ULL,
    0x2480041000800801ULL,
    0x280028004000800ULL,
    0x900410008040022ULL,
    0x280020001001080ULL,
    0x2880002041000080ULL,
    0xa000800080400034ULL,
    0x4808020004000ULL,
    0x2290802004801000ULL,
    0x411000d00100020ULL,
    0x402800800040080ULL,
    0xb000401004208ULL,
    0x2409000100040200ULL,
    0x1002100004082ULL,
    0x22878001e24000ULL,
    0x1090810021004010ULL,
    0x801030040200012ULL,
    0x500808008001000ULL,
    0xa08018014000880ULL,
    0x8000808004000200ULL,
    0x201008080010200ULL,
    0x801020000441091ULL,
    0x800080204005ULL,
    0x1040200040100048ULL,
    0x120200402082ULL,
    0xd14880480100080ULL,
    0x12040280080080ULL,
    0x100040080020080ULL,
    0x9020010080800200ULL,
    0x813241200148449ULL,
    0x491604001800080ULL,
    0x100401000402001ULL,
    0x4820010021001040ULL,
    0x400402202000812ULL,
    0x209009005000802ULL,
    0x810800601800400ULL,
    0x4301083214000150ULL,
    0x204026458e001401ULL,
    0x40204000808000ULL,
    0x8001008040010020ULL,
    0x8410820820420010ULL,
    0x1003001000090020ULL,
    0x804040008008080ULL,
    0x12000810020004ULL,
    0x1000100200040208ULL,
    0x430000a044020001ULL,
    0x280009023410300ULL,
    0xe0100040002240ULL,
    0x200100401700ULL,
    0x2244100408008080ULL,
    0x8000400801980ULL,
    0x2000810040200ULL,
    0x8010100228810400ULL,
    0x2000009044210200ULL,
    0x4080008040102101ULL,
    0x40002080411d01ULL,
    0x2005524060000901ULL,
    0x502001008400422ULL,
    0x489a000810200402ULL,
    0x1004400080a13ULL,
    0x4000011008020084ULL,
    0x26002114058042ULL,
};

// bishop magic number
const Bitboard BISHOP_MAGICS[64] = {
    0x89a1121896040240ULL,
    0x2004844802002010ULL,
    0x2068080051921000ULL,
    0x62880a0220200808ULL,
    0x4042004000000ULL,
    0x100822020200011ULL,
    0xc00444222012000aULL,
    0x28808801216001ULL,
    0x400492088408100ULL,
    0x201c401040c0084ULL,
    0x840800910a0010ULL,
    0x82080240060ULL,
    0x2000840504006000ULL,
    0x30010c4108405004ULL,
    0x1008005410080802ULL,
    0x8144042209100900ULL,
    0x208081020014400ULL,
    0x4800201208ca00ULL,
    0xf18140408012008ULL,
    0x1004002802102001ULL,
    0x841000820080811ULL,
    0x40200200a42008ULL,
    0x800054042000ULL,
    0x88010400410c9000ULL,
    0x520040470104290ULL,
    0x1004040051500081ULL,
    0x2002081833080021ULL,
    0x400c00c010142ULL,
    0x941408200c002000ULL,
    0x658810000806011ULL,
    0x188071040440a00ULL,
    0x4800404002011c00ULL,
    0x104442040404200ULL,
    0x511080202091021ULL,
    0x4022401120400ULL,
    0x80c0040400080120ULL,
    0x8040010040820802ULL,
    0x480810700020090ULL,
    0x102008e00040242ULL,
    0x809005202050100ULL,
    0x8002024220104080ULL,
    0x431008804142000ULL,
    0x19001802081400ULL,
    0x200014208040080ULL,
    0x3308082008200100ULL,
    0x41010500040c020ULL,
    0x4012020c04210308ULL,
    0x208220a202004080ULL,
    0x111040120082000ULL,
    0x6803040141280a00ULL,
    0x2101004202410000ULL,
    0x8200000041108022ULL,
    0x21082088000ULL,
    0x2410204010040ULL,
    0x40100400809000ULL,
    0x822088220820214ULL,
    0x40808090012004ULL,
    0x910224040218c9ULL,
    0x402814422015008ULL,
    0x90014004842410ULL,
    0x1000042304105ULL,
    0x10008830412a00ULL,
    0x2520081090008908ULL,
    0x40102000a0a60140ULL,
};

int count_bits(Bitboard bitboard) {
  // bit count
  int count = 0;
  
  // pop bits untill bitboard is empty
  while (bitboard)
  {
      // increment count
      count++;
      
      // consecutively reset least significant 1st bit
      bitboard &= bitboard - 1;
  }
  
  // return bit count
  return count;
}

// get index of LS1B in bitboard
int get_ls1b_index(Bitboard bitboard) {
    // make sure bitboard is not empty
    if (bitboard != 0)
        // convert trailing zeros before LS1B to ones and count them
        return count_bits((bitboard & -bitboard) - 1);
    
    // otherwise
    else
        // return illegal index
        return -1;
}

// set occupancies
Bitboard set_occupancy(int index, int bits_in_mask, Bitboard attack_mask)
{
    // occupancy map
    Bitboard occupancy = 0ULL;
    
    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++)
    {
        // get LS1B index of attacks mask
        int square = get_ls1b_index(attack_mask);
        
        // pop LS1B in attack map
        pop_bit(attack_mask, square);
        
        // make sure occupancy is on board
        if (index & (1 << count))
            // populate occupancy map
            occupancy |= (1ULL << square);
    }
    
    // return occupancy map
    return occupancy;
}

// mask bishop attacks
Bitboard mask_bishop_attacks(int square)
{
    // attack bitboard
    Bitboard attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));
    
    // return attack map for bishop on a given square
    return attacks;
}

// mask rook attacks
Bitboard mask_rook_attacks(int square)
{
    // attacks bitboard
    Bitboard attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));
    
    // return attack map for bishop on a given square
    return attacks;
}

// bishop attacks
Bitboard bishop_attacks_on_the_fly(int square, Bitboard block)
{
    // attack bitboard
    Bitboard attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    // return attack map for bishop on a given square
    return attacks;
}

// rook attacks
Bitboard rook_attacks_on_the_fly(int square, Bitboard block)
{
    // attacks bitboard
    Bitboard attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf))) break;
    }
    
    for (r = tr - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf))) break;
    }
    
    for (f = tf + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f))) break;
    }
    
    for (f = tf - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f))) break;
    }
    
    // return attack map for bishop on a given square
    return attacks;
}

void initMoveGenerationTables(void) {
    for (int square = 0; square < 64; square++) {

        // Fill movement masks
        BISHOP_MOVEMENT[square] = mask_bishop_attacks(square);
        ROOK_MOVEMENT[square] = mask_rook_attacks(square);
        
        Bitboard bishopMask = BISHOP_MOVEMENT[square];
        Bitboard rookMask = ROOK_MOVEMENT[square];
        int bishopRelevantBits = BISHOP_RELEVANT_BITS[square];
        int rookRelevantBits = ROOK_RELEVANT_BITS[square];
        int bishopOccupancyVariations = 1 << bishopRelevantBits;
        int rookOccupancyVariations = 1 << rookRelevantBits;

        for (int i = 0; i < bishopOccupancyVariations; i++) {
            Bitboard occupancy = set_occupancy(i, bishopRelevantBits, bishopMask);
            Bitboard magic_index = occupancy * BISHOP_MAGICS[square] >> 64 - bishopRelevantBits;
            BISHOP_ATTACKS[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);                
        }

        for (int i = 0; i < rookOccupancyVariations; i++) {
            Bitboard occupancy = set_occupancy(i, rookRelevantBits, rookMask);
            Bitboard magic_index = occupancy * ROOK_MAGICS[square] >> 64 - rookRelevantBits;
            ROOK_ATTACKS[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);                
        }
    }
}

Bitboard getBishopAttacks(int square, Bitboard occupancy) {
	occupancy &= BISHOP_MOVEMENT[square];
	occupancy *=  BISHOP_MAGICS[square];
	occupancy >>= 64 - BISHOP_RELEVANT_BITS[square];
	return BISHOP_ATTACKS[square][occupancy];
}

Bitboard getRookAttacks(int square, Bitboard occupancy) {
	occupancy &= ROOK_MOVEMENT[square];
	occupancy *=  ROOK_MAGICS[square];
	occupancy >>= 64 - ROOK_RELEVANT_BITS[square];
	return ROOK_ATTACKS[square][occupancy];
}

Move* legalMoves(Board board, int* length) {
    Move* first = malloc(sizeof(Move));
    Move* move = first;

    Bitboard occupancyWhite = 0;
    Bitboard occupancyBlack = 0;
    for (int i = 0; i < 6;i++) {
        occupancyWhite |= *(&(board.pawn_W)+i);
        occupancyBlack |= *(&(board.pawn_B)+i);
    }
    Bitboard occupancy = occupancyBlack | occupancyWhite;

    if (board.turn) {
        // Pawn pushes
        Bitboard singlePush = board.pawn_W << 8;
        singlePush ^= singlePush & occupancy;

        Bitboard doublePush = (board.pawn_W & pawnStartWhite) << 8*2;
        doublePush ^= doublePush & occupancy;
        doublePush = doublePush >> 8;
        doublePush &= singlePush;
        doublePush = doublePush << 8;

        for (int i = 0; i < 64;i++) {
            if (getBit(singlePush, i)) {
                move->fromSquare = i-8;
                move->toSquare = i;
                move++;
                (*length)++;
            }
            if (getBit(doublePush, i)) {
                move->fromSquare = i-8*2;
                move->toSquare = i;
                move++;
                (*length)++;
            }
        }

        // Pawn attacks
        for (int i = 0; i < 64;i++) {
            if (getBit(board.pawn_W, i)) {
                if (PAWN_W_ATTACKS_EAST[i] & occupancyBlack) {
                    move->fromSquare = i;
                    move->toSquare = i+7;
                    move++;
                    (*length)++;
                }
                
                if (PAWN_W_ATTACKS_WEST[i] & occupancyBlack) {
                    move->fromSquare = i;
                    move->toSquare = i+9;
                    move++;
                    (*length)++;
                }
            }
        }

        // King
        int kingSquare;
        for (int i = 0; i < 64;i++) {
            if (getBit(board.king_W, i)) {
                kingSquare = i;
                break;
            }
        }
        Bitboard kingMoves = KING_MOVEMENT[kingSquare];
        kingMoves ^= kingMoves & occupancyWhite;
        for (int i = 0; i < 64;i++) {
            if (getBit(kingMoves, i)) {
                move->fromSquare = kingSquare;
                move->toSquare = i;
                move++;
                (*length)++;
            }
        }

        // Bishop
        for (int i = 0; i < 64; i++) {
            if (getBit(board.bishop_W, i)) {
                Bitboard bishopAttacks = getBishopAttacks(i, occupancy);
                bishopAttacks ^= bishopAttacks & occupancyWhite;
                for (int j = 0; j < 64; j++) {
                    if (getBit(bishopAttacks, j)) {
                        move->fromSquare = i;
                        move->toSquare = j;
                        move++;
                        (*length)++;
                    }
                }
            }
        }
        // Rook
        for (int i = 0; i < 64; i++) {
            if (getBit(board.rook_W, i)) {
                Bitboard rookAttacks = getRookAttacks(i, occupancy);
                rookAttacks ^= rookAttacks & occupancyWhite;
                for (int j = 0; j < 64; j++) {
                    if (getBit(rookAttacks, j)) {
                        move->fromSquare = i;
                        move->toSquare = j;
                        move++;
                        (*length)++;
                    }
                }
            }
        }

        // Queen
        for (int i = 0; i < 64; i++) {
            if (getBit(board.queen_W, i)) {
                Bitboard rookAttacks = getRookAttacks(i, occupancy);
                rookAttacks ^= rookAttacks & occupancyWhite;

                Bitboard bishopAttacks = getBishopAttacks(i, occupancy);
                bishopAttacks ^= bishopAttacks & occupancyWhite;
                Bitboard queenAttacks = bishopAttacks | rookAttacks;

                for (int j = 0; j < 64; j++) {
                    if (getBit(queenAttacks, j)) {
                        move->fromSquare = i;
                        move->toSquare = j;
                        move++;
                        (*length)++;
                    }
                }
            }
        }
    } else {
        // Pawn pushes
        Bitboard singlePush = board.pawn_B >> 8;
        singlePush ^= singlePush & occupancy;

        Bitboard doublePush = (board.pawn_B & pawnStartBlack) >> 8*2;
        doublePush ^= doublePush & occupancy;
        doublePush = doublePush << 8;
        doublePush &= singlePush;
        doublePush = doublePush >> 8;

        for (int i = 0; i < 64;i++) {
            if (getBit(singlePush, i)) {
                move->fromSquare = i+8;
                move->toSquare = i;
                move++;
                (*length)++;
            }
            if (getBit(doublePush, i)) {
                move->fromSquare = i+8*2;
                move->toSquare = i;
                move++;
                (*length)++;
            }
        }

        // Pawn attacks
        for (int i = 0; i < 64;i++) {
            if (getBit(board.pawn_B, i)) {
                if (PAWN_B_ATTACKS_EAST[i] & occupancyWhite) {
                    move->fromSquare = i;
                    move->toSquare = i-7;
                    move++;
                    (*length)++;
                }
                
                if (PAWN_B_ATTACKS_WEST[i] & occupancyWhite) {
                    move->fromSquare = i;
                    move->toSquare = i-9;
                    move++;
                    (*length)++;
                }
            }
        }

        // King
        int kingSquare;
        for (int i = 0; i < 64;i++) {
            if (getBit(board.king_B, i)) {
                kingSquare = i;
                break;
            }
        }
        Bitboard kingMoves = KING_MOVEMENT[kingSquare];
        kingMoves ^= kingMoves & occupancyBlack;
        for (int i = 0; i < 64;i++) {
            if (getBit(kingMoves, i)) {
                move->fromSquare = kingSquare;
                move->toSquare = i;
                move++;
                (*length)++;
            }
        }

        // Bishop
        for (int i = 0; i < 64; i++) {
            if (getBit(board.bishop_B, i)) {
                Bitboard bishopAttacks = getBishopAttacks(i, occupancy);
                bishopAttacks ^= bishopAttacks & occupancyBlack;
                for (int j = 0; j < 64; j++) {
                    if (getBit(bishopAttacks, j)) {
                        move->fromSquare = i;
                        move->toSquare = j;
                        move++;
                        (*length)++;
                    }
                }
            }
        }
        // Rook
        for (int i = 0; i < 64; i++) {
            if (getBit(board.rook_B, i)) {
                Bitboard rookAttacks = getRookAttacks(i, occupancy);
                rookAttacks ^= rookAttacks & occupancyBlack;
                for (int j = 0; j < 64; j++) {
                    if (getBit(rookAttacks, j)) {
                        move->fromSquare = i;
                        move->toSquare = j;
                        move++;
                        (*length)++;
                    }
                }
            }
        }

        // Queen
        for (int i = 0; i < 64; i++) {
            if (getBit(board.queen_B, i)) {
                Bitboard rookAttacks = getRookAttacks(i, occupancy);
                rookAttacks ^= rookAttacks & occupancyBlack;

                Bitboard bishopAttacks = getBishopAttacks(i, occupancy);
                bishopAttacks ^= bishopAttacks & occupancyBlack;
                Bitboard queenAttacks = bishopAttacks | rookAttacks;

                for (int j = 0; j < 64; j++) {
                    if (getBit(queenAttacks, j)) {
                        move->fromSquare = i;
                        move->toSquare = j;
                        move++;
                        (*length)++;
                    }
                }
            }
        }
    }

    return first;
}

void sanToMove(Move* move, char* san) {
    int fromFile = 'h' - san[0];
    int fromRank = atoi(&san[1]) - 1;
    int toFile = 'h' - san[2];
    int toRank = atoi(&san[3]) - 1;
    move->fromSquare = 8 * (fromRank) + fromFile;
    move->toSquare = 8 * (toRank) + toFile;
}

void moveToSan(Move move, char* san) {
    san[0] = SQUARE_NAMES[move.fromSquare][0];
    san[1] = SQUARE_NAMES[move.fromSquare][1];
    san[2] = SQUARE_NAMES[move.toSquare][0];
    san[3] = SQUARE_NAMES[move.toSquare][1];
}