/*!
 *************************************************************************************
 * Copyright (c) 2022 Loongson Technology Corporation Limited
 * Contributed by jinbo <jinbo@loongson.cn>
 *
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
 * \file    mc_horver_msa.c
 *
 * \brief   Loongson optimization
 *
 * \date    4/3/2023 Created
 *
 *************************************************************************************
 */

#include <stdint.h>
#include "msa_macros.h"

#define FILTER_INPUT_8BIT(_in0, _in1, _in2, _in3, \
                          _in4, _in5, _out0)      \
do {                                              \
  v8i16 _tmp;                                     \
  _in0  = __msa_addv_h(_in0, _in5);               \
  _in1  = __msa_addv_h(_in1, _in4);               \
  _in2  = __msa_addv_h(_in2, _in3);               \
  _tmp  = __msa_slli_h(_in1, 2);                  \
  _in1  = __msa_addv_h(_tmp, _in1);               \
  _in0  = __msa_subv_h(_in0, _in1);               \
  _tmp  = __msa_slli_h(_in2, 4);                  \
  _in0  = __msa_addv_h(_in0, _tmp);               \
  _tmp  = __msa_slli_h(_in2, 2);                  \
  _out0 = __msa_addv_h(_in0, _tmp);               \
}while(0)

#define HOR_FILTER_INPUT_16BIT(_in0, _in1, _in2, _in3, \
                               _in4, _in5, _out0)      \
do {                                                   \
  v4i32 _pi05, _pi14, _pi23, _temp;                    \
  _pi05 = __msa_addv_w((v4i32)_in0, (v4i32)_in5);      \
  _pi14 = __msa_addv_w((v4i32)_in1, (v4i32)_in4);      \
  _pi23 = __msa_addv_w((v4i32)_in2, (v4i32)_in3);      \
  _temp = __msa_slli_w(_pi14, 2);                      \
  _pi14 = __msa_addv_w(_temp, _pi14);                  \
  _pi05 = __msa_subv_w(_pi05, _pi14);                  \
  _temp = __msa_slli_w(_pi23, 4);                      \
  _pi05 = __msa_addv_w(_pi05, _temp);                  \
  _temp = __msa_slli_w(_pi23, 2);                      \
  _out0 = (v8i16)__msa_addv_w(_pi05, _temp);           \
}while(0)

#define UNPCK_R_W_H(in0, out0)         \
do {                                   \
  v8i16 sign = __msa_clti_s_h(in0, 0); \
  out0 = __msa_ilvr_h(sign, in0);      \
}while(0)

#define UNPCK_L_W_H(in0, out0)         \
do {                                   \
  v8i16 sign = __msa_clti_s_h(in0, 0); \
  out0 = __msa_ilvl_h(sign, in0);      \
}while(0)

void PixelAvgWidthEq4_msa(uint8_t *pDst, int32_t iDstStride, const uint8_t *pSrcA,
                          int32_t iSrcAStride, const uint8_t *pSrcB, int32_t iSrcBStride,
                          int32_t iHeight ) {
  int32_t i;
  v16u8 src0, src1;
  for (i = 0; i < iHeight; i++) {
    MSA_LD_V(v16u8, pSrcA, src0);
    MSA_LD_V(v16u8, pSrcB, src1);
    pSrcA += iSrcAStride;
    pSrcB += iSrcBStride;
    src0 = __msa_aver_u_b(src0, src1);
    MSA_ST_W(src0, 0, pDst);
    pDst  += iDstStride;
  }
}

void PixelAvgWidthEq8_msa(uint8_t *pDst, int32_t iDstStride, const uint8_t *pSrcA,
                          int32_t iSrcAStride, const uint8_t *pSrcB, int32_t iSrcBStride,
                          int32_t iHeight ) {
  int32_t i;
  int32_t strideA = iSrcAStride << 1;
  int32_t strideB = iSrcBStride << 1;
  int32_t strideC = iDstStride << 1;
  v16u8 src0, src1, src2, src3;
  for (i = 0; i < iHeight; i += 2) {
    MSA_LD_V2(v16u8, pSrcA, iSrcAStride, src0, src1);
    MSA_LD_V2(v16u8, pSrcB, iSrcBStride, src2, src3);
    src0 = __msa_aver_u_b(src0, src2);
    src1 = __msa_aver_u_b(src1, src3);
    MSA_ST_D(src0, 0, pDst);
    MSA_ST_D(src1, 0, pDst + iDstStride);
    pSrcA += strideA;
    pSrcB += strideB;
    pDst  += strideC;
  }
}

