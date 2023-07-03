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
 * \file    intra_pred_msa.c
 *
 * \brief   Loongson optimization
 *
 * \date    04/06/2023 Created
 *
 *************************************************************************************
 */

#include <stdint.h>
#include "msa_macros.h"

#define MAIN_LOOP_8x8(i, out0)                               \
  factorc = __msa_fill_h(i - 3);                             \
  tmbc  = __msa_ilvr_h(srcc, srcb);                          \
  fbc   = __msa_ilvr_h(factorc, factorb);                    \
  iTmpr = __msa_dpadd_s_w(srca, tmbc, fbc);                  \
  tmbc  = __msa_ilvl_h(srcc, srcb);                          \
  fbc   = __msa_ilvl_h(factorc, factorb);                    \
  iTmpl = __msa_dpadd_s_w(srca, tmbc, fbc);                  \
  iTmpr = __msa_srari_w(iTmpr, 5);                           \
  iTmpl = __msa_srari_w(iTmpl, 5);                           \
  MSA_CLIP_SW2_0_255(iTmpr, iTmpl);                          \
  iTmpr = (v4i32)__msa_pckev_h((v8i16)iTmpl, (v8i16)iTmpr);  \
  out0 = (v2i64)__msa_pckev_b((v16i8)iTmpr, (v16i8)iTmpr)

#define MAIN_LOOP_16x16(i, out0)                               \
  factorc = __msa_fill_h(i - 7);                               \
  tmpbc = __msa_ilvr_h(srcc, srcb);                            \
  fbc   = __msa_ilvr_h(factorc, factorbr);                     \
  tmpr_r = __msa_dpadd_s_w(srca, tmpbc, fbc);                  \
  fbc   = __msa_ilvl_h(factorc, factorbr);                     \
  tmpr_l = __msa_dpadd_s_w(srca, tmpbc, fbc);                  \
  fbc   = __msa_ilvr_h(factorc, factorbl);                     \
  tmpl_r = __msa_dpadd_s_w(srca, tmpbc, fbc);                  \
  fbc   = __msa_ilvl_h(factorc, factorbl);                     \
  tmpl_l = __msa_dpadd_s_w(srca, tmpbc, fbc);                  \
  tmpr_r = __msa_srari_w(tmpr_r, 5);                           \
  tmpr_l = __msa_srari_w(tmpr_l, 5);                           \
  tmpl_r = __msa_srari_w(tmpl_r, 5);                           \
  tmpl_l = __msa_srari_w(tmpl_l, 5);                           \
  MSA_CLIP_SW4_0_255(tmpr_r, tmpr_l, tmpl_r, tmpl_l);          \
  tmpr_r = (v4i32)__msa_pckev_h((v8i16)tmpr_l, (v8i16)tmpr_r); \
  tmpr_r = (v4i32)__msa_pckev_b((v16i8)tmpr_r, (v16i8)tmpr_r); \
  tmpl_r = (v4i32)__msa_pckev_h((v8i16)tmpl_l, (v8i16)tmpl_r); \
  tmpl_r = (v4i32)__msa_pckev_b((v16i8)tmpl_r, (v16i8)tmpl_r); \
  out0 = __msa_ilvr_d((v2i64)tmpl_r, (v2i64)tmpr_r)

