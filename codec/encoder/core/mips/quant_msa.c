/*!
 * \copy
 *     Copyright (c)  2023, Cisco Systems
 *     All rights reserved.
 *
 *     Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions
 *     are met:
 *
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in
 *          the documentation and/or other materials provided with the
 *          distribution.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *     "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *     LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *     COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *     INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *     BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *     CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *     LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *     ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *     POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * \file    quant_msa.c
 *
 * \brief   Loongson optimization
 *
 * \date    04/04/2023 Created
 *
 *************************************************************************************
 */

#include <stdint.h>
#include "msa_macros.h"

#define MSA_MUH_H(in0, in1, out0)                \
{                                                \
  v8i16 sign = { 0 };                            \
  v4i32 in0r, in0l, in1r, in1l;                  \
  in0r = (v4i32)__msa_ilvr_h(sign, in0);         \
  in0l = (v4i32)__msa_ilvl_h(sign, in0);         \
  in1r = (v4i32)__msa_ilvr_h(sign, in1);         \
  in1l = (v4i32)__msa_ilvl_h(sign, in1);         \
  in0r = in0r * in1r;                            \
  in0l = in0l * in1l;                            \
  out0 = __msa_pckod_h((v8i16)in0l, (v8i16)in0r);\
}

#define MSA_QUANT_8(pdct, pff, pmf)                      \
{                                                        \
  v8i16 sign;                                            \
  sign = __msa_srai_h(pdct, 15);                         \
  pdct = (pdct ^ sign) - sign;                           \
  pdct = (v8i16)__msa_adds_u_h((v8u16)pdct, (v8u16)pff); \
  MSA_MUH_H(pdct, pmf, pdct);                            \
  pdct = (pdct ^ sign) - sign;                           \
}

#define MSA_QUANT_8_MAX(pdct, pff, pmf, max)             \
{                                                        \
  v8i16 sign;                                            \
  sign = __msa_srai_h(pdct, 15);                         \
  pdct = (pdct ^ sign) - sign;                           \
  pdct = (v8i16)__msa_adds_u_h((v8u16)pdct, (v8u16)pff); \
  MSA_MUH_H(pdct, pmf, pdct);                            \
  max = (v8i16)__msa_max_u_h((v8u16)max, (v8u16)pdct);   \
  pdct = (pdct ^ sign) - sign;                           \
}

#define MSA_SELECT_MAX_H(in0)                       \
{                                                   \
  v8u16 tmp = (v8u16)__msa_splati_d((v2i64)in0, 1); \
  tmp = __msa_max_u_h(tmp, (v8u16)in0);             \
  in0 = (v8i16)__msa_splati_w((v4i32)tmp, 1);       \
  tmp = __msa_max_u_h(tmp, (v8u16)in0);             \
  in0 = __msa_splati_h((v8i16)tmp, 1);              \
  in0 = (v8i16)__msa_max_u_h(tmp, (v8u16)in0);      \
}

void WelsQuant4x4_msa(int16_t *pDct, const int16_t* pFF, const int16_t *pMF) {
  v8i16 src0_r, src0_l, src1, src2;
  MSA_LD_V2(v8i16, pDct, 8, src0_r, src0_l);
  MSA_LD_V(v8i16, pFF, src1);
  MSA_LD_V(v8i16, pMF, src2);
  MSA_QUANT_8(src0_r, src1, src2);
  MSA_QUANT_8(src0_l, src1, src2);
  MSA_ST_V2(v8i16, src0_r, src0_l, pDct, 8);
}

void WelsQuant4x4Dc_msa(int16_t *pDct, const int16_t iFF, int16_t iMF) {
  v8i16 src0_r, src0_l, src1, src2;
  src1 = __msa_fill_h(iFF);
  src2 = __msa_fill_h(iMF);
  MSA_LD_V2(v8i16, pDct, 8, src0_r, src0_l);
  MSA_QUANT_8(src0_r, src1, src2);
  MSA_QUANT_8(src0_l, src1, src2);
  MSA_ST_V2(v8i16, src0_r, src0_l, pDct, 8);
}

void WelsQuantFour4x4_msa(int16_t *pDct, const int16_t* pFF, const int16_t *pMF) {
  v8i16 src0_r, src0_l, src1_r, src1_l, src2_r, src2_l;
  v8i16 src3_r, src3_l, src1, src2;
  MSA_LD_V(v8i16, pFF, src1);
  MSA_LD_V(v8i16, pMF, src2);
  MSA_LD_V8(v8i16, pDct, 8, src0_r, src0_l, src1_r, src1_l, src2_r, src2_l, src3_r, src3_l);
  MSA_QUANT_8(src0_r, src1, src2);
  MSA_QUANT_8(src0_l, src1, src2);
  MSA_QUANT_8(src1_r, src1, src2);
  MSA_QUANT_8(src1_l, src1, src2);
  MSA_QUANT_8(src2_r, src1, src2);
  MSA_QUANT_8(src2_l, src1, src2);
  MSA_QUANT_8(src3_r, src1, src2);
  MSA_QUANT_8(src3_l, src1, src2);
  MSA_ST_V8(v8i16, src0_r, src0_l, src1_r, src1_l, src2_r, src2_l, src3_r, src3_l, pDct, 8);
}

void WelsQuantFour4x4Max_msa(int16_t *pDct, const int16_t *pFF,
                             const int16_t *pMF, int16_t *pMax) {
  v8i16 src0_r, src0_l, src1_r, src1_l, src2_r, src2_l;
  v8i16 src3_r, src3_l, src1, src2;
  v8i16 max0 = { 0 };
  v8i16 max1, max2, max3;
  MSA_LD_V(v8i16, pFF, src1);
  MSA_LD_V(v8i16, pMF, src2);
  MSA_LD_V8(v8i16, pDct, 8, src0_r, src0_l, src1_r, src1_l, src2_r, src2_l, src3_r, src3_l);
  max1 = max2 = max3 = max0;
  MSA_QUANT_8_MAX(src0_r, src1, src2, max0);
  MSA_QUANT_8_MAX(src0_l, src1, src2, max0);
  MSA_QUANT_8_MAX(src1_r, src1, src2, max1);
  MSA_QUANT_8_MAX(src1_l, src1, src2, max1);
  MSA_QUANT_8_MAX(src2_r, src1, src2, max2);
  MSA_QUANT_8_MAX(src2_l, src1, src2, max2);
  MSA_QUANT_8_MAX(src3_r, src1, src2, max3);
  MSA_QUANT_8_MAX(src3_l, src1, src2, max3);
  MSA_SELECT_MAX_H(max0);
  MSA_SELECT_MAX_H(max1);
  MSA_SELECT_MAX_H(max2);
  MSA_SELECT_MAX_H(max3);
  max0 = __msa_ilvr_h(max1, max0);
  max2 = __msa_ilvr_h(max3, max2);
  max0 = (v8i16)__msa_ilvr_w((v4i32)max2, (v4i32)max0);
  MSA_ST_D(max0, 0, pMax);
  MSA_ST_V8(v8i16, src0_r, src0_l, src1_r, src1_l, src2_r, src2_l, src3_r, src3_l, pDct, 8);
}