void PixelAvgWidthEq16_msa(uint8_t *pDst, int32_t iDstStride, const uint8_t *pSrcA,
                           int32_t iSrcAStride, const uint8_t *pSrcB, int32_t iSrcBStride,
                           int32_t iHeight ) {
  int32_t i;
  int32_t strideA = iSrcAStride << 2;
  int32_t strideB = iSrcBStride << 2;
  int32_t strideC = iDstStride << 2;
  v16u8 src0, src1, src2, src3;
  v16u8 src4, src5, src6, src7;
  for (i = 0; i < iHeight; i += 4) {
    MSA_LD_V4(v16u8, pSrcA, iSrcAStride, src0, src1, src2, src3);
    MSA_LD_V4(v16u8, pSrcB, iSrcBStride, src4, src5, src6, src7);
    src0 = __msa_aver_u_b(src0, src4);
    src1 = __msa_aver_u_b(src1, src5);
    src2 = __msa_aver_u_b(src2, src6);
    src3 = __msa_aver_u_b(src3, src7);
    MSA_ST_V4(v16u8, src0, src1, src2, src3, pDst, iDstStride);
    pSrcA += strideA;
    pSrcB += strideB;
    pDst  += strideC;
  }
}

void McHorVer02WidthEq8_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                            int32_t iDstStride, int32_t iHeight) {
  int32_t iStride2 = iSrcStride << 1;
  uint8_t *psrc = (uint8_t*)pSrc;
  v8i16 zero = { 0 };
  v8i16 src0, src1, src2, src3, src4, src5;
  for (int i = 0; i < iHeight; i++) {
    MSA_LD_V4(v8i16, psrc - iStride2, iSrcStride, src0, src1, src2, src3);
    MSA_LD_V2(v8i16, psrc + iStride2, iSrcStride, src4, src5);
    MSA_ILVR_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, src0, src1, src2, src3);
    MSA_ILVR_B2(v8i16, zero, src4, zero, src5, src4, src5);
    FILTER_INPUT_8BIT(src0, src1, src2, src3 ,src4, src5 ,src0);
    src0 = __msa_srari_h(src0, 5);
    MSA_CLIP_SH_0_255(src0);
    src0 = (v8i16)__msa_pckev_b((v16i8)src0, (v16i8)src0);
    MSA_ST_D(src0, 0, pDst);
    pDst += iDstStride;
    psrc += iSrcStride;
  }
}

void McHorVer02WidthEq16_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                             int32_t iDstStride, int32_t iHeight) {
  int32_t iStride2 = iSrcStride << 1;
  uint8_t *psrc = (uint8_t*)pSrc;
  v8i16 zero = { 0 };
  v8i16 src0, src1, src2, src3, src4, src5;
  v8i16 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_l, out0_h;
  for (int i = 0; i < iHeight; i++) {
    MSA_LD_V4(v8i16, psrc - iStride2, iSrcStride, src0, src1, src2, src3);
    MSA_LD_V2(v8i16, psrc + iStride2, iSrcStride, src4, src5);
    //r part
    MSA_ILVR_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, tmp0, tmp1, tmp2, tmp3);
    MSA_ILVR_B2(v8i16, zero, src4, zero, src5, tmp4, tmp5);
    FILTER_INPUT_8BIT(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_l);
    out0_l = __msa_srari_h(out0_l, 5);
    MSA_CLIP_SH_0_255(out0_l);
    //l part
    MSA_ILVL_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, tmp0, tmp1, tmp2, tmp3);
    MSA_ILVL_B2(v8i16, zero, src4, zero, src5, tmp4, tmp5);
    FILTER_INPUT_8BIT(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_h);
    out0_h = __msa_srari_h(out0_h, 5);
    MSA_CLIP_SH_0_255(out0_h);
    out0_l = (v8i16)__msa_pckev_b((v16i8)out0_h, (v16i8)out0_l);
    MSA_ST_V(v8i16, out0_l, pDst);
    pDst += iDstStride;
    psrc += iSrcStride;
  }
}