void WelsIChromaPredPlane_msa(uint8_t* pPred,  uint8_t* pRef, const int32_t kiStride) {
  uint8_t *pTop = pRef - kiStride;
  uint8_t *pLeft = pRef - 1;
  v16i8 mask = {3, 5, 2, 6, 1, 7, 0, 8};
  v8i16 factor1 = {1, 2, 3, 4, 1, 2, 3, 4};
  v8i16 factorb = {-3, -2, -1, 0, 1, 2, 3, 4};
  v8i16 num17 = __msa_fill_h(17);
  v16i8 src0 ,src1;
  v8i16 srcb, srcc, tmbc, fbc, factorc;
  v4i32 srca, iTmpr, iTmpl;
  v2i64 out0, out1, out2, out3;
  v2i64 out4, out5, out6, out7;
  MSA_LD_V(v16i8, pTop - 1, src0);
  src0 = __msa_vshf_b(mask, src0, src0);
  src1 = __msa_insert_b(src1, 0, pLeft[kiStride*2]);
  src1 = __msa_insert_b(src1, 1, pLeft[kiStride*4]);
  src1 = __msa_insert_b(src1, 2, pLeft[kiStride]);
  src1 = __msa_insert_b(src1, 3, pLeft[kiStride*5]);
  src1 = __msa_insert_b(src1, 4, pLeft[0]);
  src1 = __msa_insert_b(src1, 5, pLeft[kiStride*6]);
  src1 = __msa_insert_b(src1, 6, pLeft[-kiStride]);
  src1 = __msa_insert_b(src1, 7, pLeft[kiStride*7]);
  src0 = (v16i8)__msa_ilvr_d((v2i64)src1, (v2i64)src0);
  src0 = (v16i8)__msa_hsub_u_h((v16u8)src0, (v16u8)src0);
  src0 = (v16i8)__msa_mulv_h(factor1 ,(v8i16)src0);
  src0 = (v16i8)__msa_hadd_s_w((v8i16)src0, (v8i16)src0);
  src0 = (v16i8)__msa_hadd_s_d((v4i32)src0, (v4i32)src0);
  src0 = (v16i8)__msa_mulv_h((v8i16)src0, num17);
  src0 = (v16i8)__msa_srari_h((v8i16)src0, 5);
  srca = __msa_fill_w((pLeft[7 * kiStride] + pTop[7]) << 4);
  srcb = __msa_splati_h((v8i16)src0, 0);
  srcc = __msa_splati_h((v8i16)src0, 4);
  MAIN_LOOP_8x8(0, out0);
  MAIN_LOOP_8x8(1, out1);
  MAIN_LOOP_8x8(2, out2);
  MAIN_LOOP_8x8(3, out3);
  MAIN_LOOP_8x8(4, out4);
  MAIN_LOOP_8x8(5, out5);
  MAIN_LOOP_8x8(6, out6);
  MAIN_LOOP_8x8(7, out7);
  MSA_ILVR_D4(v2i64, out1, out0, out3, out2,
  out5, out4, out7, out6, out0, out1, out2, out3);
  MSA_ST_V4(v2i64, out0, out1, out2, out3, pPred, 16);
}

