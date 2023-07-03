/*!
 * \copy
 *     Copyright (c)  2022, Cisco Systems
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
 * \file    satd_sad_msa.c
 *
 * \brief   Loongson optimization
 *
 * \date    21/03/2023 Created
 *
 *************************************************************************************
 */

#include <stdint.h>
#include "msa_macros.h"

int32_t WelsSampleSad4x4_msa (uint8_t* pSample1, int32_t iStride1, uint8_t* pSample2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3;
  v16u8 vec0, vec1, vec2, vec3;

  MSA_LD_V4(v16u8, pSample1, iStride1, src0, src1, src2, src3);
  MSA_LD_V4(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3);

  MSA_ILVR_W4(v16u8, src0, src1, src2, src3, vec0, vec1, vec2, vec3, src0, src1, vec0, vec1);
  MSA_ILVR_D2(v16u8, src0, src1, vec0, vec1, src0, vec0);

  src0 = __msa_asub_u_b(src0, vec0);
  src0 = (v16u8)__msa_hadd_u_h(src0, src0);
  src0 = (v16u8)__msa_hadd_u_w((v8u16)src0, (v8u16)src0);
  src0 = (v16u8)__msa_hadd_u_d((v4u32)src0, (v4u32)src0);
  src1 = (v16u8)__msa_splati_d((v2i64)src0, 1);
  src0 = (v16u8)__msa_addv_d((v2i64)src0, (v2i64)src1);
  return __msa_copy_s_w((v4i32)src0, 0);
}