void McHorVer20WidthEq4_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                            int32_t iDstStride, int32_t iHeight) {
  uint8_t *psrc = (uint8_t*)pSrc -2;
  v8i16 zero = { 0 };
  v8i16 src0, src1, src2, src3, src4, src5;
  for (int i = 0; i < iHeight; i++) {
    MSA_LD_V(v8i16, psrc, src0);
    src1 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 1);
    src2 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 2);
    src3 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 3);
    src4 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 4);
    src5 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 5);
    MSA_ILVR_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, src0, src1, src2, src3);
    MSA_ILVR_B2(v8i16, zero, src4, zero, src5, src4, src5);
    FILTER_INPUT_8BIT(src0, src1, src2, src3 ,src4, src5 ,src0);
    src0 = __msa_srari_h(src0, 5);
    MSA_CLIP_SH_0_255(src0);
    src0 = (v8i16)__msa_pckev_b((v16i8)src0, (v16i8)src0);
    MSA_ST_W(src0, 0, pDst);
    pDst += iDstStride;
    psrc += iSrcStride;
  }
}

void McHorVer20WidthEq5_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                            int32_t iDstStride, int32_t iHeight) {
  uint8_t *psrc = (uint8_t*)pSrc -2;
  v8i16 zero = { 0 };
  v8i16 src0, src1, src2, src3, src4, src5;
  for (int i = 0; i < iHeight; i++) {
    MSA_LD_V(v8i16, psrc, src0);
    src1 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 1);
    src2 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 2);
    src3 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 3);
    src4 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 4);
    src5 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 5);
    MSA_ILVR_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, src0, src1, src2, src3);
    MSA_ILVR_B2(v8i16, zero, src4, zero, src5, src4, src5);
    FILTER_INPUT_8BIT(src0, src1, src2, src3 ,src4, src5 ,src0);
    src0 = __msa_srari_h(src0, 5);
    MSA_CLIP_SH_0_255(src0);
    src0 = (v8i16)__msa_pckev_b((v16i8)src0, (v16i8)src0);
    MSA_ST_W(src0, 0, pDst);
    *(pDst + 4) = __msa_copy_u_b((v16i8)src0, 4);
    pDst += iDstStride;
    psrc += iSrcStride;
  }
}

void McHorVer20WidthEq8_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                            int32_t iDstStride, int32_t iHeight) {
  uint8_t *psrc = (uint8_t*)pSrc -2;
  v8i16 zero = { 0 };
  v8i16 src0, src1, src2, src3, src4, src5;
  for (int i = 0; i < iHeight; i++) {
    MSA_LD_V(v8i16, psrc, src0);
    src1 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 1);
    src2 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 2);
    src3 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 3);
    src4 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 4);
    src5 = (v8i16)__msa_sldi_b((v16i8)src0, (v16i8)src0, 5);
    MSA_ILVR_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, src0, src1, src2, src3);
    MSA_ILVR_B2(v8i16, zero, src4, zero, src5, src4, src5);
    FILTER_INPUT_8BIT(src0, src1, src2, src3 ,src4, src5 ,src0);
    src0 = __msa_srari_h(src0, 5);
    MSA_CLIP_SH_0_255(src0);
    src0 = (v8i16)__msa_pckev_b((v16i8)src0, (v16i8)src0);
    MSA_ST_D(src0, 0, pDst);
    pDst += iDstStride;
    psrc += iSrcStride;
  }
}

void McHorVer20WidthEq9_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                            int32_t iDstStride, int32_t iHeight) {
  McHorVer20WidthEq4_msa(pSrc, iSrcStride, pDst, iDstStride, iHeight);
  McHorVer20WidthEq5_msa(&pSrc[4], iSrcStride, &pDst[4], iDstStride, iHeight);
}

