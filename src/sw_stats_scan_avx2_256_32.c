/**
 * @file
 *
 * @author jeff.daily@pnnl.gov
 *
 * Copyright (c) 2014 Battelle Memorial Institute.
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
#include "config.h"

#include <stdint.h>
#include <stdlib.h>

#include <immintrin.h>

#include "parasail.h"
#include "parasail/memory.h"
#include "parasail/internal_avx.h"
#include "parasail/matrices/blosum_map.h"

#define NEG_INF (INT32_MIN/(int32_t)(2))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define _mm256_cmplt_epi32_rpl(a,b) _mm256_cmpgt_epi32(b,a)

#define _mm256_rlli_si256_rpl(a,imm) _mm256_alignr_epi8(a, _mm256_permute2x128_si256(a, a, _MM_SHUFFLE(0,0,0,1)), 16-imm)

#define _mm256_srli_si256_rpl(a,imm) _mm256_or_si256(_mm256_slli_si256(_mm256_permute2x128_si256(a, a, _MM_SHUFFLE(3,0,0,1)), 16-imm), _mm256_srli_si256(a, imm))

#define _mm256_slli_si256_rpl(a,imm) _mm256_alignr_epi8(a, _mm256_permute2x128_si256(a, a, _MM_SHUFFLE(0,0,3,0)), 16-imm)


#ifdef PARASAIL_TABLE
static inline void arr_store_si256(
        int *array,
        __m256i vH,
        int32_t t,
        int32_t seglen,
        int32_t d,
        int32_t dlen)
{
    array[(0*seglen+t)*dlen + d] = (int32_t)_mm256_extract_epi32(vH, 0);
    array[(1*seglen+t)*dlen + d] = (int32_t)_mm256_extract_epi32(vH, 1);
    array[(2*seglen+t)*dlen + d] = (int32_t)_mm256_extract_epi32(vH, 2);
    array[(3*seglen+t)*dlen + d] = (int32_t)_mm256_extract_epi32(vH, 3);
    array[(4*seglen+t)*dlen + d] = (int32_t)_mm256_extract_epi32(vH, 4);
    array[(5*seglen+t)*dlen + d] = (int32_t)_mm256_extract_epi32(vH, 5);
    array[(6*seglen+t)*dlen + d] = (int32_t)_mm256_extract_epi32(vH, 6);
    array[(7*seglen+t)*dlen + d] = (int32_t)_mm256_extract_epi32(vH, 7);
}
#endif

#ifdef PARASAIL_TABLE
#define FNAME parasail_sw_stats_table_scan_avx2_256_32
#else
#define FNAME parasail_sw_stats_scan_avx2_256_32
#endif

parasail_result_t* FNAME(
        const char * const restrict s1, const int s1Len,
        const char * const restrict s2, const int s2Len,
        const int open, const int gap, const parasail_matrix_t *matrix)
{
    int32_t i = 0;
    int32_t j = 0;
    int32_t k = 0;
    int32_t segNum = 0;
    const int32_t n = 24; /* number of amino acids in table */
    const int32_t segWidth = 8;
    const int32_t segLen = (s1Len + segWidth - 1) / segWidth;
    __m256i* const restrict pvP  = parasail_memalign___m256i(32, n * segLen);
    __m256i* const restrict pvPm = parasail_memalign___m256i(32, n * segLen);
    __m256i* const restrict pvPs = parasail_memalign___m256i(32, n * segLen);
    __m256i* const restrict pvE  = parasail_memalign___m256i(32, segLen);
    __m256i* const restrict pvHt = parasail_memalign___m256i(32, segLen);
    __m256i* const restrict pvFt = parasail_memalign___m256i(32, segLen);
    __m256i* const restrict pvMt = parasail_memalign___m256i(32, segLen);
    __m256i* const restrict pvSt = parasail_memalign___m256i(32, segLen);
    __m256i* const restrict pvLt = parasail_memalign___m256i(32, segLen);
    __m256i* const restrict pvEx = parasail_memalign___m256i(32, segLen);
    __m256i* const restrict pvH  = parasail_memalign___m256i(32, segLen);
    __m256i* const restrict pvM  = parasail_memalign___m256i(32, segLen);
    __m256i* const restrict pvS  = parasail_memalign___m256i(32, segLen);
    __m256i* const restrict pvL  = parasail_memalign___m256i(32, segLen);
    __m256i vGapO = _mm256_set1_epi32(open);
    __m256i vGapE = _mm256_set1_epi32(gap);
    __m256i vZero = _mm256_setzero_si256();
    __m256i vOne = _mm256_set1_epi32(1);
    __m256i vNegInf = _mm256_set1_epi32(NEG_INF);
    int32_t score = NEG_INF;
    int32_t matches = 0;
    int32_t similar = 0;
    int32_t length = 0;
    __m256i vMaxH = vZero;
    __m256i vMaxM = vZero;
    __m256i vMaxS = vZero;
    __m256i vMaxL = vZero;
    const int32_t segLenXgap = -segLen*gap;
    __m256i insert_mask = _mm256_cmpeq_epi32(_mm256_setzero_si256(),
            _mm256_set_epi32(0,0,0,0,0,0,0,1));
    __m256i vSegLenXgap_reset = _mm256_blendv_epi8(vNegInf,
            _mm256_set1_epi32(segLenXgap),
            insert_mask);
    