void WelsI16x16LumaPredPlane_msa (uint8_t* pPred, uint8_t* pRef, const int32_t kiStride) {
  uint8_t *pTop = pRef - kiStride;
  uint8_t *pLeft = pRef - 1;
  v16i8 mask = {6, 8, 5, 9, 4, 10, 3, 11, 2, 12, 1, 13, 0, 14, 7, 15};
  v8i16 factor = {1, 2, 3, 4, 5, 6, 7, 8};
  v8i16 factorbr = {-7, -6, -5, -4, -3, -2, -1, 0};
  v8i16 factorbl = {1, 2, 3, 4, 5, 6, 7, 8};
  v8i16 num5 = __msa_fill_h(5);
  v16i8 src0, src1;
  v8i16 srcb, srcc, factorc, tmpbc, fbc;
  v4i32 srca, tmpr_r, tmpr_l, tmpl_r, tmpl_l;
  v2i64 tmp0, out0, out1, out2, out3, out4, out5, out6, out7;
  MSA_LD_V(v16i8, pTop, src0);
  src0 = __msa_insert_b(src0, 7, pTop[-1]);
  src0 = __msa_vshf_b(mask, src0, src0);
  src1 = __msa_insert_b(src1, 0, pLeft[kiStride*6]);
  src1 = __msa_insert_b(src1, 1, pLeft[kiStride*8]);
  src1 = __msa_insert_b(src1, 2, pLeft[kiStride*5]);
  src1 = __msa_insert_b(src1, 3, pLeft[kiStride*9]);
  src1 = __msa_insert_b(src1, 4, pLeft[kiStride*4]);
  src1 = __msa_insert_b(src1, 5, pLeft[kiStride*10]);
  src1 = __msa_insert_b(src1, 6, pLeft[kiStride*3]);
  src1 = __msa_insert_b(src1, 7, pLeft[kiStride*11]);
  src1 = __msa_insert_b(src1, 8, pLeft[kiStride*2]);
  src1 = __msa_insert_b(src1, 9, pLeft[kiStride*12]);
  src1 = __msa_insert_b(src1, 10, pLeft[kiStride*1]);
  src1 = __msa_insert_b(src1, 11, pLeft[kiStride*13]);
  src1 = __msa_insert_b(src1, 12, pLeft[0]);
  src1 = __msa_insert_b(src1, 13, pLeft[kiStride*14]);
  src1 = __msa_insert_b(src1, 14, pLeft[-kiStride]);
  src1 = __msa_insert_b(src1, 15, pLeft[kiStride*15]);
  src0 = (v16i8)__msa_hsub_u_h((v16u8)src0, (v16u8)src0);
  src1 = (v16i8)__msa_hsub_u_h((v16u8)src1, (v16u8)src1);
  src0 = (v16i8)__msa_mulv_h((v8i16)src0, factor);//src0 * factor;
  src1 = (v16i8)__msa_mulv_h((v8i16)src1, factor);//src1 * factor;
  src0 = (v16i8)__msa_hadd_s_w((v8i16)src0, (v8i16)src0);
  src1 = (v16i8)__msa_hadd_s_w((v8i16)src1, (v8i16)src1);
  src0 = (v16i8)__msa_hadd_s_d((v4i32)src0, (v4i32)src0);
  tmp0 = __msa_splati_d((v2i64)src0, 1);
  src0 = (v16i8)__msa_addv_d(tmp0, (v2i64)src0);
  src1 = (v16i8)__msa_hadd_s_d((v4i32)src1, (v4i32)src1);
  tmp0 = __msa_splati_d((v2i64)src1, 1);
  src1 = (v16i8)__msa_addv_d(tmp0, (v2i64)src1);
  src0 = (v16i8)__msa_ilvr_h((v8i16)src1, (v8i16)src0);
  src0 = (v16i8)__msa_mulv_h((v8i16)src0, num5);
  src0 = (v16i8)__msa_srari_h((v8i16)src0, 6);
  srca = __msa_fill_w((pLeft[15 * kiStride] + pTop[15]) << 4);
  srcb = __msa_splati_h((v8i16)src0, 0);
  srcc = __msa_splati_h((v8i16)src0, 1);
  MAIN_LOOP_16x16(0, out0);
  MAIN_LOOP_16x16(1, out1);
  MAIN_LOOP_16x16(2, out2);
  MAIN_LOOP_16x16(3, out3);
  MAIN_LOOP_16x16(4, out4);
  MAIN_LOOP_16x16(5, out5);
  MAIN_LOOP_16x16(6, out6);
  MAIN_LOOP_16x16(7, out7);
  MSA_ST_V8(v2i64, out0, out1, out2, out3, out4, out5, out6, out7, pPred, 16);
  MAIN_LOOP_16x16(8, out0);
  MAIN_LOOP_16x16(9, out1);
  MAIN_LOOP_16x16(10, out2);
  MAIN_LOOP_16x16(11, out3);
  MAIN_LOOP_16x16(12, out4);
  MAIN_LOOP_16x16(13, out5);
  MAIN_LOOP_16x16(14, out6);
  MAIN_LOOP_16x16(15, out7);
  pPred += 16*8;
  MSA_ST_V8(v2i64, out0, out1, out2, out3, out4, out5, out6, out7, pPred, 16);
}

void WelsIChromaPredV_msa (uint8_t* pPred, uint8_t* pRef, const int32_t kiStride) {
  v2i64 src0;
  src0 = __msa_fill_d(LD(pRef - kiStride));
  MSA_ST_V4(v2i64, src0, src0, src0, src0, pPred, 16);
}

