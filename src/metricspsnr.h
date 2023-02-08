/***************************************************************************
****************************************************************************
*  Metrics PSNR                                                            *
*  file: metricspsnr.h                                                     *
*  version: 0.4.0                                                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __METRICS_PSNR__H
#define __METRICS_PSNR__H

#define METRICS_PSNR_VERSION "0.4.0"

#ifdef METRICS_STATIC
#define METRICSAPI static
#else
#define METRICSAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
METRICSAPI float metrics_mse(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels, int corel);
METRICSAPI float metrics_psnr(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels, int corel);
METRICSAPI float metrics_sdsnr(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels, int corel);
#ifdef __cplusplus
}
#endif

#ifdef METRICS_PSNR_IMPLEMENTATION

/* MSE(a,b) = 1/N * SUM(((a-b)/MAX)^2), MAX = 255 */
METRICSAPI float metrics_mse(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels, int corel)
{
    float im1, im2;
    float sum1, sum2, sum1l, sum2l, suml, sum, error;
    float sum12, sumq1, sumq2, q12, sum12l, sumq1l, sumq2l, sumq, cor;
    int y, x, d;
    size_t k;

    sum1 = 0.0f;
    sum2 = 0.0f;
    sumq1 = 1.0f;
    sumq2 = 1.0f;
    // Correlation ref and cmp
    if (corel)
    {
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
    }

    // MSE
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
                error = im2 - im1;
                suml += error * error;
                if (delta)
                {
                    error *= 0.5f;
                    error += 128.0f;
                    error = (error < 0.0f) ? 0.0f : (error < 255.0f) ? error : 255.0f;
                    delta[k] = (unsigned char)error;
                }
                k++;
            }
        }
        sum += suml;
    }
    sum /= (float)k;
    sum /= 255.0f;
    sum /= 255.0f;

    return sum;
}

/* PSNR(a,b) = 10*log10(1 / MSE(a,b)) */
METRICSAPI float metrics_psnr(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels, int corel)
{
    int y, x, d;
    size_t k;
    float mse, psnr, t;

    mse = metrics_mse(ref, cmp, delta, height, width, channels, corel);
    psnr = (mse > 0.0f) ? (10.0f * log10( 1.0f / mse)) : 0.0f;

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
                    t -= 0.5f;
                    t *= 2.0f;
                    t *= t;
                    t += mse;
                    t *= 0.5f;
                    t = (t > 0.0f) ? (10.0f * log10( 1.0f / t)) : 255.0f;
                    t *= 3.0f;
                    t = (t < 0.0f) ? 0.0f : (t < 255.0f) ? t : 255.0f;
                    delta[k] = (unsigned char)t;
                    k++;
                }
            }
        }
    }

    return psnr;
}

/* StDev2(a) = 1/N * (SUM((a/MAX)^2) - SUM(1/N * (a/MAX))^2) */
float stdev2(unsigned char *ref, int height, int width, int channels)
{
    float sum2l, sum2, suml, sum, val;
    size_t k;
    int y, x, d;

    k = 0;
    sum2 = 0.0f;
    sum = 0.0f;
    for (y = 0; y < height; y++)
    {
        sum2l = 0.0f;
        suml = 0.0f;
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < channels; d++)
            {
                val = (float)ref[k];
                sum2l += (val * val);
                suml += val;
                k++;
            }
        }
        sum2 += sum2l;
        sum += suml;
    }
    sum /= (float)k;
    sum *= sum;
    sum2 -= sum;
    sum2 /= (float)k;

    return sum2;
}

/* SDSNR(a,b) = 10*log10((S2(a) + S2(b)) / MSE(a,b) / 2)
 * S2(a) = StDev2(a)
 * S2(b) = StDev2(b)
 * MSE(a,b) = 1/N * SUM(((a-b)/MAX)^2), MAX = 255 */
METRICSAPI float metrics_sdsnr(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels, int corel)
{
    int y, x, d;
    size_t k;
    float s2a, s2b, s2, mse, sdsnr, t;

    s2a = stdev2(ref, height, width, channels);
    s2b = stdev2(cmp, height, width, channels);
    s2 = 0.5f * (s2a + s2b);
    mse = metrics_mse(ref, cmp, delta, height, width, channels, corel);
    mse *= 255.0f;
    mse *= 255.0f;

    sdsnr = (mse > 0.0f) ? (10.0f * log10(s2 / mse)) : 0.0f;

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
                    t -= 127.5f;
                    t *= 2.0f;
                    t *= t;
                    t += mse;
                    t *= 0.5f;
                    t = (t > 0.0f) ? (10.0f * log10(s2 / t)) : 255.0f;
                    t *= 3.0f;
                    delta[k] = (unsigned char)((t < 0.0f) ? 0.0f : (t < 255.0f) ? t : 255);
                    k++;
                }
            }
        }
    }

    return sdsnr;
}

#endif /* METRICS_PSNR_IMPLEMENTATION */

#endif // __METRICS_PSNR__H