int32_t WelsSampleSad8x8_msa(uint8_t* pSample1, int32_t iStride1, uint8_t* pSample2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;

  MSA_LD_V8(v16u8, pSample1, iStride1, src0, src1, src2, src3, src4, src5, src6, src7);
  MSA_LD_V8(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  MSA_ILVR_D4(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, src0, src2, src4, src6);
  MSA_ILVR_D4(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, vec0, vec2, vec4, vec6);

  src0 = __msa_asub_u_b(src0, vec0);
  src2 = __msa_asub_u_b(src2, vec2);
  src4 = __msa_asub_u_b(src4, vec4);
  src6 = __msa_asub_u_b(src6, vec6);
  src0 = (v16u8)__msa_hadd_u_h(src0, src0);
  src2 = (v16u8)__msa_hadd_u_h(src2, src2);
  src4 = (v16u8)__msa_hadd_u_h(src4, src4);
  src6 = (v16u8)__msa_hadd_u_h(src6, src6);
  src0 = (v16u8)__msa_addv_h((v8i16)src0, (v8i16)src2);
  src0 = (v16u8)__msa_addv_h((v8i16)src0, (v8i16)src4);
  src0 = (v16u8)__msa_addv_h((v8i16)src0, (v8i16)src6);
  src0 = (v16u8)__msa_hadd_u_w((v8u16)src0, (v8u16)src0);
  src0 = (v16u8)__msa_hadd_u_d((v4u32)src0, (v4u32)src0);
  src1 = (v16u8)__msa_splati_d((v2i64)src0, 1);
  src0 = (v16u8)__msa_addv_d((v2i64)src0, (v2i64)src1);
  return __msa_copy_s_w((v4i32)src0, 0);
}

int32_t WelsSampleSad8x16_msa (uint8_t* pSample1, int32_t iStride1, uint8_t* pSample2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
  v16u8 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  v4u32 tmp8;
  v2u64 tmp9, tmp10;

  MSA_LD_V8(v16u8, pSample1, iStride1, src0, src1, src2, src3, src4, src5, src6, src7);
  MSA_LD_V8(v16u8, pSample1 + (iStride1 << 3), iStride1, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  MSA_ILVR_D4(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, src0, src1, src2, src3);
  MSA_ILVR_D4(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, src4, src5, src6, src7);

  MSA_LD_V8(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  MSA_LD_V8(v16u8, pSample2 + (iStride2 << 3), iStride2, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7);
  MSA_ILVR_D4(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, vec0, vec1, vec2, vec3);
  MSA_ILVR_D4(v16u8, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, vec4, vec5, vec6, vec7);

  src0 = __msa_asub_u_b(src0, vec0);
  src1 = __msa_asub_u_b(src1, vec1);
  src2 = __msa_asub_u_b(src2, vec2);
  src3 = __msa_asub_u_b(src3, vec3);
  src4 = __msa_asub_u_b(src4, vec4);
  src5 = __msa_asub_u_b(src5, vec5);
  src6 = __msa_asub_u_b(src6, vec6);
  src7 = __msa_asub_u_b(src7, vec7);

  tmp0 = (v16u8)__msa_hadd_u_h(src0, src0);
  tmp1 = (v16u8)__msa_hadd_u_h(src1, src1);
  tmp2 = (v16u8)__msa_hadd_u_h(src2, src2);
  tmp3 = (v16u8)__msa_hadd_u_h(src3, src3);
  tmp4 = (v16u8)__msa_hadd_u_h(src4, src4);
  tmp5 = (v16u8)__msa_hadd_u_h(src5, src5);
  tmp6 = (v16u8)__msa_hadd_u_h(src6, src6);
  tmp7 = (v16u8)__msa_hadd_u_h(src7, src7);

  tmp0 = (v16u8)__msa_addv_h((v8i16)tmp0, (v8i16)tmp1);
  tmp0 = (v16u8)__msa_addv_h((v8i16)tmp0, (v8i16)tmp2);
  tmp0 = (v16u8)__msa_addv_h((v8i16)tmp0, (v8i16)tmp3);
  tmp0 = (v16u8)__msa_addv_h((v8i16)tmp0, (v8i16)tmp4);
  tmp0 = (v16u8)__msa_addv_h((v8i16)tmp0, (v8i16)tmp5);
  tmp0 = (v16u8)__msa_addv_h((v8i16)tmp0, (v8i16)tmp6);
  tmp0 = (v16u8)__msa_addv_h((v8i16)tmp0, (v8i16)tmp7);

  tmp8 = __msa_hadd_u_w((v8u16)tmp0, (v8u16)tmp0);
  tmp9 = __msa_hadd_u_d(tmp8, tmp8);
  tmp10 = (v2u64)__msa_splati_d((v2i64)tmp9, 1);
  tmp9 = (v2u64)__msa_addv_d((v2i64)tmp9, (v2i64)tmp10);
  return __msa_copy_s_w((v4i32)tmp9, 0);
}

int32_t WelsSampleSad16x8_msa (uint8_t* pSample1, int32_t iStride1, uint8_t* pSample2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
  v8u16 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  v4u32 tmp8;
  v2u64 tmp9, tmp10;

  MSA_LD_V8(v16u8, pSample1, iStride1, src0, src1, src2, src3, src4, src5, src6, src7);
  MSA_LD_V8(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);

  src0 = __msa_asub_u_b(src0, vec0);
  src1 = __msa_asub_u_b(src1, vec1);
  src2 = __msa_asub_u_b(src2, vec2);
  src3 = __msa_asub_u_b(src3, vec3);
  src4 = __msa_asub_u_b(src4, vec4);
  src5 = __msa_asub_u_b(src5, vec5);
  src6 = __msa_asub_u_b(src6, vec6);
  src7 = __msa_asub_u_b(src7, vec7);

  tmp0 = __msa_hadd_u_h(src0, src0);
  tmp1 = __msa_hadd_u_h(src1, src1);
  tmp2 = __msa_hadd_u_h(src2, src2);
  tmp3 = __msa_hadd_u_h(src3, src3);
  tmp4 = __msa_hadd_u_h(src4, src4);
  tmp5 = __msa_hadd_u_h(src5, src5);
  tmp6 = __msa_hadd_u_h(src6, src6);
  tmp7 = __msa_hadd_u_h(src7, src7);

  tmp0 = (v8u16)__msa_addv_h((v8i16)tmp0, (v8i16)tmp1);
  tmp0 = (v8u16)__msa_addv_h((v8i16)tmp0, (v8i16)tmp2);
  tmp0 = (v8u16)__msa_addv_h((v8i16)tmp0, (v8i16)tmp3);
  tmp0 = (v8u16)__msa_addv_h((v8i16)tmp0, (v8i16)tmp4);
  tmp0 = (v8u16)__msa_addv_h((v8i16)tmp0, (v8i16)tmp5);
  tmp0 = (v8u16)__msa_addv_h((v8i16)tmp0, (v8i16)tmp6);
  tmp0 = (v8u16)__msa_addv_h((v8i16)tmp0, (v8i16)tmp7);

  tmp8 = __msa_hadd_u_w(tmp0, tmp0);
  tmp9 = __msa_hadd_u_d(tmp8, tmp8);
  tmp10 = (v2u64)__msa_splati_d((v2i64)tmp9, 1);
  tmp9 = (v2u64)__msa_addv_d((v2i64)tmp9, (v2i64)tmp10);
  return __msa_copy_s_w((v4i32)tmp9, 0);
}

int32_t WelsSampleSad16x16_msa (uint8_t* pSample1, int32_t iStride1, uint8_t* pSample2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
  v8u16 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  v4u32 tmp8;
  v2u64 tmp9, tmp10;
  v8u16 ret0, ret1;

  MSA_LD_V8(v16u8, pSample1, iStride1, src0, src1, src2, src3, src4, src5, src6, src7);
  MSA_LD_V8(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);

  src0 = __msa_asub_u_b(src0, vec0);
  src1 = __msa_asub_u_b(src1, vec1);
  src2 = __msa_asub_u_b(src2, vec2);
  src3 = __msa_asub_u_b(src3, vec3);
  src4 = __msa_asub_u_b(src4, vec4);
  src5 = __msa_asub_u_b(src5, vec5);
  src6 = __msa_asub_u_b(src6, vec6);
  src7 = __msa_asub_u_b(src7, vec7);

  tmp0 = __msa_hadd_u_h(src0, src0);
  tmp1 = __msa_hadd_u_h(src1, src1);
  tmp2 = __msa_hadd_u_h(src2, src2);
  tmp3 = __msa_hadd_u_h(src3, src3);
  tmp4 = __msa_hadd_u_h(src4, src4);
  tmp5 = __msa_hadd_u_h(src5, src5);
  tmp6 = __msa_hadd_u_h(src6, src6);
  tmp7 = __msa_hadd_u_h(src7, src7);

  ret0 = (v8u16)__msa_addv_h((v8i16)tmp0, (v8i16)tmp1);
  ret0 = (v8u16)__msa_addv_h((v8i16)ret0, (v8i16)tmp2);
  ret0 = (v8u16)__msa_addv_h((v8i16)ret0, (v8i16)tmp3);
  ret0 = (v8u16)__msa_addv_h((v8i16)ret0, (v8i16)tmp4);
  ret0 = (v8u16)__msa_addv_h((v8i16)ret0, (v8i16)tmp5);
  ret0 = (v8u16)__msa_addv_h((v8i16)ret0, (v8i16)tmp6);
  ret0 = (v8u16)__msa_addv_h((v8i16)ret0, (v8i16)tmp7);

  MSA_LD_V8(v16u8, pSample1 + (iStride1 << 3), iStride1, src0, src1, src2, src3, src4, src5, src6, src7);
  MSA_LD_V8(v16u8, pSample2 + (iStride2 << 3), iStride2, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);

  src0 = __msa_asub_u_b(src0, vec0);
  src1 = __msa_asub_u_b(src1, vec1);
  src2 = __msa_asub_u_b(src2, vec2);
  src3 = __msa_asub_u_b(src3, vec3);
  src4 = __msa_asub_u_b(src4, vec4);
  src5 = __msa_asub_u_b(src5, vec5);
  src6 = __msa_asub_u_b(src6, vec6);
  src7 = __msa_asub_u_b(src7, vec7);

  tmp0 = __msa_hadd_u_h(src0, src0);
  tmp1 = __msa_hadd_u_h(src1, src1);
  tmp2 = __msa_hadd_u_h(src2, src2);
  tmp3 = __msa_hadd_u_h(src3, src3);
  tmp4 = __msa_hadd_u_h(src4, src4);
  tmp5 = __msa_hadd_u_h(src5, src5);
  tmp6 = __msa_hadd_u_h(src6, src6);
  tmp7 = __msa_hadd_u_h(src7, src7);

  ret1 = (v8u16)__msa_addv_h((v8i16)tmp0, (v8i16)tmp1);
  ret1 = (v8u16)__msa_addv_h((v8i16)ret1, (v8i16)tmp2);
  ret1 = (v8u16)__msa_addv_h((v8i16)ret1, (v8i16)tmp3);
  ret1 = (v8u16)__msa_addv_h((v8i16)ret1, (v8i16)tmp4);
  ret1 = (v8u16)__msa_addv_h((v8i16)ret1, (v8i16)tmp5);
  ret1 = (v8u16)__msa_addv_h((v8i16)ret1, (v8i16)tmp6);
  ret1 = (v8u16)__msa_addv_h((v8i16)ret1, (v8i16)tmp7);

  tmp0 = (v8u16)__msa_addv_h((v8i16)ret0, (v8i16)ret1);
  tmp8 = __msa_hadd_u_w(tmp0, tmp0);
  tmp9 = __msa_hadd_u_d(tmp8, tmp8);
  tmp10 = (v2u64)__msa_splati_d((v2i64)tmp9, 1);
  tmp9 = (v2u64)__msa_addv_d((v2i64)tmp9, (v2i64)tmp10);
  return __msa_copy_s_w((v4i32)tmp9, 0);

}

void WelsSampleSadFour4x4_msa (uint8_t* iSample1, int32_t iStride1, uint8_t* iSample2, int32_t iStride2, int32_t* pSad) {
  v16u8 src0, src1, src2, src3;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
  v16u8 vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15;

  MSA_LD_V4(v16u8, iSample1, iStride1, src0, src1, src2, src3);
  MSA_LD_V4(v16u8, iSample2 - iStride2, iStride2, vec0, vec1, vec2, vec3);
  MSA_LD_V4(v16u8, iSample2 + iStride2, iStride2, vec4, vec5, vec6, vec7);
  MSA_LD_V4(v16u8, iSample2 - 1, iStride2, vec8, vec9, vec10, vec11);
  MSA_LD_V4(v16u8, iSample2 + 1, iStride2, vec12, vec13, vec14, vec15);

  MSA_ILVR_W4(v16u8, src0, src1, src2, src3, vec0, vec1, vec2, vec3, src0, src1, vec0, vec1);
  MSA_ILVR_D2(v16u8, src0, src1, vec0, vec1, src0, vec0);
  MSA_ILVR_W4(v16u8, vec4, vec5, vec6, vec7, vec8, vec9, vec10, vec11, vec4, vec5, vec6, vec7);
  MSA_ILVR_D2(v16u8, vec4, vec5, vec6, vec7, vec4, vec5);
  MSA_ILVR_W2(v16u8, vec12, vec13, vec14, vec15, vec12, vec13);
  MSA_ILVR_D(v16u8, vec12, vec13, vec12);

  vec0 = __msa_asub_u_b(src0, vec0);
  vec1 = __msa_asub_u_b(src0, vec4);
  vec2 = __msa_asub_u_b(src0, vec5);
  vec3 = __msa_asub_u_b(src0, vec12);

  vec0 = (v16u8)__msa_hadd_u_h(vec0, vec0);
  vec1 = (v16u8)__msa_hadd_u_h(vec1, vec1);
  vec2 = (v16u8)__msa_hadd_u_h(vec2, vec2);
  vec3 = (v16u8)__msa_hadd_u_h(vec3, vec3);

  vec0 = (v16u8)__msa_hadd_u_w((v8u16)vec0, (v8u16)vec0);
  vec1 = (v16u8)__msa_hadd_u_w((v8u16)vec1, (v8u16)vec1);
  vec2 = (v16u8)__msa_hadd_u_w((v8u16)vec2, (v8u16)vec2);
  vec3 = (v16u8)__msa_hadd_u_w((v8u16)vec3, (v8u16)vec3);

  vec0 = (v16u8)__msa_hadd_u_d((v4u32)vec0, (v4u32)vec0);
  vec1 = (v16u8)__msa_hadd_u_d((v4u32)vec1, (v4u32)vec1);
  vec2 = (v16u8)__msa_hadd_u_d((v4u32)vec2, (v4u32)vec2);
  vec3 = (v16u8)__msa_hadd_u_d((v4u32)vec3, (v4u32)vec3);

  src0 = (v16u8)__msa_splati_d((v2i64)vec0, 1);
  src1 = (v16u8)__msa_splati_d((v2i64)vec1, 1);
  src2 = (v16u8)__msa_splati_d((v2i64)vec2, 1);
  src3 = (v16u8)__msa_splati_d((v2i64)vec3, 1);

  src0 = (v16u8)__msa_addv_d((v2i64)src0, (v2i64)vec0);
  src1 = (v16u8)__msa_addv_d((v2i64)src1, (v2i64)vec1);
  src2 = (v16u8)__msa_addv_d((v2i64)src2, (v2i64)vec2);
  src3 = (v16u8)__msa_addv_d((v2i64)src3, (v2i64)vec3);
  MSA_ILVR_W2(v16u8, src0, src1, src2, src3, src0, src1);
  MSA_ILVR_D(v16u8, src0, src1, src0);
  MSA_ST_V(v16u8, src0, pSad);
}

#define SAMPLESAD8x8(in0, in1, in2, in3, out0) do {\
  src1 = __msa_asub_u_b(src0, in0);\
  src3 = __msa_asub_u_b(src2, in1);\
  src5 = __msa_asub_u_b(src4, in2);\
  src7 = __msa_asub_u_b(src6, in3);\
  src1 = (v16u8)__msa_hadd_u_h(src1, src1);\
  src3 = (v16u8)__msa_hadd_u_h(src3, src3);\
  src5 = (v16u8)__msa_hadd_u_h(src5, src5);\
  src7 = (v16u8)__msa_hadd_u_h(src7, src7);\
  src1 = (v16u8)__msa_addv_h((v8i16)src1, (v8i16)src3);\
  src1 = (v16u8)__msa_addv_h((v8i16)src1, (v8i16)src5);\
  src1 = (v16u8)__msa_addv_h((v8i16)src1, (v8i16)src7);\
  src1 = (v16u8)__msa_hadd_u_w((v8u16)src1, (v8u16)src1);\
  src1 = (v16u8)__msa_hadd_u_d((v4u32)src1, (v4u32)src1);\
  tmp4 = (v16u8)__msa_splati_d((v2i64)src1, 1);\
  out0 = (v16u8)__msa_addv_d((v2i64)tmp4, (v2i64)src1);\
} while(0)

void WelsSampleSadFour8x8_msa (uint8_t* iSample1, int32_t iStride1, uint8_t* iSample2, int32_t iStride2, int32_t* pSad) {
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
  v16u8 tmp0, tmp1, tmp2, tmp3, tmp4;

  MSA_LD_V8(v16u8, iSample1, iStride1, src0, src1, src2, src3, src4, src5, src6, src7);
  MSA_ILVR_D4(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, src0, src2, src4, src6);
  MSA_LD_V8(v16u8, iSample2 - iStride2, iStride2, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  MSA_ILVR_D4(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, vec0, vec2, vec4, vec6);
  SAMPLESAD8x8(vec0, vec2, vec4, vec6, tmp0);
  MSA_LD_V8(v16u8, iSample2 + iStride2, iStride2, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  MSA_ILVR_D4(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, vec0, vec2, vec4, vec6);
  SAMPLESAD8x8(vec0, vec2, vec4, vec6, tmp1);
  MSA_LD_V8(v16u8, iSample2 - 1, iStride2, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  MSA_ILVR_D4(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, vec0, vec2, vec4, vec6);
  SAMPLESAD8x8(vec0, vec2, vec4, vec6, tmp2);
  MSA_LD_V8(v16u8, iSample2 + 1, iStride2, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  MSA_ILVR_D4(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, vec0, vec2, vec4, vec6);
  SAMPLESAD8x8(vec0, vec2, vec4, vec6, tmp3);
  * (pSad)     = __msa_copy_s_w((v4i32)tmp0, 0);
  * (pSad + 1) = __msa_copy_s_w((v4i32)tmp1, 0);
  * (pSad + 2) = __msa_copy_s_w((v4i32)tmp2, 0);
  * (pSad + 3) = __msa_copy_s_w((v4i32)tmp3, 0);
}

void WelsSampleSadFour8x16_msa (uint8_t* iSample1, int32_t iStride1, uint8_t* iSample2, int32_t iStride2, int32_t* pSad) {
  * (pSad)     = WelsSampleSad8x16_msa (iSample1, iStride1, (iSample2 - iStride2), iStride2);
  * (pSad + 1) = WelsSampleSad8x16_msa (iSample1, iStride1, (iSample2 + iStride2), iStride2);
  * (pSad + 2) = WelsSampleSad8x16_msa (iSample1, iStride1, (iSample2 - 1), iStride2);
  * (pSad + 3) = WelsSampleSad8x16_msa (iSample1, iStride1, (iSample2 + 1), iStride2);
}

void WelsSampleSadFour16x8_msa (uint8_t* iSample1, int32_t iStride1, uint8_t* iSample2, int32_t iStride2, int32_t* pSad) {
  * (pSad)     = WelsSampleSad16x8_msa (iSample1, iStride1, (iSample2 - iStride2), iStride2);
  * (pSad + 1) = WelsSampleSad16x8_msa (iSample1, iStride1, (iSample2 + iStride2), iStride2);
  * (pSad + 2) = WelsSampleSad16x8_msa (iSample1, iStride1, (iSample2 - 1), iStride2);
  * (pSad + 3) = WelsSampleSad16x8_msa (iSample1, iStride1, (iSample2 + 1), iStride2);
}

void WelsSampleSadFour16x16_msa (uint8_t* iSample1, int32_t iStride1, uint8_t* iSample2, int32_t iStride2, int32_t* pSad) {
  * (pSad)     = WelsSampleSad16x16_msa (iSample1, iStride1, (iSample2 - iStride2), iStride2);
  * (pSad + 1) = WelsSampleSad16x16_msa (iSample1, iStride1, (iSample2 + iStride2), iStride2);
  * (pSad + 2) = WelsSampleSad16x16_msa (iSample1, iStride1, (iSample2 - 1), iStride2);
  * (pSad + 3) = WelsSampleSad16x16_msa (iSample1, iStride1, (iSample2 + 1), iStride2);
}

int32_t WelsSampleSatd4x4_msa (uint8_t* pSample1, int32_t iStride1, uint8_t* pSample2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3, vec0, vec1, vec2, vec3;
  v8i16 s0, s1, s2, s3;

  MSA_LD_V4(v16u8, pSample1, iStride1, src0, src1, src2, src3);
  MSA_LD_V4(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3);
  MSA_ILVR_B4(v16u8, src0, vec0, src1, vec1, src2, vec2, src3, vec3, src0, src1, src2, src3);
  src0 = (v16u8)__msa_hsub_u_h(src0, src0);
  src1 = (v16u8)__msa_hsub_u_h(src1, src1);
  src2 = (v16u8)__msa_hsub_u_h(src2, src2);
  src3 = (v16u8)__msa_hsub_u_h(src3, src3);
  MSA_TRANSPOSE4x4_H(v16u8, src0, src1, src2, src3, vec0, vec1, vec2, vec3);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  MSA_TRANSPOSE4x4_H(v16u8, vec0, vec1, vec2, vec3, src0, src1, src2, src3);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec1);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec2);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec3);
  vec0 = (v16u8)__msa_hadd_u_w((v8u16)vec0, (v8u16)vec0);
  vec0 = (v16u8)__msa_hadd_u_d((v4u32)vec0, (v4u32)vec0);
  vec0 = (v16u8)__msa_srai_w((v4i32)vec0, 1);
  return __msa_copy_s_w((v4i32)vec0, 0);
}

int32_t WelsSampleSatd8x8_msa (uint8_t* pSample1, int32_t iStride1, uint8_t* pSample2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
  v8i16 s0, s1, s2, s3, s4, s5, s6, s7;

  MSA_LD_V4(v16u8, pSample1, iStride1, src0, src1, src2, src3);
  MSA_LD_V4(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3);
  MSA_LD_V4(v16u8, pSample1 + (iStride1 << 2), iStride1, src4, src5, src6, src7);
  MSA_LD_V4(v16u8, pSample2 + (iStride2 << 2), iStride2, vec4, vec5, vec6, vec7);
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
  MSA_TRANSPOSE8x8_H(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  MSA_TRANSPOSE8x8_H(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, src0, src1, src2, src3, src4, src5, src6, src7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec1);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec2);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec3);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec4);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec6);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec7);
  vec0 = (v16u8)__msa_hadd_u_w((v8u16)vec0, (v8u16)vec0);
  vec0 = (v16u8)__msa_hadd_u_d((v4u32)vec0, (v4u32)vec0);
  vec1 = (v16u8)__msa_splati_d((v2i64)vec0, 1);
  vec0 = (v16u8)__msa_addv_d((v2i64)vec0, (v2i64)vec1);
  vec0 = (v16u8)__msa_srai_w((v4i32)vec0, 1);
  return __msa_copy_s_w((v4i32)vec0, 0);
}

