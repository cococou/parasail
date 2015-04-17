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

%(HEADER)s

#include "parasail.h"
#include "parasail_internal.h"
#include "parasail_internal_%(ISA)s.h"
#include "blosum/blosum_map.h"

#define NEG_INF %(NEG_INF)s
%(FIXES)s

#ifdef PARASAIL_TABLE
static inline void arr_store_si%(BITS)s(
        int *array,
        %(VTYPE)s vH,
        int32_t t,
        int32_t seglen,
        int32_t d,
        int32_t dlen)
{
%(PRINTER)s
}
#endif

#ifdef PARASAIL_TABLE
#define FNAME %(NAME_TABLE)s
#else
#define FNAME %(NAME)s
#endif

parasail_result_t* FNAME(
        const char * const restrict s1, const int s1Len,
        const char * const restrict s2, const int s2Len,
        const int open, const int gap, const int matrix[24][24])
{
    int32_t i = 0;
    int32_t j = 0;
    int32_t k = 0;
    int32_t segNum = 0;
    const int32_t n = 24; /* number of amino acids in table */
    const int32_t segWidth = %(LANES)s; /* number of values in vector unit */
    const int32_t segLen = (s1Len + segWidth - 1) / segWidth;
    const int32_t offset = (s1Len - 1) %% segLen;
    const int32_t position = (segWidth - 1) - (s1Len - 1) / segLen;
    %(VTYPE)s* const restrict vProfile  = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, n * segLen);
    %(VTYPE)s* const restrict vProfileM = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, n * segLen);
    %(VTYPE)s* const restrict vProfileS = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, n * segLen);
    %(VTYPE)s* restrict pvHStore        = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* restrict pvHLoad         = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* restrict pvHMStore       = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* restrict pvHMLoad        = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* restrict pvHSStore       = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* restrict pvHSLoad        = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* restrict pvHLStore       = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* restrict pvHLLoad        = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* restrict pvEStore        = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* restrict pvELoad         = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* const restrict pvEM      = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* const restrict pvES      = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s* const restrict pvEL      = parasail_memalign_%(VTYPE)s(%(ALIGNMENT)s, segLen);
    %(VTYPE)s vGapO = %(VSET1)s(open);
    %(VTYPE)s vGapE = %(VSET1)s(gap);
    %(VTYPE)s vNegInf = %(VSET1)s(NEG_INF);
    %(VTYPE)s vZero = %(VSET0)s();
    %(VTYPE)s vOne = %(VSET1)s(1);
    %(INT)s score = NEG_INF;
    %(INT)s matches = NEG_INF;
    %(INT)s similar = NEG_INF;
    %(INT)s length = NEG_INF;
    %(STATS_SATURATION_CHECK_INIT)s
    %(VTYPE)s vMaxH = vNegInf;
    %(VTYPE)s vMaxHM = vNegInf;
    %(VTYPE)s vMaxHS = vNegInf;
    %(VTYPE)s vMaxHL = vNegInf;
#ifdef PARASAIL_TABLE
    parasail_result_t *result = parasail_result_new_table3(segLen*segWidth, s2Len);
#else
    parasail_result_t *result = parasail_result_new();
