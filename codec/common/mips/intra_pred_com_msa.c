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
 * \file    intra_pred_com_msa.c
 *
 * \brief   Loongson optimization
 *
 * \date    04/07/2023 Created
 *
 *************************************************************************************
 */
#include <stdint.h>
#include "msa_macros.h"

void WelsI16x16LumaPredV_msa (uint8_t* pPred, uint8_t* pRef, const int32_t kiStride) {
  v16i8 src0;
  MSA_LD_V(v16i8, pRef - kiStride, src0);
  MSA_ST_V8(v16i8, src0, src0, src0, src0, src0, src0, src0, src0, pPred, 16);
  pPred += 16*8;
  MSA_ST_V8(v16i8, src0, src0, src0, src0, src0, src0, src0, src0, pPred, 16);
}

void WelsI16x16LumaPredH_msa (uint8_t* pPred, uint8_t* pRef, const int32_t kiStride) {
  v16i8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16i8 src8, src9, src10, src11, src12, src13, src14, src15;
  pRef -= 1;
  src0 = __msa_fill_b(pRef[0]);
  src1 = __msa_fill_b(pRef[kiStride]);
  src2 = __msa_fill_b(pRef[kiStride*2]);
  src3 = __msa_fill_b(pRef[kiStride*3]);
  src4 = __msa_fill_b(pRef[kiStride*4]);
  src5 = __msa_fill_b(pRef[kiStride*5]);
  src6 = __msa_fill_b(pRef[kiStride*6]);
  src7 = __msa_fill_b(pRef[kiStride*7]);
  src8 = __msa_fill_b(pRef[kiStride*8]);
  src9 = __msa_fill_b(pRef[kiStride*9]);
  src10 = __msa_fill_b(pRef[kiStride*10]);
  src11 = __msa_fill_b(pRef[kiStride*11]);
  src12 = __msa_fill_b(pRef[kiStride*12]);
  src13 = __msa_fill_b(pRef[kiStride*13]);
  src14 = __msa_fill_b(pRef[kiStride*14]);
  src15 = __msa_fill_b(pRef[kiStride*15]);
  MSA_ST_V8(v16i8, src0, src1, src2, src3, src4, src5, src6, src7, pPred, 16);
  pPred += 16*8;
  MSA_ST_V8(v16i8, src8, src9, src10, src11, src12, src13, src14, src15, pPred, 16);
}