void WelsIChromaPredH_msa (uint8_t* pPred, uint8_t* pRef, const int32_t kiStride) {
  v16i8 src0, src1, src2, src3, src4, src5, src6, src7;
  pRef -= 1;
  src0 = __msa_fill_b(pRef[0]);
  src1 = __msa_fill_b(pRef[kiStride]);
  src2 = __msa_fill_b(pRef[kiStride*2]);
  src3 = __msa_fill_b(pRef[kiStride*3]);
  src4 = __msa_fill_b(pRef[kiStride*4]);
  src5 = __msa_fill_b(pRef[kiStride*5]);
  src6 = __msa_fill_b(pRef[kiStride*6]);
  src7 = __msa_fill_b(pRef[kiStride*7]);
  src0 = (v16i8)__msa_ilvr_d((v2i64)src1, (v2i64)src0);
  src2 = (v16i8)__msa_ilvr_d((v2i64)src3, (v2i64)src2);
  src4 = (v16i8)__msa_ilvr_d((v2i64)src5, (v2i64)src4);
  src6 = (v16i8)__msa_ilvr_d((v2i64)src7, (v2i64)src6);
  MSA_ST_V4(v16i8, src0, src2, src4, src6, pPred, 16);
}

void WelsIChromaPredDc_msa (uint8_t* pPred, uint8_t* pRef, const int32_t kiStride) {
  const int32_t kuiL1 = kiStride - 1;
  const int32_t kuiL2 = kuiL1 + kiStride;
  const int32_t kuiL3 = kuiL2 + kiStride;
  const int32_t kuiL4 = kuiL3 + kiStride;
  const int32_t kuiL5 = kuiL4 + kiStride;
  const int32_t kuiL6 = kuiL5 + kiStride;
  const int32_t kuiL7 = kuiL6 + kiStride;
  v16i8 src0, src1, src2, src3, src4;
  MSA_LD_V(v16i8, pRef - kiStride, src0);
  src1 = (v16i8)__msa_splati_w((v4i32)src0, 1);
  src0 = __msa_insert_b(src0, 4, pRef[-1]);
  src0 = __msa_insert_b(src0, 5, pRef[kuiL1]);
  src0 = __msa_insert_b(src0, 6, pRef[kuiL2]);
  src0 = __msa_insert_b(src0, 7, pRef[kuiL3]);
  src3 = __msa_insert_b(src3, 0, pRef[kuiL4]);
  src3 = __msa_insert_b(src3, 1, pRef[kuiL5]);
  src3 = __msa_insert_b(src3, 2, pRef[kuiL6]);
  src3 = __msa_insert_b(src3, 3, pRef[kuiL7]);
  src0 = (v16i8)__msa_hadd_u_h((v16u8)src0, (v16u8)src0);
  src0 = (v16i8)__msa_hadd_u_w((v8u16)src0, (v8u16)src0);
  src0 = (v16i8)__msa_hadd_u_d((v4u32)src0, (v4u32)src0);
  src0 = (v16i8)__msa_srari_w((v4i32)src0, 3);
  src0 = __msa_splati_b(src0, 0); //kuiMean1
  src1 = (v16i8)__msa_hadd_u_h((v16u8)src1, (v16u8)src1);
  src1 = (v16i8)__msa_hadd_u_w((v8u16)src1, (v8u16)src1); //kuiSum2
  src3 = (v16i8)__msa_hadd_u_h((v16u8)src3, (v16u8)src3);
  src3 = (v16i8)__msa_hadd_u_w((v8u16)src3, (v8u16)src3); //kuiSum3
  src4 = (v16i8)__msa_addv_w((v4i32)src1, (v4i32)src3);
  src4 = (v16i8)__msa_srari_w((v4i32)src4, 3);
  src4 = __msa_splati_b(src4, 0); //kuiMean4
  src2 = (v16i8)__msa_srari_w((v4i32)src1, 2);
  src2 = __msa_splati_b(src2, 0); //kuiMean2
  src3 = (v16i8)__msa_srari_w((v4i32)src3, 2);
  src3 = __msa_splati_b(src3, 0); //kuiMean3
  src0 = (v16i8)__msa_ilvr_w((v4i32)src2, (v4i32)src0);
  src0 = (v16i8)__msa_splati_d((v2i64)src0, 0);
  src3 = (v16i8)__msa_ilvr_w((v4i32)src4, (v4i32)src3);
  src3 = (v16i8)__msa_splati_d((v2i64)src3, 0);
  MSA_ST_V2(v16i8, src0, src0, pPred, 16);
  MSA_ST_V2(v16i8, src3, src3, pPred + 32, 16);
}