#endif

    parasail_memset_%(VTYPE)s(pvHMStore, vZero, segLen);
    parasail_memset_%(VTYPE)s(pvHSStore, vZero, segLen);
    parasail_memset_%(VTYPE)s(pvHLStore, vZero, segLen);

    /* Generate query profile.
     * Rearrange query sequence & calculate the weight of match/mismatch.
     * Don't alias. */
    {
        int32_t index = 0;
        for (k=0; k<n; ++k) {
            for (i=0; i<segLen; ++i) {
                %(VTYPE)s_%(WIDTH)s_t p;
                %(VTYPE)s_%(WIDTH)s_t m;
                %(VTYPE)s_%(WIDTH)s_t s;
                j = i;
                for (segNum=0; segNum<segWidth; ++segNum) {
                    p.v[segNum] = j >= s1Len ? 0 : matrix[k][MAP_BLOSUM_[(unsigned char)s1[j]]];
                    m.v[segNum] = j >= s1Len ? 0 : (k == MAP_BLOSUM_[(unsigned char)s1[j]]);
                    s.v[segNum] = p.v[segNum] > 0;
                    j += segLen;
                }
                %(VSTORE)s(&vProfile[index], p.m);
                %(VSTORE)s(&vProfileM[index], m.m);
                %(VSTORE)s(&vProfileS[index], s.m);
                ++index;
            }
        }
    }

    /* initialize H and E */
    {
        int32_t index = 0;
        for (i=0; i<segLen; ++i) {
            %(VTYPE)s_%(WIDTH)s_t h;
            %(VTYPE)s_%(WIDTH)s_t e;
            for (segNum=0; segNum<segWidth; ++segNum) {
                h.v[segNum] = 0;
                e.v[segNum] = -open;
            }
            %(VSTORE)s(&pvHStore[index], h.m);
            %(VSTORE)s(&pvEStore[index], e.m);
            ++index;
        }
    }

    /* outer loop over database sequence */
    for (j=0; j<s2Len; ++j) {
        %(VTYPE)s vE;
        %(VTYPE)s vEM;
        %(VTYPE)s vES;
        %(VTYPE)s vEL;
        %(VTYPE)s vF;
        %(VTYPE)s vFM;
        %(VTYPE)s vFS;
        %(VTYPE)s vFL;
        %(VTYPE)s vH;
        %(VTYPE)s vHM;
        %(VTYPE)s vHS;
        %(VTYPE)s vHL;
        const %(VTYPE)s* vP = NULL;
        const %(VTYPE)s* vPM = NULL;
        const %(VTYPE)s* vPS = NULL;
        %(VTYPE)s* pv = NULL;

        /* Initialize F value to neg inf.  Any errors to vH values will
         * be corrected in the Lazy_F loop.  */
        vF = vNegInf;
        vFM = vZero;
        vFS = vZero;
        vFL = vZero;

        /* load final segment of pvHStore and shift left by 2 bytes */
        vH = %(VSHIFT)s(pvHStore[segLen - 1], %(BYTES)s);
        vHM = %(VSHIFT)s(pvHMStore[segLen - 1], %(BYTES)s);
        vHS = %(VSHIFT)s(pvHSStore[segLen - 1], %(BYTES)s);
        vHL = %(VSHIFT)s(pvHLStore[segLen - 1], %(BYTES)s);

        /* Correct part of the vProfile */
        vP = vProfile + MAP_BLOSUM_[(unsigned char)s2[j]] * segLen;
        vPM = vProfileM + MAP_BLOSUM_[(unsigned char)s2[j]] * segLen;
        vPS = vProfileS + MAP_BLOSUM_[(unsigned char)s2[j]] * segLen;

        /* Swap the 2 H buffers. */
        pv = pvHLoad;
        pvHLoad = pvHStore;
        pvHStore = pv;
        pv = pvHMLoad;
        pvHMLoad = pvHMStore;
        pvHMStore = pv;
        pv = pvHSLoad;
        pvHSLoad = pvHSStore;
        pvHSStore = pv;
        pv = pvHLLoad;
        pvHLLoad = pvHLStore;
        pvHLStore = pv;
        pv = pvELoad;
        pvELoad = pvEStore;
        pvEStore = pv;

        /* inner loop to process the query sequence */
        for (i=0; i<segLen; ++i) {
            %(VTYPE)s case1not;
            %(VTYPE)s case2not;
            %(VTYPE)s case2;
            %(VTYPE)s case3;

            vH = %(VADD)s(vH, %(VLOAD)s(vP + i));
            vE = %(VLOAD)s(pvELoad + i);

            /* determine which direction of length and match to
             * propagate, before vH is finished calculating */
            case1not = %(VOR)s(
                    %(VCMPLT)s(vH,vF),%(VCMPLT)s(vH,vE));
            case2not = %(VCMPLT)s(vF,vE);
            case2 = %(VANDNOT)s(case2not,case1not);
            case3 = %(VAND)s(case1not,case2not);

            /* Get max from vH, vE and vF. */
            vH = %(VMAX)s(vH, vE);
            vH = %(VMAX)s(vH, vF);
            /* Save vH values. */
            %(VSTORE)s(pvHStore + i, vH);

            /* calculate vM */
            vEM = %(VLOAD)s(pvEM + i);
            vHM = %(VBLEND)s(
                    %(VADD)s(vHM, %(VLOAD)s(vPM + i)),
                    %(VOR)s(
                        %(VAND)s(case2, vFM),
                        %(VAND)s(case3, vEM)),
                    case1not);
            %(VSTORE)s(pvHMStore + i, vHM);

            /* calculate vS */
            vES = %(VLOAD)s(pvES + i);
            vHS = %(VBLEND)s(
                    %(VADD)s(vHS, %(VLOAD)s(vPS + i)),
                    %(VOR)s(
                        %(VAND)s(case2, vFS),
                        %(VAND)s(case3, vES)),
                    case1not);
            %(VSTORE)s(pvHSStore + i, vHS);

            /* calculate vL */
            vEL = %(VLOAD)s(pvEL + i);
            vHL = %(VBLEND)s(
                    %(VADD)s(vHL, vOne),
                    %(VOR)s(
                        %(VAND)s(case2, %(VADD)s(vFL, vOne)),
                        %(VAND)s(case3, %(VADD)s(vEL, vOne))),
                    case1not);
            %(VSTORE)s(pvHLStore + i, vHL);
            %(STATS_SATURATION_CHECK_MID)s
#ifdef PARASAIL_TABLE
            arr_store_si%(BITS)s(result->matches_table, vHM, i, segLen, j, s2Len);
            arr_store_si%(BITS)s(result->similar_table, vHS, i, segLen, j, s2Len);
            arr_store_si%(BITS)s(result->length_table, vHL, i, segLen, j, s2Len);
            arr_store_si%(BITS)s(result->score_table, vH, i, segLen, j, s2Len);
#endif

            /* Update vE value. */
            vH = %(VSUB)s(vH, vGapO);
            vE = %(VSUB)s(vE, vGapE);
            vE = %(VMAX)s(vE, vH);
            %(VSTORE)s(pvEStore + i, vE);
            %(VSTORE)s(pvEM + i, vHM);
            %(VSTORE)s(pvES + i, vHS);
            %(VSTORE)s(pvEL + i, vHL);

            /* Update vF value. */
            vF = %(VSUB)s(vF, vGapE);
            vF = %(VMAX)s(vF, vH);
            vFM = vHM;
            vFS = vHS;
            vFL = vHL;

            /* Load the next vH. */
            vH = %(VLOAD)s(pvHLoad + i);
            vHM = %(VLOAD)s(pvHMLoad + i);
            vHS = %(VLOAD)s(pvHSLoad + i);
            vHL = %(VLOAD)s(pvHLLoad + i);
        }

        /* Lazy_F loop: has been revised to disallow adjecent insertion and
         * then deletion, so don't update E(i, i), learn from SWPS3 */
        for (k=0; k<segWidth; ++k) {
            %(VTYPE)s vHp = %(VSHIFT)s(pvHLoad[segLen - 1], %(BYTES)s);
            vF = %(VSHIFT)s(vF, %(BYTES)s);
            vF = %(VINSERT)s(vF, -open, 0);
            vFM = %(VSHIFT)s(vFM, %(BYTES)s);
            vFS = %(VSHIFT)s(vFS, %(BYTES)s);
            vFL = %(VSHIFT)s(vFL, %(BYTES)s);
            for (i=0; i<segLen; ++i) {
                %(VTYPE)s case1not;
                %(VTYPE)s case2not;
                %(VTYPE)s case2;

                /* need to know where match and length come from so
                 * recompute the cases as in the main loop */
                vHp = %(VADD)s(vHp, %(VLOAD)s(vP + i));
                vE = %(VLOAD)s(pvELoad + i);
                case1not = %(VOR)s(
                        %(VCMPLT)s(vHp,vF),%(VCMPLT)s(vHp,vE));
                case2not = %(VCMPLT)s(vF,vE);
                case2 = %(VANDNOT)s(case2not,case1not);

                vHM = %(VLOAD)s(pvHMStore + i);
                vHM = %(VBLEND)s(vHM, vFM, case2);
                %(VSTORE)s(pvHMStore + i, vHM);
                %(VSTORE)s(pvEM + i, vHM);

                vHS = %(VLOAD)s(pvHSStore + i);
                vHS = %(VBLEND)s(vHS, vFS, case2);
                %(VSTORE)s(pvHSStore + i, vHS);
                %(VSTORE)s(pvES + i, vHS);

                vHL = %(VLOAD)s(pvHLStore + i);
                vHL = %(VBLEND)s(vHL, %(VADD)s(vFL,vOne), case2);
                %(VSTORE)s(pvHLStore + i, vHL);
                %(VSTORE)s(pvEL + i, vHL);

                vH = %(VLOAD)s(pvHStore + i);
                vH = %(VMAX)s(vH,vF);
                %(VSTORE)s(pvHStore + i, vH);
#ifdef PARASAIL_TABLE
                arr_store_si%(BITS)s(result->matches_table, vHM, i, segLen, j, s2Len);
                arr_store_si%(BITS)s(result->similar_table, vHS, i, segLen, j, s2Len);
                arr_store_si%(BITS)s(result->length_table, vHL, i, segLen, j, s2Len);
                arr_store_si%(BITS)s(result->score_table, vH, i, segLen, j, s2Len);
#endif
                vH = %(VSUB)s(vH, vGapO);
                vF = %(VSUB)s(vF, vGapE);
                if (! %(VMOVEMASK)s(%(VCMPGT)s(vF, vH))) goto end;
                /*vF = %(VMAX)s(vF, vH);*/
                vFM = vHM;
                vFS = vHS;
                vFL = vHL;
                vHp = %(VLOAD)s(pvHLoad + i);
            }
        }
end:
        {
            /* extract vector containing last value from the column */
            %(VTYPE)s cond_max;
            vH = %(VLOAD)s(pvHStore + offset);
            vHM = %(VLOAD)s(pvHMStore + offset);
            vHS = %(VLOAD)s(pvHSStore + offset);
            vHL = %(VLOAD)s(pvHLStore + offset);
            cond_max = %(VCMPGT)s(vH, vMaxH);
            vMaxH = %(VBLEND)s(vMaxH, vH, cond_max);
            vMaxHM = %(VBLEND)s(vMaxHM, vHM, cond_max);
            vMaxHS = %(VBLEND)s(vMaxHS, vHS, cond_max);
            vMaxHL = %(VBLEND)s(vMaxHL, vHL, cond_max);
        }
    }

    {
        /* extract last value from the column */
        %(INT)s tmp;
        for (k=0; k<position; ++k) {
            vMaxH  = %(VSHIFT)s (vMaxH, %(BYTES)s);
            vMaxHM = %(VSHIFT)s (vMaxHM, %(BYTES)s);
            vMaxHS = %(VSHIFT)s (vMaxHS, %(BYTES)s);
            vMaxHL = %(VSHIFT)s (vMaxHL, %(BYTES)s);
        }
        tmp = (%(INT)s) %(VEXTRACT)s (vMaxH, %(LAST_POS)s);
        if (tmp > score) {
            score = tmp;
            matches = (%(INT)s)%(VEXTRACT)s(vMaxHM, %(LAST_POS)s);
            similar = (%(INT)s)%(VEXTRACT)s(vMaxHS, %(LAST_POS)s);
            length = (%(INT)s)%(VEXTRACT)s(vMaxHL, %(LAST_POS)s);
        }
    }

    /* max of last column */
    {
        vMaxH = vNegInf;
        vMaxHM = vNegInf;
        vMaxHS = vNegInf;
        vMaxHL = vNegInf;

        for (i=0; i<segLen; ++i) {
            /* load the last stored values */
            %(VTYPE)s vH = %(VLOAD)s(pvHStore + i);
            %(VTYPE)s vHM = %(VLOAD)s(pvHMStore + i);
            %(VTYPE)s vHS = %(VLOAD)s(pvHSStore + i);
            %(VTYPE)s vHL = %(VLOAD)s(pvHLStore + i);
            %(VTYPE)s cond_max = %(VCMPGT)s(vH, vMaxH);
            vMaxH = %(VBLEND)s(vMaxH, vH, cond_max);
            vMaxHM = %(VBLEND)s(vMaxHM, vHM, cond_max);
            vMaxHS = %(VBLEND)s(vMaxHS, vHS, cond_max);
            vMaxHL = %(VBLEND)s(vMaxHL, vHL, cond_max);
        }

        /* max in vec */
        for (j=0; j<segWidth; ++j) {
            %(INT)s value = (%(INT)s) %(VEXTRACT)s(vMaxH, %(LAST_POS)s);
            if (value > score) {
                score = value;
                matches = (%(INT)s)%(VEXTRACT)s(vMaxHM, %(LAST_POS)s);
                similar = (%(INT)s)%(VEXTRACT)s(vMaxHS, %(LAST_POS)s);
                length = (%(INT)s)%(VEXTRACT)s(vMaxHL, %(LAST_POS)s);
            }
            vMaxH = %(VSHIFT)s(vMaxH, %(BYTES)s);
            vMaxHM = %(VSHIFT)s(vMaxHM, %(BYTES)s);
            vMaxHS = %(VSHIFT)s(vMaxHS, %(BYTES)s);
            vMaxHL = %(VSHIFT)s(vMaxHL, %(BYTES)s);
        }
    }

    %(STATS_SATURATION_CHECK_FINAL)s

    result->score = score;
    result->matches = matches;
    result->similar = similar;
    result->length = length;

    parasail_free(pvEL);
    parasail_free(pvES);
    parasail_free(pvEM);
    parasail_free(pvELoad);
    parasail_free(pvEStore);
    parasail_free(pvHLLoad);
    parasail_free(pvHLStore);
    parasail_free(pvHSLoad);
    parasail_free(pvHSStore);
    parasail_free(pvHMLoad);
    parasail_free(pvHMStore);
    parasail_free(pvHLoad);
    parasail_free(pvHStore);
    parasail_free(vProfileS);
    parasail_free(vProfileM);
    parasail_free(vProfile);

    return result;
}
