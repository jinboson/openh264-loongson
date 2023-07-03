/*!
 **********************************************************************************
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
 * \file    mc_chroma_msa.c
 *
 * \brief   Loongson optimization
 *
 * \date    03/31/2023 Created
 *
 **********************************************************************************
 */

#include "stdint.h"
#include "msa_macros.h"

void McCopyWidthEq4_msa(const uint8_t *pSrc, int iSrcStride,
                        uint8_t *pDst, int iDstStride, int iHeight) {
  uint64_t src0, src1;
  int iSrcStride_x2 = iSrcStride << 1;
  int iDstStride_x2 = iDstStride << 1;
  iHeight = iHeight >> 1;
  for (int i = 0; i < iHeight; i++) {
    src0 = LW(pSrc);
    src1 = LW(pSrc + iSrcStride);
    SW(src0, pDst);
    SW(src1, pDst + iDstStride);
    pSrc += iSrcStride_x2;
    pDst += iDstStride_x2;
  }
}

void McCopyWidthEq8_msa(const uint8_t *pSrc, int iSrcStride,
                        uint8_t *pDst, int iDstStride, int iHeight) {
  uint64_t src0, src1;
  int iSrcStride_x2 = iSrcStride << 1;
  int iDstStride_x2 = iDstStride << 1;
  iHeight = iHeight >> 1;
  for (int i = 0; i < iHeight; i++) {
    src0 = LD(pSrc);
    src1 = LD(pSrc + iSrcStride);
    SD(src0, pDst);
    SD(src1, pDst + iDstStride);
    pSrc += iSrcStride_x2;
    pDst += iDstStride_x2;
  }
}

void McCopyWidthEq16_msa(const uint8_t *pSrc, int iSrcStride,
                         uint8_t *pDst, int iDstStride, int iHeight) {
  v16u8 src0, src1;
  int iSrcStride_x2 = iSrcStride << 1;
  int iDstStride_x2 = iDstStride << 1;
  iHeight = iHeight >> 1;
  for (int i = 0; i < iHeight; i++) {
    MSA_LD_V2(v16u8, pSrc, iSrcStride, src0, src1);
    MSA_ST_V2(v16u8, src0, src1, pDst, iDstStride);
    pSrc += iSrcStride_x2;
    pDst += iDstStride_x2;
  }
}

void McChromaWidthEq4_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                          int32_t iDstStride, const uint8_t *pABCD, int32_t iHeight) {
  int32_t i;
  v16i8 src_abcd, src0, src1, src2, src3;

  MSA_LD_V(v16i8, pABCD, src_abcd);
  src_abcd = (v16i8)__msa_ilvr_h((v8i16)src_abcd, (v8i16)src_abcd);//ababcdcd
  src_abcd = (v16i8)__msa_ilvr_w((v4i32)src_abcd, (v4i32)src_abcd);
  MSA_LD_V(v16i8, pSrc, src0);
  for (i = 0; i < iHeight; i++) {
    MSA_LD_V(v16i8, pSrc + iSrcStride, src1);// 0 1 2 3 4 ==> 0 1 1 2 2 3 3 4
    src2 = __msa_sldi_b(src0, src0, 1); // 1 2 3 4 0 1 2 3 4
    src3 = __msa_sldi_b(src1, src1, 1); // 1 2 3 4 0 1 2 3 4
    MSA_ILVR_B2(v16i8, src2, src0, src3, src1, src2, src3);
    src2 = (v16i8)__msa_ilvr_d((v2i64)src3, (v2i64)src2);
    src2 = (v16i8)__msa_dotp_u_h((v16u8)src2, (v16u8)src_abcd);
    src0 = (v16i8)__msa_splati_d((v2i64)src2, 1);
    src2 = (v16i8)__msa_addv_h((v8i16)src2, (v8i16)src0);
    src2 = (v16i8)__msa_srari_h((v8i16)src2, 6);
    src2 = (v16i8)__msa_pckev_b(src2, src2);
    src0 = src1;
    MSA_ST_W(src2, 0, pDst);
    pSrc += iSrcStride;
    pDst += iDstStride;
  }
}

void McChromaWidthEq8_msa(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,
                          int32_t iDstStride, const uint8_t *pABCD, int32_t iHeight) {
  int32_t i;
  v16i8 src_abcd, src_ab, src_cd, src0, src1, src2, src3;
  MSA_LD_V(v16i8, pABCD, src_abcd);
  src_abcd = (v16i8)__msa_ilvr_h((v8i16)src_abcd, (v8i16)src_abcd);
  src_abcd = (v16i8)__msa_ilvr_w((v4i32)src_abcd, (v4i32)src_abcd);//ababababcdcdcdcd
  src_ab = (v16i8)__msa_splati_d((v2i64)src_abcd, 0);
  src_cd = (v16i8)__msa_splati_d((v2i64)src_abcd, 1);
  MSA_LD_V(v16i8, pSrc, src0);
  for (i = 0; i < iHeight; i++) {
    MSA_LD_V(v16i8, pSrc + iSrcStride, src1);// 0 1 2 3 4 5 6 7 8
    src2 = __msa_sldi_b(src0, src0, 1); // 1 2 3 4 5 6 7 8
    src3 = __msa_sldi_b(src1, src1, 1);
    MSA_ILVR_B2(v16i8, src2, src0, src3, src1, src2, src3);
    src2 = (v16i8)__msa_dotp_u_h((v16u8)src2, (v16u8)src_ab);
    src3 = (v16i8)__msa_dotp_u_h((v16u8)src3, (v16u8)src_cd);
    src2 = (v16i8)__msa_addv_h((v8i16)src2, (v8i16)src3);
    src2 = (v16i8)__msa_srari_h((v8i16)src2, 6);
    src2 = (v16i8)__msa_pckev_b(src2, src2);
    src0 = src1;
    MSA_ST_D(src2, 0, pDst);
    pSrc += iSrcStride;
    pDst += iDstStride;
  }
}