void WelsI16x16LumaPredDc_msa (uint8_t* pPred, uint8_t* pRef, const int32_t kiStride) {
  int32_t kiStridex2 = kiStride << 1;
  int32_t kiStridex3 = kiStridex2 + kiStride;
  int32_t kiStridex4 = kiStridex3 + kiStride;
  int32_t kiStridex5 = kiStridex4 + kiStride;
  int32_t kiStridex6 = kiStridex5 + kiStride;
  int32_t kiStridex7 = kiStridex6 + kiStride;
  int32_t kiStridex8 = kiStridex7 + kiStride;
  int32_t kiStridex9 = kiStridex8 + kiStride;
  int32_t kiStridex10 = kiStridex9 + kiStride;
  int32_t kiStridex11 = kiStridex10 + kiStride;
  int32_t kiStridex12 = kiStridex11 + kiStride;
  int32_t kiStridex13 = kiStridex12 + kiStride;
  int32_t kiStridex14 = kiStridex13 + kiStride;
  int32_t kiStridex15 = kiStridex14 + kiStride;
  v16i8 src0, src1, tmpl;
  MSA_LD_V(v16i8, pRef - kiStride, src0);
  pRef -= 1;
  src1 = __msa_insert_b(src1, 0, pRef[0]);
  src1 = __msa_insert_b(src1, 1, pRef[kiStride]);
  src1 = __msa_insert_b(src1, 2, pRef[kiStridex2]);
  src1 = __msa_insert_b(src1, 3, pRef[kiStridex3]);
  src1 = __msa_insert_b(src1, 4, pRef[kiStridex4]);
  src1 = __msa_insert_b(src1, 5, pRef[kiStridex5]);
  src1 = __msa_insert_b(src1, 6, pRef[kiStridex6]);
  src1 = __msa_insert_b(src1, 7, pRef[kiStridex7]);
  src1 = __msa_insert_b(src1, 8, pRef[kiStridex8]);
  src1 = __msa_insert_b(src1, 9, pRef[kiStridex9]);
  src1 = __msa_insert_b(src1, 10, pRef[kiStridex10]);
  src1 = __msa_insert_b(src1, 11, pRef[kiStridex11]);
  src1 = __msa_insert_b(src1, 12, pRef[kiStridex12]);
  src1 = __msa_insert_b(src1, 13, pRef[kiStridex13]);
  src1 = __msa_insert_b(src1, 14, pRef[kiStridex14]);
  src1 = __msa_insert_b(src1, 15, pRef[kiStridex15]);
  src0 = (v16i8)__msa_hadd_u_h((v16u8)src0, (v16u8)src0);
  src0 = (v16i8)__msa_hadd_u_w((v8u16)src0, (v8u16)src0);
  src0 = (v16i8)__msa_hadd_u_d((v4u32)src0, (v4u32)src0);
  tmpl = (v16i8)__msa_splati_d((v2i64)src0, 1);
  src0 = (v16i8)__msa_addv_d((v2i64)src0, (v2i64)tmpl);
  src1 = (v16i8)__msa_hadd_u_h((v16u8)src1, (v16u8)src1);
  src1 = (v16i8)__msa_hadd_u_w((v8u16)src1, (v8u16)src1);
  src1 = (v16i8)__msa_hadd_u_d((v4u32)src1, (v4u32)src1);
  tmpl = (v16i8)__msa_splati_d((v2i64)src1, 1);
  src1 = (v16i8)__msa_addv_d((v2i64)src1, (v2i64)tmpl);
  src0 = (v16i8)__msa_addv_d((v2i64)src0, (v2i64)src1);
  src0 = (v16i8)__msa_srari_w((v4i32)src0, 5);
  src0 = __msa_splati_b(src0, 0);
  MSA_ST_V8(v16i8, src0, src0, src0, src0, src0, src0, src0, src0, pPred, 16);
  MSA_ST_V8(v16i8, src0, src0, src0, src0, src0, src0, src0, src0, pPred + 128, 16);
}
