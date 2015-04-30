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
 * ftp://ftp.cbi.pku.edu.cn/pub/software/blast/matrices/PAM440, the
 * Center for Bioinformatics, Peking University, China.
 */
#ifndef _PARASAIL_PAM440_H_
#define _PARASAIL_PAM440_H_

#include "parasail.h"
#include "pam_map.h"

/* # */
/* # This matrix was produced by "pam" Version 1.0.6 [28-Jul-93] */
/* # */
/* # PAM 440 substitution matrix, scale = ln(2)/6 = 0.115525 */
/* # */
/* # Expected score = -0.529, Entropy = 0.111 bits */
/* # */
/* # Lowest score = -9, Highest score = 30 */
/* # */

static const int parasail_pam440_[] = {
/*        A   R   N   D   C   Q   E   G   H   I   L   K   M   F   P   S   T   W   Y   V   B   Z   X   * */
/* A */   1, -1,  0,  1, -2,  0,  1,  2, -1,  0, -2,  0, -1, -4,  1,  1,  1, -6, -4,  0,  1,  0,  0, -9,
/* R */  -1,  5,  1,  0, -4,  2,  0, -2,  2, -2, -3,  4,  0, -5,  0,  0,  0,  4, -4, -2,  0,  1,  0, -9,
/* N */   0,  1,  1,  2, -4,  1,  2,  1,  1, -2, -3,  1, -1, -4,  0,  1,  0, -5, -3, -1,  2,  1,  0, -9,
/* D */   1,  0,  2,  3, -5,  2,  3,  1,  1, -2, -3,  1, -2, -6,  0,  1,  0, -7, -5, -2,  2,  2,  0, -9,
/* C */  -2, -4, -4, -5, 20, -5, -5, -3, -4, -2, -6, -5, -5, -4, -3,  0, -2, -9,  1, -2, -4, -5, -3, -9,
/* Q */   0,  2,  1,  2, -5,  3,  2,  0,  3, -2, -2,  1, -1, -4,  1,  0,  0, -5, -4, -1,  2,  2,  0, -9,
/* E */   1,  0,  2,  3, -5,  2,  3,  1,  1, -2, -3,  1, -2, -5,  0,  1,  0, -7, -5, -1,  2,  3,  0, -9,
/* G */   2, -2,  1,  1, -3,  0,  1,  4, -1, -2, -4, -1, -2, -5,  0,  1,  1, -8, -5, -1,  1,  0,  0, -9,
/* H */  -1,  2,  1,  1, -4,  3,  1, -1,  5, -2, -2,  1, -1, -2,  0,  0, -1, -3,  0, -2,  1,  2,  0, -9,
/* I */   0, -2, -2, -2, -2, -2, -2, -2, -2,  4,  4, -2,  3,  2, -1, -1,  0, -5,  0,  3, -2, -2,  0, -9,
/* L */  -2, -3, -3, -3, -6, -2, -3, -4, -2,  4,  7, -3,  5,  4, -2, -2, -1, -2,  1,  3, -3, -2, -1, -9,
/* K */   0,  4,  1,  1, -5,  1,  1, -1,  1, -2, -3,  4,  0, -5,  0,  0,  0, -3, -5, -2,  1,  1,  0, -9,
/* M */  -1,  0, -1, -2, -5, -1, -2, -2, -1,  3,  5,  0,  4,  1, -1, -1,  0, -4, -1,  2, -2, -1,  0, -9,
/* F */  -4, -5, -4, -6, -4, -4, -5, -5, -2,  2,  4, -5,  1, 13, -5, -4, -3,  2, 12,  0, -5, -5, -2, -9,
/* P */   1,  0,  0,  0, -3,  1,  0,  0,  0, -1, -2,  0, -1, -5,  5,  1,  1, -6, -5, -1,  0,  0,  0, -9,
/* S */   1,  0,  1,  1,  0,  0,  1,  1,  0, -1, -2,  0, -1, -4,  1,  1,  1, -3, -3, -1,  1,  0,  0, -9,
/* T */   1,  0,  0,  0, -2,  0,  0,  1, -1,  0, -1,  0,  0, -3,  1,  1,  1, -6, -3,  0,  0,  0,  0, -9,
/* W */  -6,  4, -5, -7, -9, -5, -7, -8, -3, -5, -2, -3, -4,  2, -6, -3, -6, 30,  2, -6, -6, -6, -4, -9,
/* Y */  -4, -4, -3, -5,  1, -4, -5, -5,  0,  0,  1, -5, -1, 12, -5, -3, -3,  2, 15, -2, -4, -4, -2, -9,
/* V */   0, -2, -1, -2, -2, -1, -1, -1, -2,  3,  3, -2,  2,  0, -1, -1,  0, -6, -2,  4, -1, -1,  0, -9,
/* B */   1,  0,  2,  2, -4,  2,  2,  1,  1, -2, -3,  1, -2, -5,  0,  1,  0, -6, -4, -1,  2,  2,  0, -9,
/* Z */   0,  1,  1,  2, -5,  2,  3,  0,  2, -2, -2,  1, -1, -5,  0,  0,  0, -6, -4, -1,  2,  3,  0, -9,
/* X */   0,  0,  0,  0, -3,  0,  0,  0,  0,  0, -1,  0,  0, -2,  0,  0,  0, -4, -2,  0,  0,  0, -1, -9,
/* * */  -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9,  1
};

static const parasail_matrix_t parasail_pam440 = {
    "pam440",
    parasail_pam440_,
    parasail_pam_map,
    24
};

#endif /* _PARASAIL_PAM440_H_ */
