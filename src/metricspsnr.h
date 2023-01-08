/***************************************************************************
****************************************************************************
*  Metrics PSNR                                                            *
*  file: metricspsnr.h                                                     *
*  version: 0.2.6                                                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __METRICS_PSNR__H
#define __METRICS_PSNR__H

#define METRICS_PSNR_VERSION "0.2.6"

#ifdef METRICS_STATIC
#define METRICSAPI static
#else
#define METRICSAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
METRICSAPI float metrics_mse(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels);
METRICSAPI float metrics_psnr(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels);
#ifdef __cplusplus
}
#endif

#ifdef METRICS_PSNR_IMPLEMENTATION

/* MSE(a,b) = 1/N * SUM(((a-b)/MAX)^2), MAX = 255 */
METRICSAPI float metrics_mse(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels)
{
    float error, suml, sum;
    size_t k;
    int y, x, d;

    k = 0;
    sum = 0.0f;
    for (y = 0; y < height; y++)
    {
        suml = 0.0f;
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < channels; d++)
            {
                error = (float)((ref[k] > cmp[k]) ? (ref[k] - cmp[k]) : (cmp[k] - ref[k]));
                suml += error * error;
                if (delta) delta[k] = (unsigned char)error;
                k++;
            }
        }
        sum += suml;
    }
    sum /= (float)height;
    sum /= (float)width;
    sum /= (float)channels;
    sum /= 255.0f;
    sum /= 255.0f;

    return sum;
}

/* PSNR(a,b) = 10*log10(1 / MSE(a,b)) */
METRICSAPI float metrics_psnr(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels)
{
    int y, x, d;
    size_t k;
    float mse, psnr, t;

    mse = metrics_mse(ref, cmp, delta, height, width, channels);
    psnr = 10.0f * log10( 1.0f / mse);

    if (delta)
    {
        k = 0;
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                for (d = 0; d < channels; d++)
                {
                    t = (float)delta[k];
                    t /= 255.0f;
                    t *= t;
                    t += mse;
                    t *= 0.5f;
                    t = (t > 0.0f) ? (10.0f * log10( 1.0f / t)) : 255.0f;
                    delta[k] = (unsigned char)((t < 255.0f) ? t : 255);
                    k++;
                }
            }
        }
    }

    return psnr;
}

#endif /* METRICS_PSNR_IMPLEMENTATION */

#endif // __METRICS_PSNR__H