int32_t WelsSampleSatd16x8_msa (uint8_t* pSample1, int32_t iStride1, uint8_t* pSample2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
  v16u8 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  v16u8 dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7;
  v8i16 s0, s1, s2, s3, s4, s5, s6, s7;
  v16u8 tmp8;

  MSA_LD_V4(v16u8, pSample1, iStride1, src0, src1, src2, src3);
  MSA_LD_V4(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3);
  MSA_LD_V4(v16u8, pSample1 + (iStride1 << 2), iStride1, src4, src5, src6, src7);
  MSA_LD_V4(v16u8, pSample2 + (iStride2 << 2), iStride2, vec4, vec5, vec6, vec7);
  tmp0 = (v16u8)__msa_splati_d((v2i64)src0, 1);
  tmp1 = (v16u8)__msa_splati_d((v2i64)src1, 1);
  tmp2 = (v16u8)__msa_splati_d((v2i64)src2, 1);
  tmp3 = (v16u8)__msa_splati_d((v2i64)src3, 1);
  dst0 = (v16u8)__msa_splati_d((v2i64)vec0, 1);
  dst1 = (v16u8)__msa_splati_d((v2i64)vec1, 1);
  dst2 = (v16u8)__msa_splati_d((v2i64)vec2, 1);
  dst3 = (v16u8)__msa_splati_d((v2i64)vec3, 1);
  tmp4 = (v16u8)__msa_splati_d((v2i64)src4, 1);
  tmp5 = (v16u8)__msa_splati_d((v2i64)src5, 1);
  tmp6 = (v16u8)__msa_splati_d((v2i64)src6, 1);
  tmp7 = (v16u8)__msa_splati_d((v2i64)src7, 1);
  dst4 = (v16u8)__msa_splati_d((v2i64)vec4, 1);
  dst5 = (v16u8)__msa_splati_d((v2i64)vec5, 1);
  dst6 = (v16u8)__msa_splati_d((v2i64)vec6, 1);
  dst7 = (v16u8)__msa_splati_d((v2i64)vec7, 1);
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
  MSA_TRANSPOSE8x8_H(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  MSA_TRANSPOSE8x8_H(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, src0, src1, src2, src3, src4, src5, src6, src7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec1);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec2);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec3);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec4);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec6);
  tmp8 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec7);
  //pSample1 + 8
  MSA_ILVR_B4(v16u8, tmp0, dst0, tmp1, dst1, tmp2, dst2, tmp3, dst3, src0, src1, src2, src3);
  MSA_ILVR_B4(v16u8, tmp4, dst4, tmp5, dst5, tmp6, dst6, tmp7, dst7, src4, src5, src6, src7);
  src0 = (v16u8)__msa_hsub_u_h(src0, src0);
  src1 = (v16u8)__msa_hsub_u_h(src1, src1);
  src2 = (v16u8)__msa_hsub_u_h(src2, src2);
  src3 = (v16u8)__msa_hsub_u_h(src3, src3);
  src4 = (v16u8)__msa_hsub_u_h(src4, src4);
  src5 = (v16u8)__msa_hsub_u_h(src5, src5);
  src6 = (v16u8)__msa_hsub_u_h(src6, src6);
  src7 = (v16u8)__msa_hsub_u_h(src7, src7);
  MSA_TRANSPOSE8x8_H(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  MSA_TRANSPOSE8x8_H(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, src0, src1, src2, src3, src4, src5, src6, src7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec1);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec2);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec3);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec4);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec6);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec7);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)tmp8);
  vec0 = (v16u8)__msa_hadd_u_w((v8u16)vec0, (v8u16)vec0);
  vec0 = (v16u8)__msa_hadd_u_d((v4u32)vec0, (v4u32)vec0);
  vec1 = (v16u8)__msa_splati_d((v2i64)vec0, 1);
  vec0 = (v16u8)__msa_addv_d((v2i64)vec0, (v2i64)vec1);
  vec0 = (v16u8)__msa_srai_w((v4i32)vec0, 1);
  return __msa_copy_s_w((v4i32)vec0, 0);
}

