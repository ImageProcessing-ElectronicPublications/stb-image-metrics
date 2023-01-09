/***************************************************************************
****************************************************************************
*  Metrics PSNR                                                            *
*  file: metricspsnr.h                                                     *
*  version: 0.2.9                                                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __METRICS_PSNR__H
#define __METRICS_PSNR__H

#define METRICS_PSNR_VERSION "0.2.9"

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
METRICSAPI float metrics_mse_cor(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels);
METRICSAPI float metrics_psnr_cor(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels);
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

METRICSAPI float metrics_mse_cor(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels)
{
    float im1, im2;
    float sum1, sum2, sum1l, sum2l, suml, sum, error;
    float sum12, sumq1, sumq2, q12, sum12l, sumq1l, sumq2l, sumq, cor;
    int y, x, d;
    size_t k, n;

    n = width * height;

    k = 0;
    sum1 = 0.0f;
    sum2 = 0.0f;
    for (y = 0; y < height; y++)
    {
        sum1l = 0.0f;
        sum2l = 0.0f;
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < channels; d++)
            {
                im1 = (float)ref[k];
                im2 = (float)cmp[k];
                sum1l += im1;
                sum2l += im2;
                k++;
            }
        }
        sum1 += sum1l;
        sum2 += sum2l;
    }
    sum1 /= (float)k;
    sum2 /= (float)k;

    k = 0;
    sum12 = 0.0f;
    sumq1 = 0.0f;
    sumq2 = 0.0f;
    for (y = 0; y < height; y++)
    {
        sum12l = 0.0f;
        sumq1l = 0.0f;
        sumq2l = 0.0f;
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < channels; d++)
            {
                im1 = (float)ref[k];
                im1 -= sum1;
                im2 = (float)cmp[k];
                im2 -= sum2;
                q12 = (im1 * im2);
                sum12l += q12;
                sumq1l += (im1 * im1);
                sumq2l += (im2 * im2);
                k++;
            }
        }
        sum12 += sum12l;
        sumq1 += sumq1l;
        sumq2 += sumq2l;
    }
    sumq = sqrt(sumq1 * sumq2);
    if (sumq > 0.0f)
    {
        cor = sum12 / sumq;
    } else {
        cor = (sumq1 == sumq2) ? 1.0f : 0.0f;
    }
    cor = (cor < 0.0f) ? -cor : cor;
    sumq1 /= k;
    sumq2 /= k;
    sumq1 = sqrt(sumq1);
    sumq2 = sqrt(sumq2);
    q12 = (sumq1 + sumq2);
    if (q12 > 0.0f)
    {
        sumq1 *= (2.0f / q12);
        sumq2 *= (2.0f / q12);
    }
    else
    {
        sumq1 = 1.0f;
        sumq2 = 1.0f;
    }
    k = 0;
    sum = 0.0f;
    for (y = 0; y < height; y++)
    {
        suml = 0.0f;
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < channels; d++)
            {
                im1 = (float)ref[k];
                im1 -= sum1;
                im1 *= sumq2;
                im2 = (float)cmp[k];
                im2 -= sum2;
                im2 *= sumq1;
                error = (im1 > im2) ? (im1 - im2) : (im2 - im1);
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

METRICSAPI float metrics_psnr_cor(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels)
{
    int y, x, d;
    size_t k;
    float mse, psnr, t, sum1, sum2, sum1l, sum2l;

    mse = metrics_mse_cor(ref, cmp, delta, height, width, channels);
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