void McHorVer20WidthEq16_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                             int32_t iDstStride, int32_t iHeight) {
  uint8_t *psrc = (uint8_t*)pSrc - 2;
  v8i16 zero = { 0 };
  v8i16 src0, src1, src2, src3, src4, src5;
  v8i16 tmp0, tmp1, tmp2 ,tmp3 ,tmp4, tmp5, out0_l, out0_h;
  for (int i = 0; i < iHeight; i++) {
    MSA_LD_V4(v8i16, psrc, 1, src0, src1, src2, src3);
    MSA_LD_V2(v8i16, psrc + 4, 1, src4, src5);
    //r part
    MSA_ILVR_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, tmp0, tmp1, tmp2, tmp3);
    MSA_ILVR_B2(v8i16, zero, src4, zero, src5, tmp4, tmp5);
    FILTER_INPUT_8BIT(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_l);
    out0_l = __msa_srari_h(out0_l, 5);
    MSA_CLIP_SH_0_255(out0_l);
    //l part
    MSA_ILVL_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, tmp0, tmp1, tmp2, tmp3);
    MSA_ILVL_B2(v8i16, zero, src4, zero, src5, tmp4, tmp5);
    FILTER_INPUT_8BIT(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_h);
    out0_h = __msa_srari_h(out0_h, 5);
    MSA_CLIP_SH_0_255(out0_h);
    out0_l = (v8i16)__msa_pckev_b((v16i8)out0_h, (v16i8)out0_l);
    MSA_ST_V(v8i16, out0_l, pDst);
    pDst += iDstStride;
    psrc += iSrcStride;
  }
}

void McHorVer20WidthEq17_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                             int32_t iDstStride, int32_t iHeight) {
  McHorVer20WidthEq8_msa(pSrc, iSrcStride, pDst, iDstStride, iHeight);
  McHorVer20WidthEq9_msa(&pSrc[8], iSrcStride, &pDst[8], iDstStride, iHeight);
}

void McHorVer22WidthEq8_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                            int32_t iDstStride, int32_t iHeight) {
  int32_t iStride2 = iSrcStride << 1;
  uint8_t *psrc = (uint8_t*)pSrc - 2;
  v8i16 src0, src1, src2, src3, src4, src5;
  v8i16 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_l, out0_h;
  v8i16 dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7;
  v8i16 mask1 = {3, 4, 5, 6, 7, 8, 9, 10};
  v8i16 mask2 = {6, 7, 8, 9, 10, 11, 12 ,13};
  v8i16 zero = { 0 };
  for (int i = 0; i < iHeight; i++) {
    MSA_LD_V4(v8i16, psrc - iStride2, iSrcStride, src0, src1, src2, src3);
    MSA_LD_V2(v8i16, psrc + iStride2, iSrcStride, src4, src5);
    //r part
    MSA_ILVR_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, tmp0, tmp1, tmp2, tmp3);
    MSA_ILVR_B2(v8i16, zero, src4, zero, src5, tmp4, tmp5);
    FILTER_INPUT_8BIT(tmp0, tmp1 ,tmp2, tmp3, tmp4, tmp5, out0_l);
    //l part
    MSA_ILVL_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, tmp0, tmp1, tmp2, tmp3);
    MSA_ILVL_B2(v8i16, zero, src4, zero, src5, tmp4, tmp5);
    FILTER_INPUT_8BIT(tmp0, tmp1 ,tmp2, tmp3, tmp4, tmp5, out0_h);
    dst0 = out0_l;
    dst1 = (v8i16)__msa_sldi_b((v16i8)out0_l, (v16i8)out0_l, 2);
    dst2 = (v8i16)__msa_sldi_b((v16i8)out0_l, (v16i8)out0_l, 4);
    dst3 = __msa_vshf_h(mask1, out0_h, out0_l);
    dst4 = (v8i16)__msa_sldi_b((v16i8)dst3, (v16i8)dst3, 2);
    dst5 = (v8i16)__msa_sldi_b((v16i8)dst3, (v16i8)dst3, 4);
    dst6 = __msa_vshf_h(mask2, out0_h, out0_l);
    dst7 = (v8i16)__msa_sldi_b((v16i8)dst6, (v16i8)dst6, 2);
    MSA_TRANSPOSE8x8_H(v8i16, dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7,
                       dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7);
    //r part
    UNPCK_R_W_H(dst0, tmp0);
    UNPCK_R_W_H(dst1, tmp1);
    UNPCK_R_W_H(dst2, tmp2);
    UNPCK_R_W_H(dst3, tmp3);
    UNPCK_R_W_H(dst4, tmp4);
    UNPCK_R_W_H(dst5, tmp5);
    HOR_FILTER_INPUT_16BIT(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_l);
    //l part
    UNPCK_L_W_H(dst0, tmp0);
    UNPCK_L_W_H(dst1, tmp1);
    UNPCK_L_W_H(dst2, tmp2);
    UNPCK_L_W_H(dst3, tmp3);
    UNPCK_L_W_H(dst4, tmp4);
    UNPCK_L_W_H(dst5, tmp5);
    HOR_FILTER_INPUT_16BIT(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_h);
    out0_l = (v8i16)__msa_srari_w((v4i32)out0_l, 10);
    out0_h = (v8i16)__msa_srari_w((v4i32)out0_h, 10);
    MSA_CLIP_SH2_0_255(out0_l, out0_h);
    out0_l = __msa_pckev_h(out0_h, out0_l);
    out0_l = (v8i16)__msa_pckev_b((v16i8)out0_l, (v16i8)out0_l);
    MSA_ST_D(out0_l, 0, pDst);
    psrc += iSrcStride;
    pDst += iDstStride;
  }
}

