/*!
 * \copy
 *     Copyright (c)  2009-2018, Cisco Systems
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
 * \file    vaa_msa.c
 *
 * \brief   Loongson optimization
 *
 * \date    22/03/2023 Created
 *
 *************************************************************************************
 */
#include <stdint.h>
#include "msa_macros.h"

#define MSA_SELECT_MAX_H(in0, out0) do {\
  v8u16 tmp0 = (v8u16)__msa_splati_d((v2i64)in0, 1);\
  tmp0 = __msa_max_u_h(tmp0, in0);\
  in0 = (v8u16)__msa_splati_w((v4i32)tmp0, 1);\
  tmp0 = __msa_max_u_h(tmp0, in0);\
  in0 = (v8u16)__msa_splati_h((v8i16)tmp0, 1);\
  tmp0 = __msa_max_u_h(tmp0, in0);\
  out0 = __msa_copy_s_h((v8i16)tmp0, 0);\
} while(0)

#define CALC_SAD_SD_MAD(in0, in1) do {\
  vec_diff = __msa_subv_h((v8i16)in0, (v8i16)in1);\
  abs_diff = __msa_asub_u_h((v8u16)in0, (v8u16)in1);\
  vec_l_sad += abs_diff;\
  vec_l_sd  += vec_diff;\
  vec_l_mad = __msa_max_u_h(abs_diff, vec_l_mad);\
} while(0)

