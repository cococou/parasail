/**
 * @file
 *
 * @author jeffrey.daily@gmail.com
 *
 * Copyright (c) 2015 Battelle Memorial Institute.
 */
#include "config.h"

#include <stdint.h>
#include <stdlib.h>



#include "parasail.h"
#include "parasail/memory.h"
#include "parasail/internal_neon.h"

#define SG_TRACE
#define SG_SUFFIX _scan_neon_128_8
#define SG_SUFFIX_PROF _scan_profile_neon_128_8
#include "sg_helper.h"



static inline void arr_store(
        simde__m128i *array,
        simde__m128i vH,
        int32_t t,
        int32_t seglen,
        int32_t d)
{
    simde_mm_store_si128(array + (1LL*d*seglen+t), vH);
}

static inline simde__m128i arr_load(
        simde__m128i *array,
        int32_t t,
        int32_t seglen,
        int32_t d)
{
    return simde_mm_load_si128(array + (1LL*d*seglen+t));
}

#define FNAME parasail_sg_flags_trace_scan_neon_128_8
#define PNAME parasail_sg_flags_trace_scan_profile_neon_128_8

parasail_result_t* FNAME(
        const char * const restrict s1, const int s1Len,
        const char * const restrict s2, const int s2Len,
        const int open, const int gap, const parasail_matrix_t *matrix,
        int s1_beg, int s1_end, int s2_beg, int s2_end)
{
    parasail_profile_t *profile = parasail_profile_create_neon_128_8(s1, s1Len, matrix);
    parasail_result_t *result = PNAME(profile, s2, s2Len, open, gap, s1_beg, s1_end, s2_beg, s2_end);
    parasail_profile_free(profile);
    return result;
}

