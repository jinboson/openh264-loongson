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
 * \file    dct_msa.c
 *
 * \brief   Loongson optimization
 *
 * \date    23/03/2023 Created
 *
 *************************************************************************************
 */
#include <stdint.h>
#include "msa_macros.h"

#define CALC_TEMPS_AND_PDCT(src0, src1, src2, src3, \
                            dst0, dst1, dst2, dst3) \
do {                                                \
  v8i16 tms0, tms1, tms2, tms3;                     \
  tms0 = __msa_addv_h((v8i16)src0, (v8i16)src3);    \
  tms1 = __msa_addv_h((v8i16)src1, (v8i16)src2);    \
  tms2 = __msa_subv_h((v8i16)src1, (v8i16)src2);    \
  tms3 = __msa_subv_h((v8i16)src0, (v8i16)src3);    \
  dst0 = (v16u8)__msa_addv_h(tms0, tms1);           \
  dst1 = (v16u8)__msa_slli_h(tms3, 1);              \
  dst1 = (v16u8)__msa_addv_h((v8i16)dst1, tms2);    \
  dst2 = (v16u8)__msa_subv_h(tms0, tms1);           \
  dst3 = (v16u8)__msa_slli_h(tms2, 1);              \
  dst3 = (v16u8)__msa_subv_h(tms3, (v8i16)dst3);    \
}while(0)

void WelsDctT4_msa (int16_t* pDct, uint8_t* pPixel1, int32_t iStride1, uint8_t* pPixel2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3, vec0, vec1, vec2, vec3;
  v16u8 dst0, dst1, dst2, dst3;

  MSA_LD_V4(v16u8, pPixel1, iStride1, src0, src1, src2, src3);
  MSA_LD_V4(v16u8, pPixel2, iStride2, vec0, vec1, vec2, vec3);
  MSA_ILVR_B4(v16u8, src0, vec0, src1, vec1, src2, vec2, src3, vec3, src0, src1, src2, src3);
  src0 = (v16u8)__msa_hsub_u_h(src0, src0);
  src1 = (v16u8)__msa_hsub_u_h(src1, src1);
  src2 = (v16u8)__msa_hsub_u_h(src2, src2);
  src3 = (v16u8)__msa_hsub_u_h(src3, src3);
  MSA_TRANSPOSE4x4_H(v16u8, src0, src1, src2, src3, vec0, vec1, vec2, vec3);
  CALC_TEMPS_AND_PDCT(vec0, vec1, vec2, vec3, dst0, dst1, dst2, dst3);
  MSA_TRANSPOSE4x4_H(v16u8, dst0, dst1, dst2, dst3, src0, src1, src2, src3);
  CALC_TEMPS_AND_PDCT(src0, src1, src2, src3, dst0, dst1, dst2, dst3);
  dst0 = (v16u8)__msa_ilvr_d((v2i64)dst1, (v2i64)dst0);
  dst2 = (v16u8)__msa_ilvr_d((v2i64)dst3, (v2i64)dst2);
  MSA_ST_V2(v16u8, dst0, dst2, pDct, 8);
}

