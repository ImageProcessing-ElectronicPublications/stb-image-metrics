/*
 * Copyright (c) 2014, Derek Buitenhuis
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/***************************************************************************
****************************************************************************
*  Metrics PSNR                                                            *
*  file: metricspsnr.h                                                     *
*  version: 0.3.1                                                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#ifndef __METRICS_SMALLFRY__H
#define __METRICS_SMALLFRY__H

#define METRICS_SMALLFRY_VERSION "0.3.1"

#ifdef METRICS_STATIC
#define METRICSAPI static
#else
#define METRICSAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
METRICSAPI float metric_smallfry (unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels);
METRICSAPI float metric_sharpenbad (unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels);
METRICSAPI float metric_cor (unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels);
#ifdef __cplusplus
}
#endif

#ifdef METRICS_SMALLFRY_IMPLEMENTATION

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static float factor_psnr (unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels, unsigned char max, float part)
{
    float ret, sum, suml, mse, error;
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
    mse = sum;

    ret  = 10.0f * log10(1.0f / mse);

    if (max > 128)
        ret /= 50.0f;
    else
        ret /= (0.0016f * (float) (max * max)) - (0.38f * (float) max + 72.5f);
    ret = MAX(MIN(ret, 1.0f), 0.0f);
    ret *= part;
    if (delta)
    {
        k = 0;
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                for (d = 0; d < channels; d++)
                {
                    error = (float)((ref[k] > cmp[k]) ? (ref[k] - cmp[k]) : (cmp[k] - ref[k]));
                    error /= 255.0f;
                    error *= error;
                    error += mse;
                    error *= 0.5f;
                    error = (error > 0.0f) ? (10.0f * log10( 1.0f / error)) : 255.0f;
                    if (max > 128)
                        error /= 50.0f;
                    else
                        error /= (0.0016f * (float) (max * max)) - (0.38f * (float) max + 72.5f);
                    error *= part;
                    delta[k] = (unsigned char)((error < 255.0f) ? error : 255);
                    k++;
                }
            }
        }
    }

    return ret;
}

static float factor_aae (unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels, unsigned char max, float part)
{
    float ret, sum, suml, dt, calc, cfmax, cf;
    size_t k, kt, line, cnt;
    int y, x, d, i, ia, ib, di[4], d0, d1, d2;

    line = width * channels;

    if (max > 128)
        cfmax = 0.65f;
    else
        cfmax = 1.0f - 0.35f * (float) max / 128.0f;

    cnt = 0;
    k = 0;
    sum = 0.0f;
    for (y = 0; y < height; y++)
    {
        suml = 0.0f;
        for (x = 7; x < width - 2; x += 8)
        {
            k = (y * width + x) * channels;
            for (d = 0; d < channels; d++)
            {
                cnt++;
                kt = k + d - channels;
                for (i = 0; i < 4; i++)
                {
                    ia = (int)ref[kt];
                    ib = (int)cmp[kt];
                    di[i] = (ia > ib) ? (ia - ib) : (ib - ia);
                    kt += channels;
                }
                d0 = (di[1] > di[0]) ? (di[1] - di[0]) : (di[0] - di[1]);
                d1 = (di[1] > di[2]) ? (di[1] - di[2]) : (di[2] - di[1]);
                d2 = (di[3] > di[2]) ? (di[3] - di[2]) : (di[2] - di[3]);
                calc = 2.0f * (float)d1 / (0.0001f + (float)d0 + (float)d2);
                dt = 0.0f;
                if (calc > 5.0f)
                    dt = 1.0f;
                else if (calc > 2.0f)
                    dt = (calc - 2.0f) / (5.0f - 2.0f);
                suml += dt;

                if (delta)
                {
                    dt *= part;
                    dt *= cfmax;
                    dt += (float)delta[k + d];
                    delta[k + d] = (unsigned char)((dt < 255.0f) ? dt : 255);
                }
            }
        }
        sum += suml;
    }
    for (y = 7; y < height - 2; y += 8)
    {
        suml = 0.0f;
        for (x = 0; x < width; x ++)
        {
            k = (y * width + x) * channels;
            for (d = 0; d < channels; d++)
            {
                cnt++;
                kt = k + d - line;
                for (i = 0; i < 4; i++)
                {
                    ia = (int)ref[kt];
                    ib = (int)cmp[kt];
                    di[i] = (ia > ib) ? (ia - ib) : (ib - ia);
                    kt += line;
                }
                d0 = (di[1] > di[0]) ? (di[1] - di[0]) : (di[0] - di[1]);
                d1 = (di[1] > di[2]) ? (di[1] - di[2]) : (di[2] - di[1]);
                d2 = (di[3] > di[2]) ? (di[3] - di[2]) : (di[2] - di[3]);
                calc = 2.0f * (float)d1 / (0.0001f + (float)d0 + (float)d2);
                dt = 0.0f;
                if (calc > 5.0f)
                    dt = 1.0f;
                else if (calc > 2.0f)
                    dt = (calc - 2.0f) / (5.0f - 2.0f);
                suml += dt;
                if (delta)
                {
                    dt *= part;
                    dt *= cfmax;
                    dt += (float)delta[k + d];
                    delta[k + d] = (unsigned char)((dt < 255.0f) ? dt : 255);
                }
            }
        }
        sum += suml;
    }

    ret = 1.0f - (sum / (float) cnt);

    cf = MAX(cfmax, MIN(1.0f, 0.25f + (1000.0f * (float) cnt) / sum));
    ret *= cf;
    ret *= part;

    return ret;
}

static unsigned char maxluma (unsigned char *ref, int height, int width, int channels)
{
    size_t k;
    int y, x, d;
    unsigned char max;

    k = 0;
    max = 0;
    for (y = 0; y < height; y++)
    {
         for (x = 0; x < width; x++)
        {
            for (d = 0; d < channels; d++)
            {
                max = MAX(ref[k], max);
                k++;
            }
        }
    }

    return max;
}

METRICSAPI float metric_smallfry (unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels)
{
    float p, a, b;
    uint8_t max;

    max = maxluma(ref, height, width, channels);

    p = factor_psnr(ref, cmp, delta, height, width, channels, max, 37.1891885161239f);
    a = factor_aae(ref, cmp, delta, height, width, channels, max, 78.5328607296973f);

    b = p + a;

    return b;
}

METRICSAPI float metric_sharpenbad (unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels)
{
    float sharpenbad, exp1n, k332, k255p;
    float im1, im2, imf1, imf2, ims1, ims2, imd, imd1, imd2, imdc;
    float sumd, sumd1, sumd2, sumdc, sumdl, sumd1l, sumd2l, sumdcl;
    int y, x, d, y0, x0, y1, x1, y2, x2;
    size_t k, ky0, ky, kx, n, line;

    line = width * channels;
    exp1n = exp(-1);
    k332 = (3.0f * 3.0f * 2.0f + 1.0f) / (3.0f * 3.0f * 2.0f - 1.0f);
    k255p = 1.0f / 255.0f;

    k = 0;
    sumd = 0.0f;
    sumd1 = 0.0f;
    sumd2 = 0.0f;
    sumdc = 0.0f;
    for (y = 0; y < height; y++)
    {
        sumdl = 0.0f;
        sumd1l = 0.0f;
        sumd2l = 0.0f;
        sumdcl = 0.0f;
        y0 = y - 1;
        if (y0 < 0) {y0 = 0;}
        y2 = y + 2;
        if (y2 > height) {y2 = height;}
        ky0 = y0 * width;
        for (x = 0; x < width; x++)
        {
            x0 = x - 1;
            if (x0 < 0) {x0 = 0;}
            x2 = x + 2;
            if (x2 > width) {x2 = width;}
            for (d = 0; d < channels; d++)
            {
                im1 = (float)ref[k];
                im2 = (float)cmp[k];

                ims1 = 0.0f;
                ims2 = 0.0f;
                n = 0;
                ky = ky0;
                for (y1 = y0; y1 < y2; y1++)
                {
                    for (x1 = x0; x1 < x2; x1++)
                    {
                        kx = (ky + x1) * channels;
                        imf1 = (float)ref[kx + d];
                        ims1 += imf1;
                        imf2 = (float)cmp[kx + d];
                        ims2 += imf2;
                        n++;
                    }
                    ky += width;
                }
                ims1 /= (float)n;
                ims2 /= (float)n;
                imd1 = im1 - ims1;
                imd2 = im2 - ims2;
                im1 += imd1;
                im2 += imd2;
                imd = im1 - im2;
                imd1 *= k255p;
                imd2 *= k255p;
                imd *= k255p;
                imd *= imd;
                imdc = imd1 * imd2;
                imd1 *= imd1;
                imd2 *= imd2;
                sumdl += imd;
                sumd1l += imd1;
                sumd2l += imd2;
                sumdcl += imdc;
                if (delta)
                {
                    imd2 *= imd1;
                    if (imd2 > 0.0f)
                    {
                        imd /= imd2;
                        imd *= imdc;
                        imd *= 2.0f;
                    }
                    if (imd < 0.0f) {imd = -imd;}
                    imd = sqrt(imd);
                    imd = -imd;
                    imd *= exp1n;
                    imd += k332;
                    imd *= 127.5f;
                    imd += 127.5f;
                    delta[k] = (unsigned char)((imd < 0.0f) ? 0 : (imd < 255.0f) ? imd : 255);
                }
                k++;
            }
        }
        sumd += sumdl;
        sumd1 += sumd1l;
        sumd2 += sumd2l;
        sumdc += sumdcl;
    }
    sumd2 *= sumd1;
    if (sumd2 > 0.0f)
    {
        sumd /= sumd2;
        sumd *= sumdc;
        sumd *= 2.0f;
    } else {
        sumd /= (float)height;
        sumd /= (float)width;
        sumd /= (float)channels;
    }
    if (sumd < 0.0f) {sumd = -sumd;}
    sumd = sqrt(sumd);
    sumd = -sumd;
    sumd *= exp1n;
    sumd += k332;

    sharpenbad = sumd;

    return sharpenbad;
}

METRICSAPI float metric_cor (unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels)
{
    float im1, im2;
    float sum1, sum2, sum1l, sum2l;
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
    if (delta)
    {
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
        for (y = 0; y < height; y++)
        {
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
                    q12 = (im1 > im2) ? (im1 - im2) : (im2 - im1);
                    q12 = 255.0f - q12;
                    delta[k] = (unsigned char)((q12 < 255.0f) ? q12 : 255);
                    k++;
                }
            }
        }
    }

    return cor;
}

#endif /* METRICS_SMALLFRY_IMPLEMENTATION */

#endif // __METRICS_SMALLFRY__H