void VAACalcSadBgd_msa (const uint8_t* pCurData, const uint8_t* pRefData, int32_t iPicWidth, int32_t iPicHeight,
                        int32_t iPicStride, int32_t* pFrameSad, int32_t* pSad8x8, int32_t* pSd8x8, uint8_t* pMad8x8) {
  const uint8_t* tmp_ref = pRefData;
  const uint8_t* tmp_cur = pCurData;
  int32_t iMbWidth = (iPicWidth >> 4);
  int32_t mb_height = (iPicHeight >> 4);
  int32_t mb_index = 0;
  int32_t pic_stride_x8 = iPicStride << 3;
  int32_t step = (iPicStride << 4) - iPicWidth;
  const uint8_t* tmp_cur_row;
  const uint8_t* tmp_ref_row;
  int32_t l_sad, l_sd, l_mad;
  int32_t iFrameSad = 0, index;
  v16u8 zero = {0};
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
  v16u8 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  v16u8 dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7;
  v8i16 vec_diff, vec_l_sd;
  v8u16 abs_diff, vec_l_sad, vec_l_mad;
  for (int32_t i = 0; i < mb_height; i ++) {
    for (int32_t j = 0; j < iMbWidth; j ++) {
      l_mad = l_sd = l_sad =  0;
      index = mb_index << 2;
      tmp_cur_row = tmp_cur;
      tmp_ref_row = tmp_ref;
      vec_l_sad = (v8u16)__msa_fill_h(0);
      vec_l_sd  = __msa_fill_h(0);
      vec_l_mad = (v8u16)__msa_fill_h(0);
      MSA_LD_V8(v16u8, tmp_cur_row, iPicStride, src0, src1, src2, src3, src4, src5, src6, src7);
      MSA_LD_V8(v16u8, tmp_ref_row, iPicStride, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
      //tmp_cur_row + 8
      tmp0 = (v16u8)__msa_splati_d((v2i64)src0, 1);
      tmp1 = (v16u8)__msa_splati_d((v2i64)src1, 1);
      tmp2 = (v16u8)__msa_splati_d((v2i64)src2, 1);
      tmp3 = (v16u8)__msa_splati_d((v2i64)src3, 1);
      tmp4 = (v16u8)__msa_splati_d((v2i64)src4, 1);
      tmp5 = (v16u8)__msa_splati_d((v2i64)src5, 1);
      tmp6 = (v16u8)__msa_splati_d((v2i64)src6, 1);
      tmp7 = (v16u8)__msa_splati_d((v2i64)src7, 1);
      //tmp_cur_ref + 8
      dst0 = (v16u8)__msa_splati_d((v2i64)vec0, 1);
      dst1 = (v16u8)__msa_splati_d((v2i64)vec1, 1);
      dst2 = (v16u8)__msa_splati_d((v2i64)vec2, 1);
      dst3 = (v16u8)__msa_splati_d((v2i64)vec3, 1);
      dst4 = (v16u8)__msa_splati_d((v2i64)vec4, 1);
      dst5 = (v16u8)__msa_splati_d((v2i64)vec5, 1);
      dst6 = (v16u8)__msa_splati_d((v2i64)vec6, 1);
      dst7 = (v16u8)__msa_splati_d((v2i64)vec7, 1);
      MSA_ILVR_B4(v16u8, zero, src0, zero, src1, zero, src2, zero, src3, src0, src1, src2, src3);
      MSA_ILVR_B4(v16u8, zero, src4, zero, src5, zero, src6, zero, src7, src4, src5, src6, src7);
      MSA_ILVR_B4(v16u8, zero, vec0, zero, vec1, zero, vec2, zero, vec3, vec0, vec1, vec2, vec3);
      MSA_ILVR_B4(v16u8, zero, vec4, zero, vec5, zero, vec6, zero, vec7, vec4, vec5, vec6, vec7);
      CALC_SAD_SD_MAD(src0, vec0);
      CALC_SAD_SD_MAD(src1, vec1);
      CALC_SAD_SD_MAD(src2, vec2);
      CALC_SAD_SD_MAD(src3, vec3);
      CALC_SAD_SD_MAD(src4, vec4);
      CALC_SAD_SD_MAD(src5, vec5);
      CALC_SAD_SD_MAD(src6, vec6);
      CALC_SAD_SD_MAD(src7, vec7);
      MSA_HADD_UH_U32(vec_l_sad, l_sad);
      MSA_HADD_SH_S32(vec_l_sd, l_sd);
      MSA_SELECT_MAX_H(vec_l_mad, l_mad);
      iFrameSad += l_sad;
      pSad8x8[index + 0] = l_sad;
      pSd8x8 [index + 0] = l_sd;
      pMad8x8[index + 0] = l_mad;

      vec_l_sad = (v8u16)__msa_fill_h(0);
      vec_l_sd  = __msa_fill_h(0);
      vec_l_mad = (v8u16)__msa_fill_h(0);
      MSA_ILVR_B4(v16u8, zero, tmp0, zero, tmp1, zero, tmp2, zero, tmp3, tmp0, tmp1, tmp2, tmp3);
      MSA_ILVR_B4(v16u8, zero, tmp4, zero, tmp5, zero, tmp6, zero, tmp7, tmp4, tmp5, tmp6, tmp7);
      MSA_ILVR_B4(v16u8, zero, dst0, zero, dst1, zero, dst2, zero, dst3, dst0, dst1, dst2, dst3);
      MSA_ILVR_B4(v16u8, zero, dst4, zero, dst5, zero, dst6, zero, dst7, dst4, dst5, dst6, dst7);
      CALC_SAD_SD_MAD(tmp0, dst0);
      CALC_SAD_SD_MAD(tmp1, dst1);
      CALC_SAD_SD_MAD(tmp2, dst2);
      CALC_SAD_SD_MAD(tmp3, dst3);
      CALC_SAD_SD_MAD(tmp4, dst4);
      CALC_SAD_SD_MAD(tmp5, dst5);
      CALC_SAD_SD_MAD(tmp6, dst6);
      CALC_SAD_SD_MAD(tmp7, dst7);
      MSA_HADD_UH_U32(vec_l_sad, l_sad);
      MSA_HADD_SH_S32(vec_l_sd, l_sd);
      MSA_SELECT_MAX_H(vec_l_mad, l_mad);
      iFrameSad += l_sad;
      pSad8x8[index + 1] = l_sad;
      pSd8x8 [index + 1] = l_sd;
      pMad8x8[index + 1] = l_mad;

      tmp_cur_row = tmp_cur + pic_stride_x8;
      tmp_ref_row = tmp_ref + pic_stride_x8;
      vec_l_sad = (v8u16)__msa_fill_h(0);
      vec_l_sd  = __msa_fill_h(0);
      vec_l_mad = (v8u16)__msa_fill_h(0);
      MSA_LD_V8(v16u8, tmp_cur_row, iPicStride, src0, src1, src2, src3, src4, src5, src6, src7);
      MSA_LD_V8(v16u8, tmp_ref_row, iPicStride, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
      //tmp_cur_row + 8
      tmp0 = (v16u8)__msa_splati_d((v2i64)src0, 1);
      tmp1 = (v16u8)__msa_splati_d((v2i64)src1, 1);
      tmp2 = (v16u8)__msa_splati_d((v2i64)src2, 1);
      tmp3 = (v16u8)__msa_splati_d((v2i64)src3, 1);
      tmp4 = (v16u8)__msa_splati_d((v2i64)src4, 1);
      tmp5 = (v16u8)__msa_splati_d((v2i64)src5, 1);
      tmp6 = (v16u8)__msa_splati_d((v2i64)src6, 1);
      tmp7 = (v16u8)__msa_splati_d((v2i64)src7, 1);
      //tmp_cur_ref + 8
      dst0 = (v16u8)__msa_splati_d((v2i64)vec0, 1);
      dst1 = (v16u8)__msa_splati_d((v2i64)vec1, 1);
      dst2 = (v16u8)__msa_splati_d((v2i64)vec2, 1);
      dst3 = (v16u8)__msa_splati_d((v2i64)vec3, 1);
      dst4 = (v16u8)__msa_splati_d((v2i64)vec4, 1);
      dst5 = (v16u8)__msa_splati_d((v2i64)vec5, 1);
      dst6 = (v16u8)__msa_splati_d((v2i64)vec6, 1);
      dst7 = (v16u8)__msa_splati_d((v2i64)vec7, 1);
      MSA_ILVR_B4(v16u8, zero, src0, zero, src1, zero, src2, zero, src3, src0, src1, src2, src3);
      MSA_ILVR_B4(v16u8, zero, src4, zero, src5, zero, src6, zero, src7, src4, src5, src6, src7);
      MSA_ILVR_B4(v16u8, zero, vec0, zero, vec1, zero, vec2, zero, vec3, vec0, vec1, vec2, vec3);
      MSA_ILVR_B4(v16u8, zero, vec4, zero, vec5, zero, vec6, zero, vec7, vec4, vec5, vec6, vec7);
      CALC_SAD_SD_MAD(src0, vec0);
      CALC_SAD_SD_MAD(src1, vec1);
      CALC_SAD_SD_MAD(src2, vec2);
      CALC_SAD_SD_MAD(src3, vec3);
      CALC_SAD_SD_MAD(src4, vec4);
      CALC_SAD_SD_MAD(src5, vec5);
      CALC_SAD_SD_MAD(src6, vec6);
      CALC_SAD_SD_MAD(src7, vec7);
      MSA_HADD_UH_U32(vec_l_sad, l_sad);
      MSA_HADD_SH_S32(vec_l_sd, l_sd);
      MSA_SELECT_MAX_H(vec_l_mad, l_mad);
      iFrameSad += l_sad;
      pSad8x8[index + 2] = l_sad;
      pSd8x8 [index + 2] = l_sd;
      pMad8x8[index + 2] = l_mad;

      vec_l_sad = (v8u16)__msa_fill_h(0);
      vec_l_sd  = __msa_fill_h(0);
      vec_l_mad = (v8u16)__msa_fill_h(0);
      MSA_ILVR_B4(v16u8, zero, tmp0, zero, tmp1, zero, tmp2, zero, tmp3, tmp0, tmp1, tmp2, tmp3);
      MSA_ILVR_B4(v16u8, zero, tmp4, zero, tmp5, zero, tmp6, zero, tmp7, tmp4, tmp5, tmp6, tmp7);
      MSA_ILVR_B4(v16u8, zero, dst0, zero, dst1, zero, dst2, zero, dst3, dst0, dst1, dst2, dst3);
      MSA_ILVR_B4(v16u8, zero, dst4, zero, dst5, zero, dst6, zero, dst7, dst4, dst5, dst6, dst7);
      CALC_SAD_SD_MAD(tmp0, dst0);
      CALC_SAD_SD_MAD(tmp1, dst1);
      CALC_SAD_SD_MAD(tmp2, dst2);
      CALC_SAD_SD_MAD(tmp3, dst3);
      CALC_SAD_SD_MAD(tmp4, dst4);
      CALC_SAD_SD_MAD(tmp5, dst5);
      CALC_SAD_SD_MAD(tmp6, dst6);
      CALC_SAD_SD_MAD(tmp7, dst7);
      MSA_HADD_UH_U32(vec_l_sad, l_sad);
      MSA_HADD_SH_S32(vec_l_sd, l_sd);
      MSA_SELECT_MAX_H(vec_l_mad, l_mad);
      iFrameSad += l_sad;
      pSad8x8[index + 3] = l_sad;
      pSd8x8 [index + 3] = l_sd;
      pMad8x8[index + 3] = l_mad;

      tmp_ref += 16;
      tmp_cur += 16;
      ++mb_index;
    }
    tmp_ref += step;
    tmp_cur += step;
  }
  *pFrameSad = iFrameSad;
}