int32_t WelsSampleSatd8x16_msa (uint8_t* pSample1, int32_t iStride1, uint8_t* pSample2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
  v8i16 s0, s1, s2, s3, s4, s5, s6, s7;
  v16u8 tmp0;

  MSA_LD_V4(v16u8, pSample1, iStride1, src0, src1, src2, src3);
  MSA_LD_V4(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3);
  MSA_LD_V4(v16u8, pSample1 + (iStride1 << 2), iStride1, src4, src5, src6, src7);
  MSA_LD_V4(v16u8, pSample2 + (iStride2 << 2), iStride2, vec4, vec5, vec6, vec7);
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
  MSA_TRANSPOSE8x8_H(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  MSA_TRANSPOSE8x8_H(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, src0, src1, src2, src3, src4, src5, src6, src7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec1);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec2);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec3);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec4);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec6);
  tmp0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec7);

  pSample1 = pSample1 + (iStride1 << 3);
  pSample2 = pSample2 + (iStride2 << 3);
  MSA_LD_V4(v16u8, pSample1, iStride1, src0, src1, src2, src3);
  MSA_LD_V4(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3);
  MSA_LD_V4(v16u8, pSample1 + (iStride1 << 2), iStride1, src4, src5, src6, src7);
  MSA_LD_V4(v16u8, pSample2 + (iStride2 << 2), iStride2, vec4, vec5, vec6, vec7);
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
  MSA_TRANSPOSE8x8_H(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  MSA_TRANSPOSE8x8_H(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, src0, src1, src2, src3, src4, src5, src6, src7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec1);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec2);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec3);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec4);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec6);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec7);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)tmp0);
  vec0 = (v16u8)__msa_hadd_u_w((v8u16)vec0, (v8u16)vec0);
  vec0 = (v16u8)__msa_hadd_u_d((v4u32)vec0, (v4u32)vec0);
  vec1 = (v16u8)__msa_splati_d((v2i64)vec0, 1);
  vec0 = (v16u8)__msa_addv_d((v2i64)vec0, (v2i64)vec1);
  vec0 = (v16u8)__msa_srai_w((v4i32)vec0, 1);
  return __msa_copy_s_w((v4i32)vec0, 0);
}

