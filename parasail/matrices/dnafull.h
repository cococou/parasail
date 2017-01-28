/**
 * @file
 *
 * @author jeff.daily@pnnl.gov
 *
 * Copyright (c) 2015 Battelle Memorial Institute.
 */
#ifndef _PARASAIL_DNAFULL_H_
#define _PARASAIL_DNAFULL_H_

#include "parasail.h"

#ifdef __cplusplus
extern "C" {
#endif

/* # This matrix was created by Todd Lowe   12/10/92 */
/* # Uses ambiguous nucleotide codes, probabilities rounded to */
/* #  nearest integer */
/* # Lowest score = -4, Highest score = 5 */

static const int parasail_dnafull_[] = {
/*       0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 */
/*       A   T   G   C   S   W   R   Y   K   M   B   V   H   D   N   U */
/* A */  5, -4, -4, -4, -4,  1,  1, -4, -4,  1, -4, -1, -1, -1, -2, -4,
/* T */ -4,  5, -4, -4, -4,  1, -4,  1,  1, -4, -1, -4, -1, -1, -2,  5,
/* G */ -4, -4,  5, -4,  1, -4,  1, -4,  1, -4, -1, -1, -4, -1, -2, -4,
/* C */ -4, -4, -4,  5,  1, -4, -4,  1, -4,  1, -1, -1, -1, -4, -2, -4,
/* S */ -4, -4,  1,  1, -1, -4, -2, -2, -2, -2, -1, -1, -3, -3, -1, -4,
/* W */  1,  1, -4, -4, -4, -1, -2, -2, -2, -2, -3, -3, -1, -1, -1,  1,
/* R */  1, -4,  1, -4, -2, -2, -1, -4, -2, -2, -3, -1, -3, -1, -1, -4,
/* Y */ -4,  1, -4,  1, -2, -2, -4, -1, -2, -2, -1, -3, -1, -3, -1,  1,
/* K */ -4,  1,  1, -4, -2, -2, -2, -2, -1, -4, -1, -3, -3, -1, -1,  1,
/* M */  1, -4, -4,  1, -2, -2, -2, -2, -4, -1, -3, -1, -1, -3, -1, -4,
/* B */ -4, -1, -1, -1, -1, -3, -3, -1, -1, -3, -1, -2, -2, -2, -1, -1,
/* V */ -1, -4, -1, -1, -1, -3, -1, -3, -3, -1, -2, -1, -2, -2, -1, -4,
/* H */ -1, -1, -4, -1, -3, -1, -3, -1, -3, -1, -2, -2, -1, -2, -1, -1,  
/* D */ -1, -1, -1, -4, -3, -1, -1, -3, -1, -3, -2, -2, -2, -1, -1, -1,
/* N */ -2, -2, -2, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2,
/* U */ -4,  5, -4, -4, -4,  1, -4,  1,  1, -4, -1, -4, -1, -1, -2,  5
};

static const int parasail_dnafull_map_[256] = {
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15,  0, 10,  3, 13, 15, 15,  2, 12, 15, 15,  8, 15,  9, 14, 15,
     15, 15,  6,  4,  1, 15, 11,  5, 15,  7, 15, 15, 15, 15, 15, 15,
     15,  0, 10,  3, 13, 15, 15,  2, 12, 15, 15,  8, 15,  9, 14, 15,
     15, 15,  6,  4,  1, 15, 11,  5, 15,  7, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

static const parasail_matrix_t parasail_dnafull = {
    "dnafull",
    parasail_dnafull_,
    parasail_dnafull_map_,
    16,
    5,
    -4,
    0
};

#ifdef __cplusplus
}
#endif

#endif /* _PARASAIL_DNAFULL_H_ */

