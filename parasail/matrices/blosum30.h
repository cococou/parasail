/**
 * @file
 *
 * @author jeff.daily@pnnl.gov
 *
 * Copyright (c) 2015 Battelle Memorial Institute.
 *
 * This file was converted to C code from the raw file found at
 * ftp://ftp.cbi.pku.edu.cn/pub/software/blast/matrices/BLOSUM30, the
 * Center for Bioinformatics, Peking University, China.
 */
#ifndef _PARASAIL_BLOSUM30_H_
#define _PARASAIL_BLOSUM30_H_

#include "parasail.h"
#include "blosum_map.h"

#ifdef __cplusplus
extern "C" {
#endif

/* #  Matrix made by matblas from blosum30.iij */
/* #  * column uses minimum score */
/* #  BLOSUM Clustered Scoring Matrix in 1/5 Bit Units */
/* #  Blocks Database = /data/blocks_5.0/blocks.dat */
/* #  Cluster Percentage: >= 30 */
/* #  Entropy =   0.1424, Expected =  -0.1074 */

static const int parasail_blosum30_[] = {
/*        A   R   N   D   C   Q   E   G   H   I   L   K   M   F   P   S   T   W   Y   V   B   Z   X   * */
/* A */   4, -1,  0,  0, -3,  1,  0,  0, -2,  0, -1,  0,  1, -2, -1,  1,  1, -5, -4,  1,  0,  0,  0, -7,
/* R */  -1,  8, -2, -1, -2,  3, -1, -2, -1, -3, -2,  1,  0, -1, -1, -1, -3,  0,  0, -1, -2,  0, -1, -7,
/* N */   0, -2,  8,  1, -1, -1, -1,  0, -1,  0, -2,  0,  0, -1, -3,  0,  1, -7, -4, -2,  4, -1,  0, -7,
/* D */   0, -1,  1,  9, -3, -1,  1, -1, -2, -4, -1,  0, -3, -5, -1,  0, -1, -4, -1, -2,  5,  0, -1, -7,
/* C */  -3, -2, -1, -3, 17, -2,  1, -4, -5, -2,  0, -3, -2, -3, -3, -2, -2, -2, -6, -2, -2,  0, -2, -7,
/* Q */   1,  3, -1, -1, -2,  8,  2, -2,  0, -2, -2,  0, -1, -3,  0, -1,  0, -1, -1, -3, -1,  4,  0, -7,
/* E */   0, -1, -1,  1,  1,  2,  6, -2,  0, -3, -1,  2, -1, -4,  1,  0, -2, -1, -2, -3,  0,  5, -1, -7,
/* G */   0, -2,  0, -1, -4, -2, -2,  8, -3, -1, -2, -1, -2, -3, -1,  0, -2,  1, -3, -3,  0, -2, -1, -7,
/* H */  -2, -1, -1, -2, -5,  0,  0, -3, 14, -2, -1, -2,  2, -3,  1, -1, -2, -5,  0, -3, -2,  0, -1, -7,
/* I */   0, -3,  0, -4, -2, -2, -3, -1, -2,  6,  2, -2,  1,  0, -3, -1,  0, -3, -1,  4, -2, -3,  0, -7,
/* L */  -1, -2, -2, -1,  0, -2, -1, -2, -1,  2,  4, -2,  2,  2, -3, -2,  0, -2,  3,  1, -1, -1,  0, -7,
/* K */   0,  1,  0,  0, -3,  0,  2, -1, -2, -2, -2,  4,  2, -1,  1,  0, -1, -2, -1, -2,  0,  1,  0, -7,
/* M */   1,  0,  0, -3, -2, -1, -1, -2,  2,  1,  2,  2,  6, -2, -4, -2,  0, -3, -1,  0, -2, -1,  0, -7,
/* F */  -2, -1, -1, -5, -3, -3, -4, -3, -3,  0,  2, -1, -2, 10, -4, -1, -2,  1,  3,  1, -3, -4, -1, -7,
/* P */  -1, -1, -3, -1, -3,  0,  1, -1,  1, -3, -3,  1, -4, -4, 11, -1,  0, -3, -2, -4, -2,  0, -1, -7,
/* S */   1, -1,  0,  0, -2, -1,  0,  0, -1, -1, -2,  0, -2, -1, -1,  4,  2, -3, -2, -1,  0, -1,  0, -7,
/* T */   1, -3,  1, -1, -2,  0, -2, -2, -2,  0,  0, -1,  0, -2,  0,  2,  5, -5, -1,  1,  0, -1,  0, -7,
/* W */  -5,  0, -7, -4, -2, -1, -1,  1, -5, -3, -2, -2, -3,  1, -3, -3, -5, 20,  5, -3, -5, -1, -2, -7,
/* Y */  -4,  0, -4, -1, -6, -1, -2, -3,  0, -1,  3, -1, -1,  3, -2, -2, -1,  5,  9,  1, -3, -2, -1, -7,
/* V */   1, -1, -2, -2, -2, -3, -3, -3, -3,  4,  1, -2,  0,  1, -4, -1,  1, -3,  1,  5, -2, -3,  0, -7,
/* B */   0, -2,  4,  5, -2, -1,  0,  0, -2, -2, -1,  0, -2, -3, -2,  0,  0, -5, -3, -2,  5,  0, -1, -7,
/* Z */   0,  0, -1,  0,  0,  4,  5, -2,  0, -3, -1,  1, -1, -4,  0, -1, -1, -1, -2, -3,  0,  4,  0, -7,
/* X */   0, -1,  0, -1, -2,  0, -1, -1, -1,  0,  0,  0,  0, -1, -1,  0,  0, -2, -1,  0, -1,  0, -1, -7,
/* * */  -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7,  1
};

static const parasail_matrix_t parasail_blosum30 = {
    "blosum30",
    parasail_blosum30_,
    parasail_blosum_map,
    24,
    20,
    -7,
    NULL
};

#ifdef __cplusplus
}
#endif

#endif /* _PARASAIL_BLOSUM30_H_ */

