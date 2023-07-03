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
 * \file    score_msa.c
 *
 * \brief   Loongson optimization
 *
 * \date    11/04/2023 Created
 *
 *************************************************************************************
 */
#include <stdint.h>
#include "msa_macros.h"

void WelsScan4x4Ac_msa(int16_t *zig_value, int16_t *pDct) {
  v8i16 src0, src1, out0, out1;
  v8i16 mask0 = {1,4,8,5,2,3,6,9};
  v8i16 mask1 = {12,13,10,7,11,14,15,0};
  MSA_LD_V2(v8i16, pDct, 8, src0, src1);
  MSA_VSHF_H2(v8i16, src1, src0, src1, src0, mask0, mask1, out0, out1);
  out1 = __msa_insert_h(out1, 7, 0);
  MSA_ST_V2(v8i16, out0, out1, zig_value, 8);
}

void WelsScan4x4DcAc_msa(int16_t *zig_value, int16_t *pDct) {
  v8i16 src0, src1, out0, out1;
  v8i16 mask0 = {0,1,4,8,5,2,3,6};
  v8i16 mask1 = {9,12,13,10,7,11,14,15};
  MSA_LD_V2(v8i16, pDct, 8, src0, src1);
  MSA_VSHF_H2(v8i16, src1, src0, src1, src0, mask0, mask1, out0, out1);
  MSA_ST_V2(v8i16, out0, out1, zig_value, 8);
}
