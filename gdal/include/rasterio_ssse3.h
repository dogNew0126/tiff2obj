/******************************************************************************
 *
 * Project:  GDAL Core
 * Purpose:  SSSE3 specializations
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2016, Even Rouault <even dot rouault at spatialys dot com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef RASTERIO_SSSE3_H_INCLUDED
#define RASTERIO_SSSE3_H_INCLUDED

#include "cpl_port.h"

#if defined(HAVE_SSSE3_AT_COMPILE_TIME) && ( defined(__x86_64) || defined(_M_X64) )

void GDALUnrolledCopy_GByte_3_1_SSSE3( GByte* CPL_RESTRICT pDest,
                                       const GByte* CPL_RESTRICT pSrc,
                                       GPtrDiff_t nIters );

#endif

#endif /* RASTERIO_SSSE3_H_INCLUDED */