parasail_result_t* PNAME(
        const parasail_profile_t * const restrict profile,
        const char * const restrict s2, const int s2Len,
        const int open, const int gap,
        int s1_beg, int s1_end, int s2_beg, int s2_end)
{
    int32_t i = 0;
    int32_t j = 0;
    int32_t k = 0;
    const int s1Len = profile->s1Len;
    int32_t end_query = s1Len-1;
    int32_t end_ref = s2Len-1;
    const parasail_matrix_t *matrix = profile->matrix;
    const int32_t segWidth = 16; /* number of values in vector unit */
    const int32_t segLen = (s1Len + segWidth - 1) / segWidth;
    const int32_t offset = (s1Len - 1) % segLen;
    const int32_t position = (segWidth - 1) - (s1Len - 1) / segLen;
    simde__m128i* const restrict pvP  = (simde__m128i*)profile->profile8.score;
    simde__m128i* const restrict pvE  = parasail_memalign_simde__m128i(16, segLen);
    int8_t* const restrict boundary = parasail_memalign_int8_t(16, s2Len+1);
    simde__m128i* const restrict pvHt = parasail_memalign_simde__m128i(16, segLen);
    simde__m128i* const restrict pvH  = parasail_memalign_simde__m128i(16, segLen);
    simde__m128i* const restrict pvGapper = parasail_memalign_simde__m128i(16, segLen);
    simde__m128i vGapO = simde_mm_set1_epi8(open);
    simde__m128i vGapE = simde_mm_set1_epi8(gap);
    const int8_t NEG_LIMIT = (-open < matrix->min ?
        INT8_MIN + open : INT8_MIN - matrix->min) + 1;
    const int8_t POS_LIMIT = INT8_MAX - matrix->max - 1;
    simde__m128i vZero = simde_mm_setzero_si128();
    int8_t score = NEG_LIMIT;
    simde__m128i vNegLimit = simde_mm_set1_epi8(NEG_LIMIT);
    simde__m128i vPosLimit = simde_mm_set1_epi8(POS_LIMIT);
    simde__m128i vSaturationCheckMin = vPosLimit;
    simde__m128i vSaturationCheckMax = vNegLimit;
    simde__m128i vMaxH = vNegLimit;
    simde__m128i vPosMask = simde_mm_cmpeq_epi8(simde_mm_set1_epi8(position),
            simde_mm_set_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15));
    simde__m128i vNegInfFront = vZero;
    simde__m128i vSegLenXgap;
    parasail_result_t *result = parasail_result_new_trace(segLen, s2Len, 16, sizeof(simde__m128i));
    simde__m128i vTIns  = simde_mm_set1_epi8(PARASAIL_INS);
    simde__m128i vTDel  = simde_mm_set1_epi8(PARASAIL_DEL);
    simde__m128i vTDiag = simde_mm_set1_epi8(PARASAIL_DIAG);
    simde__m128i vTDiagE = simde_mm_set1_epi8(PARASAIL_DIAG_E);
    simde__m128i vTInsE = simde_mm_set1_epi8(PARASAIL_INS_E);
    simde__m128i vTDiagF = simde_mm_set1_epi8(PARASAIL_DIAG_F);
    simde__m128i vTDelF = simde_mm_set1_epi8(PARASAIL_DEL_F);

    vNegInfFront = simde_mm_insert_epi8(vNegInfFront, NEG_LIMIT, 0);
    vSegLenXgap = simde_mm_adds_epi8(vNegInfFront,
            simde_mm_slli_si128(simde_mm_set1_epi8(-segLen*gap), 1));

    /* initialize H and E */
    {
        int32_t index = 0;
        for (i=0; i<segLen; ++i) {
            int32_t segNum = 0;
            simde__m128i h;
            simde__m128i e;
            for (segNum=0; segNum<segWidth; ++segNum) {
                int64_t tmp = s1_beg ? 0 : (-open-gap*(segNum*segLen+i));
                h.i8[segNum] = tmp < INT8_MIN ? INT8_MIN : tmp;
                tmp = tmp - open;
                e.i8[segNum] = tmp < INT8_MIN ? INT8_MIN : tmp;
            }
            simde_mm_store_si128(&pvH[index], h);
            simde_mm_store_si128(&pvE[index], e);
            ++index;
        }
    }

    /* initialize uppder boundary */
    {
        boundary[0] = 0;
        for (i=1; i<=s2Len; ++i) {
            int64_t tmp = s2_beg ? 0 : (-open-gap*(i-1));
            boundary[i] = tmp < INT8_MIN ? INT8_MIN : tmp;
        }
    }

    {
        simde__m128i vGapper = simde_mm_subs_epi8(vZero,vGapO);
        for (i=segLen-1; i>=0; --i) {
            simde_mm_store_si128(pvGapper+i, vGapper);
            vGapper = simde_mm_subs_epi8(vGapper, vGapE);
        }
    }

    /* outer loop over database sequence */
    for (j=0; j<s2Len; ++j) {
        simde__m128i vE;
        simde__m128i vE_ext;
        simde__m128i vE_opn;
        simde__m128i vHt;
        simde__m128i vF;
        simde__m128i vF_ext;
        simde__m128i vF_opn;
        simde__m128i vH;
        simde__m128i vHp;
        simde__m128i *pvW;
        simde__m128i vW;
        simde__m128i case1;
        simde__m128i case2;
        simde__m128i vGapper;
        simde__m128i vT;
        simde__m128i vET;
        simde__m128i vFT;

        /* calculate E */
        /* calculate Ht */
        /* calculate F and H first pass */
        vHp = simde_mm_load_si128(pvH+(segLen-1));
        vHp = simde_mm_slli_si128(vHp, 1);
        vHp = simde_mm_insert_epi8(vHp, boundary[j], 0);
        pvW = pvP + matrix->mapper[(unsigned char)s2[j]]*segLen;
        vHt = simde_mm_subs_epi8(vNegLimit, pvGapper[0]);
        vF = vNegLimit;
        for (i=0; i<segLen; ++i) {
            vH = simde_mm_load_si128(pvH+i);
            vE = simde_mm_load_si128(pvE+i);
            vW = simde_mm_load_si128(pvW+i);
            vGapper = simde_mm_load_si128(pvGapper+i);
            vE_opn = simde_mm_subs_epi8(vH, vGapO);
            vE_ext = simde_mm_subs_epi8(vE, vGapE);
            case1 = simde_mm_cmpgt_epi8(vE_opn, vE_ext);
            vET = simde_mm_blendv_epi8(vTInsE, vTDiagE, case1);
            arr_store(result->trace->trace_table, vET, i, segLen, j);
            vE = simde_mm_max_epi8(vE_opn, vE_ext);
            vSaturationCheckMin = simde_mm_min_epi8(vSaturationCheckMin, vE);
            vGapper = simde_mm_adds_epi8(vHt, vGapper);
            vF = simde_mm_max_epi8(vF, vGapper);
            vHp = simde_mm_adds_epi8(vHp, vW);
            vHt = simde_mm_max_epi8(vE, vHp);
            simde_mm_store_si128(pvE+i, vE);
            simde_mm_store_si128(pvHt+i, vHt);
            simde_mm_store_si128(pvH+i, vHp);
            vHp = vH;
        }

        /* pseudo prefix scan on F and H */
        vHt = simde_mm_slli_si128(vHt, 1);
        vHt = simde_mm_insert_epi8(vHt, boundary[j+1], 0);
        vGapper = simde_mm_load_si128(pvGapper);
        vGapper = simde_mm_adds_epi8(vHt, vGapper);
        vF = simde_mm_max_epi8(vF, vGapper);
        for (i=0; i<segWidth-2; ++i) {
            simde__m128i vFt = simde_mm_slli_si128(vF, 1);
            vFt = simde_mm_adds_epi8(vFt, vSegLenXgap);
            vF = simde_mm_max_epi8(vF, vFt);
        }

        /* calculate final H */
        vF = simde_mm_slli_si128(vF, 1);
        vF = simde_mm_adds_epi8(vF, vNegInfFront);
        vH = simde_mm_max_epi8(vF, vHt);
        for (i=0; i<segLen; ++i) {
            vET = arr_load(result->trace->trace_table, i, segLen, j);
            vHp = simde_mm_load_si128(pvH+i);
            vHt = simde_mm_load_si128(pvHt+i);
            vF_opn = simde_mm_subs_epi8(vH, vGapO);
            vF_ext = simde_mm_subs_epi8(vF, vGapE);
            vF = simde_mm_max_epi8(vF_opn, vF_ext);
            case1 = simde_mm_cmpgt_epi8(vF_opn, vF_ext);
            vFT = simde_mm_blendv_epi8(vTDelF, vTDiagF, case1);
            vH = simde_mm_max_epi8(vHt, vF);
            case1 = simde_mm_cmpeq_epi8(vH, vHp);
            case2 = simde_mm_cmpeq_epi8(vH, vF);
            vT = simde_mm_blendv_epi8(
                    simde_mm_blendv_epi8(vTIns, vTDel, case2),
                    vTDiag, case1);
            vT = simde_mm_or_si128(vT, vET);
            vT = simde_mm_or_si128(vT, vFT);
            arr_store(result->trace->trace_table, vT, i, segLen, j);
            simde_mm_store_si128(pvH+i, vH);
            vSaturationCheckMin = simde_mm_min_epi8(vSaturationCheckMin, vH);
            vSaturationCheckMin = simde_mm_min_epi8(vSaturationCheckMin, vF);
            vSaturationCheckMax = simde_mm_max_epi8(vSaturationCheckMax, vH);
        }

        /* extract vector containing last value from column */
        {
            simde__m128i vCompare;
            vH = simde_mm_load_si128(pvH + offset);
            vCompare = simde_mm_and_si128(vPosMask, simde_mm_cmpgt_epi8(vH, vMaxH));
            vMaxH = simde_mm_max_epi8(vH, vMaxH);
            if (simde_mm_movemask_epi8(vCompare)) {
                end_ref = j;
            }
        }
    } 

    /* max last value from all columns */
    if (s2_end)
    {
        for (k=0; k<position; ++k) {
            vMaxH = simde_mm_slli_si128(vMaxH, 1);
        }
        score = (int8_t) simde_mm_extract_epi8(vMaxH, 15);
        end_query = s1Len-1;
    }

    /* max of last column */
    if (s1_end)
    {
        /* Trace the alignment ending position on read. */
        int8_t *t = (int8_t*)pvH;
        int32_t column_len = segLen * segWidth;
        for (i = 0; i<column_len; ++i, ++t) {
            int32_t temp = i / segWidth + i % segWidth * segLen;
            if (temp >= s1Len) continue;
            if (*t > score) {
                score = *t;
                end_query = temp;
                end_ref = s2Len-1;
            }
            else if (*t == score && end_ref == s2Len-1 && temp < end_query) {
                end_query = temp;
            }
        }
    }

    if (!s1_end && !s2_end) {
        /* extract last value from the last column */
        {
            simde__m128i vH = simde_mm_load_si128(pvH + offset);
            for (k=0; k<position; ++k) {
                vH = simde_mm_slli_si128(vH, 1);
            }
            score = (int8_t) simde_mm_extract_epi8 (vH, 15);
            end_ref = s2Len - 1;
            end_query = s1Len - 1;
        }
    }

    if (simde_mm_movemask_epi8(simde_mm_or_si128(
            simde_mm_cmplt_epi8(vSaturationCheckMin, vNegLimit),
            simde_mm_cmpgt_epi8(vSaturationCheckMax, vPosLimit)))) {
        result->flag |= PARASAIL_FLAG_SATURATED;
        score = 0;
        end_query = 0;
        end_ref = 0;
    }

    result->score = score;
    result->end_query = end_query;
    result->end_ref = end_ref;
    result->flag |= PARASAIL_FLAG_SG | PARASAIL_FLAG_SCAN
        | PARASAIL_FLAG_TRACE
        | PARASAIL_FLAG_BITS_8 | PARASAIL_FLAG_LANES_16;
    result->flag |= s1_beg ? PARASAIL_FLAG_SG_S1_BEG : 0;
    result->flag |= s1_end ? PARASAIL_FLAG_SG_S1_END : 0;
    result->flag |= s2_beg ? PARASAIL_FLAG_SG_S2_BEG : 0;
    result->flag |= s2_end ? PARASAIL_FLAG_SG_S2_END : 0;

    parasail_free(pvGapper);
    parasail_free(pvH);
    parasail_free(pvHt);
    parasail_free(boundary);
    parasail_free(pvE);

    return result;
}

SG_IMPL_ALL
SG_IMPL_PROF_ALL