static
void McHorVer22WidthEq4_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                            int32_t iDstStride, int32_t iHeight) {
  int32_t iStride2 = iSrcStride << 1;
  uint8_t *psrc = (uint8_t*)pSrc - 2;
  v8i16 zero = { 0 };
  v8i16 src0, src1, src2, src3, src4, src5;
  v8i16 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_l, out0_h;
  v8i16 dst0, dst1, dst2, dst3, dst4 = {0}, dst5 = {0}, dst6 = {0}, dst7 = {0};
  v8i16 mask1 = {3, 4, 5, 6, 7, 8, 9, 10};
  for (int i = 0; i < iHeight; i++) {
    MSA_LD_V4(v8i16, psrc - iStride2, iSrcStride, src0, src1, src2, src3);
    MSA_LD_V2(v8i16, psrc + iStride2, iSrcStride, src4, src5);
    //r part
    MSA_ILVR_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, tmp0, tmp1, tmp2, tmp3);
    MSA_ILVR_B2(v8i16, zero, src4, zero, src5, tmp4, tmp5);
    FILTER_INPUT_8BIT(tmp0, tmp1 ,tmp2, tmp3, tmp4, tmp5, out0_l);
    //l part
    MSA_ILVL_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, tmp0, tmp1, tmp2, tmp3);
    MSA_ILVL_B2(v8i16, zero, src4, zero, src5, tmp4, tmp5);
    FILTER_INPUT_8BIT(tmp0, tmp1 ,tmp2, tmp3, tmp4, tmp5, out0_h);
    dst0 = out0_l;
    dst1 = (v8i16)__msa_sldi_b((v16i8)out0_l, (v16i8)out0_l, 2);
    dst2 = (v8i16)__msa_sldi_b((v16i8)out0_l, (v16i8)out0_l, 4);
    dst3 = __msa_vshf_h(mask1, out0_h, out0_l);
    MSA_TRANSPOSE8x8_H(v8i16, dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7,
                       dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7);
    //r part
    UNPCK_R_W_H(dst0, tmp0);
    UNPCK_R_W_H(dst1, tmp1);
    UNPCK_R_W_H(dst2, tmp2);
    UNPCK_R_W_H(dst3, tmp3);
    UNPCK_R_W_H(dst4, tmp4);
    UNPCK_R_W_H(dst5, tmp5);
    HOR_FILTER_INPUT_16BIT(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_l);
    //l part
    UNPCK_L_W_H(dst0, tmp0);
    UNPCK_L_W_H(dst1, tmp1);
    UNPCK_L_W_H(dst2, tmp2);
    UNPCK_L_W_H(dst3, tmp3);
    UNPCK_L_W_H(dst4, tmp4);
    UNPCK_L_W_H(dst5, tmp5);
    HOR_FILTER_INPUT_16BIT(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_h);
    out0_l = (v8i16)__msa_srari_w((v4i32)out0_l, 10);
    out0_h = (v8i16)__msa_srari_w((v4i32)out0_h, 10);
    MSA_CLIP_SH2_0_255(out0_l, out0_h);
    out0_l = __msa_pckev_h(out0_h, out0_l);
    out0_l = (v8i16)__msa_pckev_b((v16i8)out0_l, (v16i8)out0_l);
    MSA_ST_W(out0_l, 0, pDst);
    psrc += iSrcStride;
    pDst += iDstStride;
  }
}