void WelsDctFourT4_msa (int16_t* pDct, uint8_t* pPixel1, int32_t iStride1, uint8_t* pPixel2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
  v16u8 dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7;
  v16u8 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

  MSA_LD_V4(v16u8, pPixel1, iStride1, src0, src1, src2, src3);
  MSA_LD_V4(v16u8, pPixel2, iStride2, vec0, vec1, vec2, vec3);
  MSA_LD_V4(v16u8, pPixel1 + (iStride1 << 2), iStride1, src4, src5, src6, src7);
  MSA_LD_V4(v16u8, pPixel2 + (iStride2 << 2), iStride2, vec4, vec5, vec6, vec7);
  MSA_ILVR_B4(v16u8, src0, vec0, src1, vec1, src2, vec2, src3, vec3, src0, src1, src2, src3);
  MSA_ILVR_B4(v16u8, src4, vec4, src5, vec5, src6, vec6, src7, vec7, src4, src5, src6, src7);
  src0 = (v16u8)__msa_hsub_u_h(src0, src0);
  src1 = (v16u8)__msa_hsub_u_h(src1, src1);
  src2 = (v16u8)__msa_hsub_u_h(src2, src2);
  src3 = (v16u8)__msa_hsub_u_h(src3, src3);
  src4 = (v16u8)__msa_hsub_u_h(src4, src4);
  src5 = (v16u8)__msa_hsub_u_h(src5, src5);
  src6 = (v16u8)__msa_hsub_u_h(src6, src6);
  src7 = (v16u8)__msa_hsub_u_h(src7, src7);
  MSA_TRANSPOSE8x8_H(v16u8, src0, src1, src2, src3, src4, src5, src6, src7,
                     vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  CALC_TEMPS_AND_PDCT(vec0, vec1, vec2, vec3, dst0, dst1, dst2, dst3);
  CALC_TEMPS_AND_PDCT(vec4, vec5, vec6, vec7, dst4, dst5, dst6, dst7);
  MSA_TRANSPOSE8x8_H(v16u8, dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7,
                     src0, src1, src2, src3, src4, src5, src6, src7);
  CALC_TEMPS_AND_PDCT(src0, src1, src2, src3, dst0, dst1, dst2, dst3);
  CALC_TEMPS_AND_PDCT(src4, src5, src6, src7, dst4, dst5, dst6, dst7);
  tmp0 = (v16u8)__msa_ilvr_d((v2i64)dst1, (v2i64)dst0);
  tmp1 = (v16u8)__msa_ilvr_d((v2i64)dst3, (v2i64)dst2);
  tmp2 = (v16u8)__msa_ilvl_d((v2i64)dst1, (v2i64)dst0);
  tmp3 = (v16u8)__msa_ilvl_d((v2i64)dst3, (v2i64)dst2);
  tmp4 = (v16u8)__msa_ilvr_d((v2i64)dst5, (v2i64)dst4);
  tmp5 = (v16u8)__msa_ilvr_d((v2i64)dst7, (v2i64)dst6);
  tmp6 = (v16u8)__msa_ilvl_d((v2i64)dst5, (v2i64)dst4);
  tmp7 = (v16u8)__msa_ilvl_d((v2i64)dst7, (v2i64)dst6);
  MSA_ST_V8(v16u8, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, pDct, 8);
}

void WelsIDctFourT4Rec_msa (uint8_t* pRec, int32_t iStride, uint8_t* pPred, int32_t iPredStride, int16_t* pDct) {
  v8i16 src0, src1, src2, src3, src4, src5, src6, src7, tmp0, tmp1, tmp2, tmp3;
  v8i16 sumu, delu, sumd, deld, SumL, DelL, DelR, SumR;
  v8i16 zero = { 0 };
  MSA_LD_V4(v8i16, pDct, 8, src0, src2, src4, src6);
  src1 = __msa_splati_d(src0, 1);
  src3 = __msa_splati_d(src2, 1);
  src5 = __msa_splati_d(src4, 1);
  src7 = __msa_splati_d(src6, 1);
  MSA_TRANSPOSE4x4_H(v8i16, src0, src1, src2, src3, src0, src1, src2, src3);
  MSA_TRANSPOSE4x4_H(v8i16, src4, src5, src6, src7, src4, src5, src6, src7);
  MSA_ILVR_D4(v8i16, src4, src0, src5, src1, src6, src2, src7, src3, src0, src1, src2, src3);
  sumu = __msa_addv_h(src0, src2);
  delu = __msa_subv_h(src0, src2);
  tmp0 = __msa_srai_h(src3, 1);
  sumd = __msa_addv_h(src1, tmp0);
  tmp0 = __msa_srai_h(src1, 1);
  deld = __msa_subv_h(tmp0, src3);
  src0 = __msa_addv_h(sumu, sumd);
  src1 = __msa_addv_h(delu, deld);
  src2 = __msa_subv_h(delu, deld);
  src3 = __msa_subv_h(sumu, sumd);
  src4 = __msa_splati_d(src0, 1);
  src5 = __msa_splati_d(src1, 1);
  src6 = __msa_splati_d(src2, 1);
  src7 = __msa_splati_d(src3, 1);
  MSA_TRANSPOSE4x4_H(v8i16, src0, src1, src2, src3, src0, src1, src2, src3);
  MSA_TRANSPOSE4x4_H(v8i16, src4, src5, src6, src7, src4, src5, src6, src7);
  MSA_ILVR_D4(v8i16, src4, src0, src5, src1, src6, src2, src7, src3, src0, src1, src2, src3);
  SumL = __msa_addv_h(src0, src2);
  DelL = __msa_subv_h(src0, src2);
  tmp0 = __msa_srai_h(src1, 1);
  DelR = __msa_subv_h(tmp0, src3);
  tmp0 = __msa_srai_h(src3, 1);
  SumR = __msa_addv_h(src1, tmp0);
  MSA_LD_V4(v8i16, pPred, iPredStride, src0, src1, src2, src3);
  MSA_ILVR_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, src0, src1, src2, src3);
  tmp0 = __msa_addv_h(SumL, SumR);
  tmp1 = __msa_addv_h(DelL, DelR);
  tmp2 = __msa_subv_h(DelL, DelR);
  tmp3 = __msa_subv_h(SumL, SumR);
  MSA_SRARI_H4(v8i16, tmp0, tmp1, tmp2, tmp3, 6);
  src0 = __msa_adds_s_h(src0, tmp0);
  src1 = __msa_adds_s_h(src1, tmp1);
  src2 = __msa_adds_s_h(src2, tmp2);
  src3 = __msa_adds_s_h(src3, tmp3);
  MSA_CLIP_SH4_0_255(src0, src1, src2, src3);
  MSA_PCKEV_B2(v8i16, src1, src0, src3, src2, src0, src1);
  MSA_ST_D(src0, 0, pRec);
  MSA_ST_D(src0, 1, pRec + iStride);
  MSA_ST_D(src1, 0, pRec + iStride*2);
  MSA_ST_D(src1, 1, pRec + iStride*3);
  //iDstStridex4
  MSA_LD_V4(v8i16, pDct + 32, 8, src0, src2, src4, src6);
  src1 = __msa_splati_d(src0, 1);
  src3 = __msa_splati_d(src2, 1);
  src5 = __msa_splati_d(src4, 1);
  src7 = __msa_splati_d(src6, 1);
  MSA_TRANSPOSE4x4_H(v8i16, src0, src1, src2, src3, src0, src1, src2, src3);
  MSA_TRANSPOSE4x4_H(v8i16, src4, src5, src6, src7, src4, src5, src6, src7);
  MSA_ILVR_D4(v8i16, src4, src0, src5, src1, src6, src2, src7, src3, src0, src1, src2, src3);
  sumu = __msa_addv_h(src0, src2);
  delu = __msa_subv_h(src0, src2);
  tmp0 = __msa_srai_h(src3, 1);
  sumd = __msa_addv_h(src1, tmp0);
  tmp0 = __msa_srai_h(src1, 1);
  deld = __msa_subv_h(tmp0, src3);
  src0 = __msa_addv_h(sumu, sumd);
  src1 = __msa_addv_h(delu, deld);
  src2 = __msa_subv_h(delu, deld);
  src3 = __msa_subv_h(sumu, sumd);
  src4 = __msa_splati_d(src0, 1);
  src5 = __msa_splati_d(src1, 1);
  src6 = __msa_splati_d(src2, 1);
  src7 = __msa_splati_d(src3, 1);
  MSA_TRANSPOSE4x4_H(v8i16, src0, src1, src2, src3, src0, src1, src2, src3);
  MSA_TRANSPOSE4x4_H(v8i16, src4, src5, src6, src7, src4, src5, src6, src7);
  MSA_ILVR_D4(v8i16, src4, src0, src5, src1, src6, src2, src7, src3, src0, src1, src2, src3);
  SumL = __msa_addv_h(src0, src2);
  DelL = __msa_subv_h(src0, src2);
  tmp0 = __msa_srai_h(src1, 1);
  DelR = __msa_subv_h(tmp0, src3);
  tmp0 = __msa_srai_h(src3, 1);
  SumR = __msa_addv_h(src1, tmp0);
  MSA_LD_V4(v8i16, pPred + iPredStride*4, iPredStride, src0, src1, src2, src3);
  MSA_ILVR_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, src0, src1, src2, src3);
  tmp0 = __msa_addv_h(SumL, SumR);
  tmp1 = __msa_addv_h(DelL, DelR);
  tmp2 = __msa_subv_h(DelL, DelR);
  tmp3 = __msa_subv_h(SumL, SumR);
  MSA_SRARI_H4(v8i16, tmp0, tmp1, tmp2, tmp3, 6);
  src0 = __msa_adds_s_h(src0, tmp0);
  src1 = __msa_adds_s_h(src1, tmp1);
  src2 = __msa_adds_s_h(src2, tmp2);
  src3 = __msa_adds_s_h(src3, tmp3);
  MSA_CLIP_SH4_0_255(src0, src1, src2, src3);
  MSA_PCKEV_B2(v8i16, src1, src0, src3, src2, src0, src1);
  pRec += iStride*4;
  MSA_ST_D(src0, 0, pRec);
  MSA_ST_D(src0, 1, pRec + iStride);
  MSA_ST_D(src1, 0, pRec + iStride*2);
  MSA_ST_D(src1, 1, pRec + iStride*3);
}
