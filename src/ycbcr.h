/***************************************************************************
****************************************************************************
*  YCbCr Filter                                                            *
*  file: ycbcr.h                                                           *
*  version: 0.2.7                                                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#ifndef __YCBCR__H
#define __YCBCR__H

#define YCBCR_VERSION "0.2.7"

#ifdef METRICS_STATIC
#define METRICSAPI static
#else
#define METRICSAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
METRICSAPI void RGBtoYCbCrFilter(unsigned char *src, int height, int width, int channels, int direct);
#ifdef __cplusplus
}
#endif

#ifdef YCBCR_IMPLEMENTATION

METRICSAPI void RGBtoYCbCrFilter(unsigned char *src, int height, int width, int channels, int direct)
{
    int i, j, Y, cb, cr, r, g, b;
    size_t k;

    if (channels < 3) return;
    k = 0;
    if (direct < 0)
    {
        for (i = 0; i < height; i++)
        {
            for (j = 0; j < width; j++)
            {
                Y = (int)src[k + 0];
                cb = (int)src[k + 1];
                cr = (int)src[k + 2];
                r = Y + 1.402f * (cr - 128);
                g = Y - 0.344136f * (cb - 128) - 0.714136f * (cr - 128);
                b = Y + 1.772f * (cb - 128);
                src[k + 0] = (r < 0) ? 0 : (r < 255) ? r : 255;
                src[k + 1] = (g < 0) ? 0 : (g < 255) ? g : 255;
                src[k + 2] = (b < 0) ? 0 : (b < 255) ? b : 255;
                k += channels;
            }
        }
    }
    else
    {
        for (i = 0; i < height; i++)
        {
            for (j = 0; j < width; j++)
            {
                r = src[k + 0];
                g = src[k + 1];
                b = src[k + 2];
                Y = 0.299f  * r + 0.587f * g + 0.114f * b;
                cb = 128.0f - 0.168736f * r - 0.331264f * g + 0.5f * b;
                cr = 128.0f + 0.5f * r - 0.418688f * g - 0.081312f * b;
                src[k + 0] = (Y < 0) ? 0 : (Y < 255) ? Y : 255;
                src[k + 1] = (cb < 0) ? 0 : (cb < 255) ? cb : 255;
                src[k + 2] = (cr < 0) ? 0 : (cr < 255) ? cr : 255;
                k += channels;
            }
        }
    }
}

#endif /* __YCBCR_IMPLEMENTATION */

#endif /* __YCBCR__H_ */