int32_t WelsSampleSatd16x16_msa (uint8_t* pSample1, int32_t iStride1, uint8_t* pSample2, int32_t iStride2) {
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
  v16u8 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  v16u8 dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7;
  v8i16 s0, s1, s2, s3, s4, s5, s6, s7;
  v16u8 tmp8;

  MSA_LD_V4(v16u8, pSample1, iStride1, src0, src1, src2, src3);
  MSA_LD_V4(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3);
  MSA_LD_V4(v16u8, pSample1 + (iStride1 << 2), iStride1, src4, src5, src6, src7);
  MSA_LD_V4(v16u8, pSample2 + (iStride2 << 2), iStride2, vec4, vec5, vec6, vec7);
  tmp0 = (v16u8)__msa_splati_d((v2i64)src0, 1);
  tmp1 = (v16u8)__msa_splati_d((v2i64)src1, 1);
  tmp2 = (v16u8)__msa_splati_d((v2i64)src2, 1);
  tmp3 = (v16u8)__msa_splati_d((v2i64)src3, 1);
  dst0 = (v16u8)__msa_splati_d((v2i64)vec0, 1);
  dst1 = (v16u8)__msa_splati_d((v2i64)vec1, 1);
  dst2 = (v16u8)__msa_splati_d((v2i64)vec2, 1);
  dst3 = (v16u8)__msa_splati_d((v2i64)vec3, 1);
  tmp4 = (v16u8)__msa_splati_d((v2i64)src4, 1);
  tmp5 = (v16u8)__msa_splati_d((v2i64)src5, 1);
  tmp6 = (v16u8)__msa_splati_d((v2i64)src6, 1);
  tmp7 = (v16u8)__msa_splati_d((v2i64)src7, 1);
  dst4 = (v16u8)__msa_splati_d((v2i64)vec4, 1);
  dst5 = (v16u8)__msa_splati_d((v2i64)vec5, 1);
  dst6 = (v16u8)__msa_splati_d((v2i64)vec6, 1);
  dst7 = (v16u8)__msa_splati_d((v2i64)vec7, 1);
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
  MSA_TRANSPOSE8x8_H(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  MSA_TRANSPOSE8x8_H(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, src0, src1, src2, src3, src4, src5, src6, src7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec1);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec2);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec3);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec4);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec6);
  tmp8 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec7);
  //pSample1 + 8
  MSA_ILVR_B4(v16u8, tmp0, dst0, tmp1, dst1, tmp2, dst2, tmp3, dst3, src0, src1, src2, src3);
  MSA_ILVR_B4(v16u8, tmp4, dst4, tmp5, dst5, tmp6, dst6, tmp7, dst7, src4, src5, src6, src7);
  src0 = (v16u8)__msa_hsub_u_h(src0, src0);
  src1 = (v16u8)__msa_hsub_u_h(src1, src1);
  src2 = (v16u8)__msa_hsub_u_h(src2, src2);
  src3 = (v16u8)__msa_hsub_u_h(src3, src3);
  src4 = (v16u8)__msa_hsub_u_h(src4, src4);
  src5 = (v16u8)__msa_hsub_u_h(src5, src5);
  src6 = (v16u8)__msa_hsub_u_h(src6, src6);
  src7 = (v16u8)__msa_hsub_u_h(src7, src7);
  MSA_TRANSPOSE8x8_H(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  MSA_TRANSPOSE8x8_H(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, src0, src1, src2, src3, src4, src5, src6, src7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec1);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec2);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec3);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec4);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec6);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec7);
  tmp8 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)tmp8);

  pSample1 = pSample1 + (iStride1 << 3);
  pSample2 = pSample2 + (iStride2 << 3);
  MSA_LD_V4(v16u8, pSample1, iStride1, src0, src1, src2, src3);
  MSA_LD_V4(v16u8, pSample2, iStride2, vec0, vec1, vec2, vec3);
  MSA_LD_V4(v16u8, pSample1 + (iStride1 << 2), iStride1, src4, src5, src6, src7);
  MSA_LD_V4(v16u8, pSample2 + (iStride2 << 2), iStride2, vec4, vec5, vec6, vec7);
  tmp0 = (v16u8)__msa_splati_d((v2i64)src0, 1);
  tmp1 = (v16u8)__msa_splati_d((v2i64)src1, 1);
  tmp2 = (v16u8)__msa_splati_d((v2i64)src2, 1);
  tmp3 = (v16u8)__msa_splati_d((v2i64)src3, 1);
  dst0 = (v16u8)__msa_splati_d((v2i64)vec0, 1);
  dst1 = (v16u8)__msa_splati_d((v2i64)vec1, 1);
  dst2 = (v16u8)__msa_splati_d((v2i64)vec2, 1);
  dst3 = (v16u8)__msa_splati_d((v2i64)vec3, 1);
  tmp4 = (v16u8)__msa_splati_d((v2i64)src4, 1);
  tmp5 = (v16u8)__msa_splati_d((v2i64)src5, 1);
  tmp6 = (v16u8)__msa_splati_d((v2i64)src6, 1);
  tmp7 = (v16u8)__msa_splati_d((v2i64)src7, 1);
  dst4 = (v16u8)__msa_splati_d((v2i64)vec4, 1);
  dst5 = (v16u8)__msa_splati_d((v2i64)vec5, 1);
  dst6 = (v16u8)__msa_splati_d((v2i64)vec6, 1);
  dst7 = (v16u8)__msa_splati_d((v2i64)vec7, 1);
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
  MSA_TRANSPOSE8x8_H(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  MSA_TRANSPOSE8x8_H(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, src0, src1, src2, src3, src4, src5, src6, src7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec1);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec2);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec3);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec4);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec6);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec7);
  tmp8 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)tmp8);
  //pSample1 + 8
  MSA_ILVR_B4(v16u8, tmp0, dst0, tmp1, dst1, tmp2, dst2, tmp3, dst3, src0, src1, src2, src3);
  MSA_ILVR_B4(v16u8, tmp4, dst4, tmp5, dst5, tmp6, dst6, tmp7, dst7, src4, src5, src6, src7);
  src0 = (v16u8)__msa_hsub_u_h(src0, src0);
  src1 = (v16u8)__msa_hsub_u_h(src1, src1);
  src2 = (v16u8)__msa_hsub_u_h(src2, src2);
  src3 = (v16u8)__msa_hsub_u_h(src3, src3);
  src4 = (v16u8)__msa_hsub_u_h(src4, src4);
  src5 = (v16u8)__msa_hsub_u_h(src5, src5);
  src6 = (v16u8)__msa_hsub_u_h(src6, src6);
  src7 = (v16u8)__msa_hsub_u_h(src7, src7);
  MSA_TRANSPOSE8x8_H(v16u8, src0, src1, src2, src3, src4, src5, src6, src7, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  MSA_TRANSPOSE8x8_H(v16u8, vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, src0, src1, src2, src3, src4, src5, src6, src7);
  s0 = __msa_addv_h((v8i16)src0, (v8i16)src2);
  s1 = __msa_addv_h((v8i16)src1, (v8i16)src3);
  s2 = __msa_subv_h((v8i16)src0, (v8i16)src2);
  s3 = __msa_subv_h((v8i16)src1, (v8i16)src3);
  s4 = __msa_addv_h((v8i16)src4, (v8i16)src6);
  s5 = __msa_addv_h((v8i16)src5, (v8i16)src7);
  s6 = __msa_subv_h((v8i16)src4, (v8i16)src6);
  s7 = __msa_subv_h((v8i16)src5, (v8i16)src7);
  vec0 = (v16u8)__msa_addv_h(s0, s1);
  vec1 = (v16u8)__msa_addv_h(s2, s3);
  vec2 = (v16u8)__msa_subv_h(s2, s3);
  vec3 = (v16u8)__msa_subv_h(s0, s1);
  vec4 = (v16u8)__msa_addv_h(s4, s5);
  vec5 = (v16u8)__msa_addv_h(s6, s7);
  vec6 = (v16u8)__msa_subv_h(s6, s7);
  vec7 = (v16u8)__msa_subv_h(s4, s5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec1);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec2);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec3);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec4);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec5);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec6);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)vec7);
  vec0 = (v16u8)__msa_add_a_h((v8i16)vec0, (v8i16)tmp8);
  vec0 = (v16u8)__msa_hadd_u_w((v8u16)vec0, (v8u16)vec0);
  vec0 = (v16u8)__msa_hadd_u_d((v4u32)vec0, (v4u32)vec0);
  vec1 = (v16u8)__msa_splati_d((v2i64)vec0, 1);
  vec0 = (v16u8)__msa_addv_d((v2i64)vec0, (v2i64)vec1);
  vec0 = (v16u8)__msa_srai_w((v4i32)vec0, 1);
  return __msa_copy_s_w((v4i32)vec0, 0);
}