void McHorVer22WidthEq5_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                            int32_t iDstStride, int32_t iHeight) {
  int32_t iStride2 = iSrcStride << 1;
  uint8_t *psrc = (uint8_t*)pSrc - 2;
  v8i16 zero = { 0 };
  v8i16 src0, src1, src2, src3, src4, src5;
  v8i16 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_l, out0_h;
  v8i16 dst0, dst1, dst2, dst3, dst4 = {0}, dst5 = {0}, dst6 = {0}, dst7 = {0};
  v8i16 mask1 = {3, 4, 5, 6, 7, 8, 9, 10};
  for (int i = 0; i < iHeight; i++) {
    MSA_LD_V4(v8i16, psrc - iStride2, iSrcStride, src0, src1, src2, src3);
    MSA_LD_V2(v8i16, psrc + iStride2, iSrcStride, src4, src5);
    //r part
    MSA_ILVR_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, tmp0, tmp1, tmp2, tmp3);
    MSA_ILVR_B2(v8i16, zero, src4, zero, src5, tmp4, tmp5);
    FILTER_INPUT_8BIT(tmp0, tmp1 ,tmp2, tmp3, tmp4, tmp5, out0_l);
    //l part
    MSA_ILVL_B4(v8i16, zero, src0, zero, src1, zero, src2, zero, src3, tmp0, tmp1, tmp2, tmp3);
    MSA_ILVL_B2(v8i16, zero, src4, zero, src5, tmp4, tmp5);
    FILTER_INPUT_8BIT(tmp0, tmp1 ,tmp2, tmp3, tmp4, tmp5, out0_h);
    dst0 = out0_l;
    dst1 = (v8i16)__msa_sldi_b((v16i8)out0_l, (v16i8)out0_l, 2);
    dst2 = (v8i16)__msa_sldi_b((v16i8)out0_l, (v16i8)out0_l, 4);
    dst3 = __msa_vshf_h(mask1, out0_h, out0_l);
    dst4 = (v8i16)__msa_sldi_b((v16i8)dst3, (v16i8)dst3, 2);
    MSA_TRANSPOSE8x8_H(v8i16, dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7,
                       dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7);
    //r part
    UNPCK_R_W_H(dst0, tmp0);
    UNPCK_R_W_H(dst1, tmp1);
    UNPCK_R_W_H(dst2, tmp2);
    UNPCK_R_W_H(dst3, tmp3);
    UNPCK_R_W_H(dst4, tmp4);
    UNPCK_R_W_H(dst5, tmp5);
    HOR_FILTER_INPUT_16BIT(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_l);
    //l part
    UNPCK_L_W_H(dst0, tmp0);
    UNPCK_L_W_H(dst1, tmp1);
    UNPCK_L_W_H(dst2, tmp2);
    UNPCK_L_W_H(dst3, tmp3);
    UNPCK_L_W_H(dst4, tmp4);
    UNPCK_L_W_H(dst5, tmp5);
    HOR_FILTER_INPUT_16BIT(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, out0_h);
    out0_l = (v8i16)__msa_srari_w((v4i32)out0_l, 10);
    out0_h = (v8i16)__msa_srari_w((v4i32)out0_h, 10);
    MSA_CLIP_SH2_0_255(out0_l, out0_h);
    out0_l = __msa_pckev_h(out0_h, out0_l);
    out0_l = (v8i16)__msa_pckev_b((v16i8)out0_l, (v16i8)out0_l);
    MSA_ST_W(out0_l, 0, pDst);
    *(pDst + 4) = __msa_copy_u_b((v16i8)out0_l, 4);
    psrc += iSrcStride;
    pDst += iDstStride;
  }
}

void McHorVer22WidthEq9_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                            int32_t iDstStride, int32_t iHeight) {
  McHorVer22WidthEq4_msa(pSrc, iSrcStride, pDst, iDstStride, iHeight);
  McHorVer22WidthEq5_msa(&pSrc[4], iSrcStride, &pDst[4], iDstStride, iHeight);
}

void McHorVer22WidthEq17_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                             int32_t iDstStride, int32_t iHeight) {
  McHorVer22WidthEq8_msa(pSrc, iSrcStride, pDst, iDstStride, iHeight);
  McHorVer22WidthEq9_msa(&pSrc[8], iSrcStride, &pDst[8], iDstStride, iHeight);
}
