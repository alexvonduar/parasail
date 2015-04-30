/**
 * @file
 *
 * @author jeff.daily@pnnl.gov
 *
 * Copyright (c) 2014 Battelle Memorial Institute.
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 *
 * This file was converted to C code from the raw file found at
 * ftp://ftp.cbi.pku.edu.cn/pub/software/blast/matrices/PAM80, the
 * Center for Bioinformatics, Peking University, China.
 */
#ifndef _PARASAIL_PAM80_H_
#define _PARASAIL_PAM80_H_

#include "parasail.h"
#include "pam_map.h"

/* # */
/* # This matrix was produced by "pam" Version 1.0.6 [28-Jul-93] */
/* # */
/* # PAM 80 substitution matrix, scale = ln(2)/2 = 0.346574 */
/* # */
/* # Expected score = -2.55, Entropy = 1.44 bits */
/* # */
/* # Lowest score = -11, Highest score = 13 */
/* # */

static const int parasail_pam80_[] = {
/*        A   R   N   D   C   Q   E   G   H   I   L   K   M   F   P   S   T   W   Y   V   B   Z   X   * */
/* A */   4, -4, -1, -1, -4, -2, -1,  0, -4, -2, -4, -4, -3, -5,  0,  1,  1, -8, -5,  0, -1, -1, -1,-11,
/* R */  -4,  7, -2, -5, -5,  0, -4, -6,  0, -3, -5,  2, -2, -6, -2, -1, -3,  0, -7, -5, -3, -1, -3,-11,
/* N */  -1, -2,  5,  3, -6, -1,  0, -1,  2, -3, -5,  0, -4, -5, -3,  1,  0, -5, -3, -4,  4,  0, -1,-11,
/* D */  -1, -5,  3,  6, -9,  0,  4, -1, -1, -4, -7, -2, -6, -9, -4, -1, -2,-10, -7, -5,  5,  2, -3,-11,
/* C */  -4, -5, -6, -9,  9, -9, -9, -6, -5, -4, -9, -9, -8, -8, -5, -1, -4,-10, -2, -3, -7, -9, -5,-11,
/* Q */  -2,  0, -1,  0, -9,  7,  2, -4,  2, -4, -3, -1, -2, -8, -1, -3, -3, -8, -7, -4,  0,  5, -2,-11,
/* E */  -1, -4,  0,  4, -9,  2,  6, -2, -2, -3, -6, -2, -4, -9, -3, -2, -3,-11, -6, -4,  2,  5, -2,-11,
/* G */   0, -6, -1, -1, -6, -4, -2,  6, -5, -6, -7, -4, -5, -6, -3,  0, -2,-10, -8, -3, -1, -2, -3,-11,
/* H */  -4,  0,  2, -1, -5,  2, -2, -5,  8, -5, -4, -3, -5, -3, -2, -3, -4, -4, -1, -4,  0,  1, -2,-11,
/* I */  -2, -3, -3, -4, -4, -4, -3, -6, -5,  7,  1, -4,  1,  0, -5, -4, -1, -8, -3,  3, -4, -4, -2,-11,
/* L */  -4, -5, -5, -7, -9, -3, -6, -7, -4,  1,  6, -5,  2,  0, -4, -5, -4, -3, -4,  0, -6, -4, -3,-11,
/* K */  -4,  2,  0, -2, -9, -1, -2, -4, -3, -4, -5,  6,  0, -9, -4, -2, -1, -7, -6, -5, -1, -1, -3,-11,
/* M */  -3, -2, -4, -6, -8, -2, -4, -5, -5,  1,  2,  0,  9, -2, -5, -3, -2, -7, -6,  1, -5, -3, -2,-11,
/* F */  -5, -6, -5, -9, -8, -8, -9, -6, -3,  0,  0, -9, -2,  8, -7, -4, -5, -2,  4, -4, -7, -8, -5,-11,
/* P */   0, -2, -3, -4, -5, -1, -3, -3, -2, -5, -4, -4, -5, -7,  7,  0, -2, -9, -8, -3, -3, -2, -2,-11,
/* S */   1, -1,  1, -1, -1, -3, -2,  0, -3, -4, -5, -2, -3, -4,  0,  4,  2, -3, -4, -3,  0, -2, -1,-11,
/* T */   1, -3,  0, -2, -4, -3, -3, -2, -4, -1, -4, -1, -2, -5, -2,  2,  5, -8, -4, -1, -1, -3, -1,-11,
/* W */  -8,  0, -5,-10,-10, -8,-11,-10, -4, -8, -3, -7, -7, -2, -9, -3, -8, 13, -2,-10, -7, -9, -7,-11,
/* Y */  -5, -7, -3, -7, -2, -7, -6, -8, -1, -3, -4, -6, -6,  4, -8, -4, -4, -2,  9, -5, -4, -6, -4,-11,
/* V */   0, -5, -4, -5, -3, -4, -4, -3, -4,  3,  0, -5,  1, -4, -3, -3, -1,-10, -5,  6, -4, -4, -2,-11,
/* B */  -1, -3,  4,  5, -7,  0,  2, -1,  0, -4, -6, -1, -5, -7, -3,  0, -1, -7, -4, -4,  5,  2, -2,-11,
/* Z */  -1, -1,  0,  2, -9,  5,  5, -2,  1, -4, -4, -1, -3, -8, -2, -2, -3, -9, -6, -4,  2,  5, -2,-11,
/* X */  -1, -3, -1, -3, -5, -2, -2, -3, -2, -2, -3, -3, -2, -5, -2, -1, -1, -7, -4, -2, -2, -2, -3,-11,
/* * */ -11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,  1
};

static const parasail_matrix_t parasail_pam80 = {
    "pam80",
    parasail_pam80_,
    parasail_pam_map,
    24
};

#endif /* _PARASAIL_PAM80_H_ */