#ifdef PARASAIL_TABLE
    parasail_result_t *result = parasail_result_new_table3(segLen*segWidth, s2Len);
#else
    parasail_result_t *result = parasail_result_new();
#endif

    parasail_memset___m256i(pvM, vZero, segLen);
    parasail_memset___m256i(pvS, vZero, segLen);
    parasail_memset___m256i(pvL, vZero, segLen);

    /* Generate query profile and match profile.
     * Rearrange query sequence & calculate the weight of match/mismatch.
     * Don't alias. */
    {
        int32_t index = 0;
        for (k=0; k<n; ++k) {
            for (i=0; i<segLen; ++i) {
                __m256i_32_t p;
                __m256i_32_t m;
                __m256i_32_t s;
                j = i;
                for (segNum=0; segNum<segWidth; ++segNum) {
                    p.v[segNum] = j >= s1Len ? 0 : matrix->matrix[matrix->size*k+matrix->mapper[(unsigned char)s1[j]]];
                    m.v[segNum] = j >= s1Len ? 0 : (k == matrix->mapper[(unsigned char)s1[j]]);
                    s.v[segNum] = p.v[segNum] > 0;
                    j += segLen;
                }
                _mm256_store_si256(&pvP[index], p.m);
                _mm256_store_si256(&pvPm[index], m.m);
                _mm256_store_si256(&pvPs[index], s.m);
                ++index;
            }
        }
    }

    /* initialize H and E */
    {
        int32_t index = 0;
        for (i=0; i<segLen; ++i) {
            __m256i_32_t h;
            __m256i_32_t e;
            for (segNum=0; segNum<segWidth; ++segNum) {
                h.v[segNum] = 0;
                e.v[segNum] = NEG_INF;
            }
            _mm256_store_si256(&pvH[index], h.m);
            _mm256_store_si256(&pvE[index], e.m);
            ++index;
        }
    }

    /* outer loop over database sequence */
    for (j=0; j<s2Len; ++j) {
        __m256i vE;
        __m256i vHt;
        __m256i vFt;
        __m256i vH;
        __m256i *pvW;
        __m256i vW;
        __m256i *pvC;
        __m256i *pvD;
        __m256i vC;
        __m256i vD;
        __m256i vM;
        __m256i vMp;
        __m256i vMt;
        __m256i vS;
        __m256i vSp;
        __m256i vSt;
        __m256i vL;
        __m256i vLp;
        __m256i vLt;
        __m256i vEx;

        /* calculate E */
        for (i=0; i<segLen; ++i) {
            vH = _mm256_load_si256(pvH+i);
            vE = _mm256_load_si256(pvE+i);
            vE = _mm256_max_epi32(
                    _mm256_sub_epi32(vE, vGapE),
                    _mm256_sub_epi32(vH, vGapO));
            _mm256_store_si256(pvE+i, vE);
        }

        /* calculate Ht */
        vH = _mm256_slli_si256_rpl(_mm256_load_si256(pvH+(segLen-1)), 4);
        vMp= _mm256_slli_si256_rpl(_mm256_load_si256(pvM+(segLen-1)), 4);
        vSp= _mm256_slli_si256_rpl(_mm256_load_si256(pvS+(segLen-1)), 4);
        vLp= _mm256_slli_si256_rpl(_mm256_load_si256(pvL+(segLen-1)), 4);
        vLp= _mm256_add_epi32(vLp, vOne);
        pvW = pvP + matrix->mapper[(unsigned char)s2[j]]*segLen;
        pvC = pvPm+ matrix->mapper[(unsigned char)s2[j]]*segLen;
        pvD = pvPs+ matrix->mapper[(unsigned char)s2[j]]*segLen;
        for (i=0; i<segLen; ++i) {
            __m256i cond_max;
            /* load values we need */
            vE = _mm256_load_si256(pvE+i);
            vW = _mm256_load_si256(pvW+i);
            /* compute */
            vH = _mm256_add_epi32(vH, vW);
            vHt = _mm256_max_epi32(vH, vE);
            vHt = _mm256_max_epi32(vHt, vZero);
            /* statistics */
            vC = _mm256_load_si256(pvC+i);
            vD = _mm256_load_si256(pvD+i);
            vMp = _mm256_add_epi32(vMp, vC);
            vSp = _mm256_add_epi32(vSp, vD);
            vEx = _mm256_cmpgt_epi32(vE, vH);
            vM = _mm256_load_si256(pvM+i);
            vS = _mm256_load_si256(pvS+i);
            vL = _mm256_load_si256(pvL+i);
            vL = _mm256_add_epi32(vL, vOne);
            vMt = _mm256_blendv_epi8(vMp, vM, vEx);
            vSt = _mm256_blendv_epi8(vSp, vS, vEx);
            vLt = _mm256_blendv_epi8(vLp, vL, vEx);
            cond_max = _mm256_cmpeq_epi32(vHt, vZero);
            vEx = _mm256_andnot_si256(cond_max, vEx);
            vMt = _mm256_andnot_si256(cond_max, vMt);
            vSt = _mm256_andnot_si256(cond_max, vSt);
            vLt = _mm256_andnot_si256(cond_max, vLt);
            /* store results */
            _mm256_store_si256(pvHt+i, vHt);
            _mm256_store_si256(pvEx+i, vEx);
            _mm256_store_si256(pvMt+i, vMt);
            _mm256_store_si256(pvSt+i, vSt);
            _mm256_store_si256(pvLt+i, vLt);
            /* prep for next iteration */
            vH = _mm256_load_si256(pvH+i);
            vMp = vM;
            vSp = vS;
            vLp = vL;
        }

        /* calculate Ft */
        vHt = _mm256_slli_si256_rpl(_mm256_load_si256(pvHt+(segLen-1)), 4);
        vFt = vNegInf;
        for (i=0; i<segLen; ++i) {
            vFt = _mm256_sub_epi32(vFt, vGapE);
            vFt = _mm256_max_epi32(vFt, vHt);
            vHt = _mm256_load_si256(pvHt+i);
        }
        {
            __m256i vFt_save = vFt;
            __m256i segLenXgap = vSegLenXgap_reset;
            for (i=0; i<segWidth-1; ++i) {
                __m256i vFtt = _mm256_slli_si256_rpl(vFt, 4);
                segLenXgap = _mm256_rlli_si256_rpl(segLenXgap, 4);
                vFtt = _mm256_add_epi32(vFtt, segLenXgap);
                vFt = _mm256_max_epi32(vFt, vFtt);
            }
            vFt = _mm256_blendv_epi8(vFt_save, vFt, insert_mask);
        }
        vHt = _mm256_slli_si256_rpl(_mm256_load_si256(pvHt+(segLen-1)), 4);
        vFt = _mm256_slli_si256_rpl(vFt, 4);
        vFt = _mm256_insert_epi32(vFt, NEG_INF, 0);
        for (i=0; i<segLen; ++i) {
            vFt = _mm256_sub_epi32(vFt, vGapE);
            vFt = _mm256_max_epi32(vFt, vHt);
            vHt = _mm256_load_si256(pvHt+i);
            _mm256_store_si256(pvFt+i, vFt);
        }

        /* calculate H,M,L */
        vMp = vZero;
        vSp = vZero;
        vLp = vOne;
        vC = _mm256_cmpeq_epi32(vZero, vZero); /* check if prefix sum is needed */
        vC = _mm256_srli_si256_rpl(vC, 4); /* zero out last value */
        for (i=0; i<segLen; ++i) {
            /* load values we need */
            vHt = _mm256_load_si256(pvHt+i);
            vFt = _mm256_load_si256(pvFt+i);
            /* compute */
            vFt = _mm256_sub_epi32(vFt, vGapO);
            vH = _mm256_max_epi32(vHt, vFt);
            /* statistics */
            vEx = _mm256_load_si256(pvEx+i);
            vMt = _mm256_load_si256(pvMt+i);
            vSt = _mm256_load_si256(pvSt+i);
            vLt = _mm256_load_si256(pvLt+i);
            vEx = _mm256_or_si256(
                    _mm256_and_si256(vEx, _mm256_cmpeq_epi32(vHt, vFt)),
                    _mm256_cmplt_epi32_rpl(vHt, vFt));
            vM = _mm256_blendv_epi8(vMt, vMp, vEx);
            vS = _mm256_blendv_epi8(vSt, vSp, vEx);
            vL = _mm256_blendv_epi8(vLt, vLp, vEx);
            vMp = vM;
            vSp = vS;
            vLp = _mm256_add_epi32(vL, vOne);
            vC = _mm256_and_si256(vC, vEx);
            /* store results */
            _mm256_store_si256(pvH+i, vH);
            _mm256_store_si256(pvEx+i, vEx);
            
#ifdef PARASAIL_TABLE
            arr_store_si256(result->score_table, vH, i, segLen, j, s2Len);
#endif
        }
        {
            vLp = _mm256_sub_epi32(vLp, vOne);
            {
                __m256i_32_t uMp, uSp, uLp, uC;
                uC.m = vC;
                uMp.m = vMp;
                uMp.v[1] = uC.v[1] ? uMp.v[0] : uMp.v[1];
                uMp.v[2] = uC.v[2] ? uMp.v[1] : uMp.v[2];
                uMp.v[3] = uC.v[3] ? uMp.v[2] : uMp.v[3];
                uMp.v[4] = uC.v[4] ? uMp.v[3] : uMp.v[4];
                uMp.v[5] = uC.v[5] ? uMp.v[4] : uMp.v[5];
                uMp.v[6] = uC.v[6] ? uMp.v[5] : uMp.v[6];
                uMp.v[7] = uC.v[7] ? uMp.v[6] : uMp.v[7];
                vMp = uMp.m;
                uSp.m = vSp;
                uSp.v[1] = uC.v[1] ? uSp.v[0] : uSp.v[1];
                uSp.v[2] = uC.v[2] ? uSp.v[1] : uSp.v[2];
                uSp.v[3] = uC.v[3] ? uSp.v[2] : uSp.v[3];
                uSp.v[4] = uC.v[4] ? uSp.v[3] : uSp.v[4];
                uSp.v[5] = uC.v[5] ? uSp.v[4] : uSp.v[5];
                uSp.v[6] = uC.v[6] ? uSp.v[5] : uSp.v[6];
                uSp.v[7] = uC.v[7] ? uSp.v[6] : uSp.v[7];
                vSp = uSp.m;
                uLp.m = vLp;
                uLp.v[1] = uC.v[1] ? uLp.v[1] + uLp.v[0] : uLp.v[1];
                uLp.v[2] = uC.v[2] ? uLp.v[2] + uLp.v[1] : uLp.v[2];
                uLp.v[3] = uC.v[3] ? uLp.v[3] + uLp.v[2] : uLp.v[3];
                uLp.v[4] = uC.v[4] ? uLp.v[4] + uLp.v[3] : uLp.v[4];
                uLp.v[5] = uC.v[5] ? uLp.v[5] + uLp.v[4] : uLp.v[5];
                uLp.v[6] = uC.v[6] ? uLp.v[6] + uLp.v[5] : uLp.v[6];
                uLp.v[7] = uC.v[7] ? uLp.v[7] + uLp.v[6] : uLp.v[7];
                vLp = uLp.m;
            }
            vLp = _mm256_add_epi32(vLp, vOne);
        }
        /* final pass for M,L */
        vMp = _mm256_slli_si256_rpl(vMp, 4);
        vSp = _mm256_slli_si256_rpl(vSp, 4);
        vLp = _mm256_slli_si256_rpl(vLp, 4);
        for (i=0; i<segLen; ++i) {
            /* load values we need */
            vH = _mm256_load_si256(pvH+i);
            /* statistics */
            vEx = _mm256_load_si256(pvEx+i);
            vMt = _mm256_load_si256(pvMt+i);
            vSt = _mm256_load_si256(pvSt+i);
            vLt = _mm256_load_si256(pvLt+i);
            vM = _mm256_blendv_epi8(vMt, vMp, vEx);
            vS = _mm256_blendv_epi8(vSt, vSp, vEx);
            vL = _mm256_blendv_epi8(vLt, vLp, vEx);
            vMp = vM;
            vSp = vS;
            vLp = _mm256_add_epi32(vL, vOne);
            /* store results */
            _mm256_store_si256(pvM+i, vM);
            _mm256_store_si256(pvS+i, vS);
            _mm256_store_si256(pvL+i, vL);
            
#ifdef PARASAIL_TABLE
            arr_store_si256(result->matches_table, vM, i, segLen, j, s2Len);
            arr_store_si256(result->similar_table, vS, i, segLen, j, s2Len);
            arr_store_si256(result->length_table, vL, i, segLen, j, s2Len);
#endif
            /* update max vector seen so far */
            {
                __m256i cond_max = _mm256_cmpgt_epi32(vH, vMaxH);
                vMaxH = _mm256_blendv_epi8(vMaxH, vH, cond_max);
                vMaxM = _mm256_blendv_epi8(vMaxM, vM, cond_max);
                vMaxS = _mm256_blendv_epi8(vMaxS, vS, cond_max);
                vMaxL = _mm256_blendv_epi8(vMaxL, vL, cond_max);
            }
        }
    }

    /* max in vec */
    for (j=0; j<segWidth; ++j) {
        int32_t value = (int32_t) _mm256_extract_epi32(vMaxH, 7);
        if (value > score) {
            score = value;
            matches = (int32_t) _mm256_extract_epi32(vMaxM, 7);
            similar = (int32_t) _mm256_extract_epi32(vMaxS, 7);
            length = (int32_t) _mm256_extract_epi32(vMaxL, 7);
        }
        vMaxH = _mm256_slli_si256_rpl(vMaxH, 4);
        vMaxM = _mm256_slli_si256_rpl(vMaxM, 4);
        vMaxS = _mm256_slli_si256_rpl(vMaxS, 4);
        vMaxL = _mm256_slli_si256_rpl(vMaxL, 4);
    }

    

    result->score = score;
    result->matches = matches;
    result->similar = similar;
    result->length = length;

    parasail_free(pvL);
    parasail_free(pvS);
    parasail_free(pvM);
    parasail_free(pvH);
    parasail_free(pvEx);
    parasail_free(pvLt);
    parasail_free(pvSt);
    parasail_free(pvMt);
    parasail_free(pvFt);
    parasail_free(pvHt);
    parasail_free(pvE);
    parasail_free(pvPs);
    parasail_free(pvPm);
    parasail_free(pvP);

    return result;
}


