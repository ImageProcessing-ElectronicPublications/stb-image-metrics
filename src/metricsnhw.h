/*
https://github.com/rcanut/NHW_Neatness_Metrics/issues/1
*/

/***************************************************************************
****************************************************************************
*  Metrics NHW                                                             *
*  file: metricsnhw.h                                                      *
*  version: 0.2.4                                                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __METRICS_NHW__H
#define __METRICS_NHW__H

#define METRICSNHW_VERSION "0.2.4"

#ifdef METRICS_STATIC
#define METRICSAPI static
#else
#define METRICSAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
METRICSAPI float metrics_nhw_neatness(unsigned char* image_ref, unsigned char* image_comp, unsigned char* image_mnhw, int height, int width, int channels);
METRICSAPI float metrics_nhw_convolutional(unsigned char* image_ref, unsigned char* image_comp, unsigned char* image_mnhw, int height, int width, int channels);
METRICSAPI float metrics_nhw_relative(unsigned char* image_ref, unsigned char* image_comp, unsigned char* image_mnhw, int height, int width, int channels);
#ifdef __cplusplus
}
#endif

#ifdef METRICS_NHW_IMPLEMENTATION

METRICSAPI float metrics_nhw_neatness(unsigned char* image_ref, unsigned char* image_comp, unsigned char* image_mnhw, int height, int width, int channels)
{
    int y, x, i, res, res2, n1, comp, delta;
    size_t scan, lp, ln,s0, s1, s2, s3, s5, s6, s7, s8;
    int line = width * channels;
    int NEATNESS_THRESHOLD1, NEATNESS_THRESHOLD2;
    int NEATNESS_PERCEPTION, CONTOURS;
    float neatness, sum, suml, suma, sumal;

    NEATNESS_PERCEPTION = 160;
    NEATNESS_THRESHOLD1 = 30;
    NEATNESS_THRESHOLD2 = 3;
    CONTOURS = 14;

    sum = 0.0f;
    suma = 0.0f;
    for (comp = 0; comp < channels; comp++)
    {
        scan = line + channels + comp;
        for (y = 1; y < (height - 1); y++)
        {
            suml = 0.0f;
            sumal = 0.0f;
            for (x = 1; x < (width - 1); x++)
            {
                lp = scan - line;
                ln = scan + line;
                s0 = lp - channels;
                s1 = lp;
                s2 = lp + channels;
                s3 = scan - channels;
                s5 = scan + channels;
                s6 = ln - channels;
                s7 = ln;
                s8 = ln + channels;
                res = ((int)image_comp[scan] << 3);
                res = (image_comp[scan] << 3);
                res -= image_comp[s0];
                res -= image_comp[s1];
                res -= image_comp[s2];
                res -= image_comp[s3];
                res -= image_comp[s5];
                res -= image_comp[s6];
                res -= image_comp[s7];
                res -= image_comp[s8];
// If neatness pixel
                if (abs(res) > NEATNESS_PERCEPTION)
                {
// Detect contours
                    n1 = 0;
                    if (abs(image_comp[scan]-image_comp[s0])<CONTOURS) n1++;
                    if (abs(image_comp[scan]-image_comp[s1])<CONTOURS) n1++;
                    if (abs(image_comp[scan]-image_comp[s2])<CONTOURS) n1++;
                    if (abs(image_comp[scan]-image_comp[s3])<CONTOURS) n1++;
                    if (abs(image_comp[scan]-image_comp[s5])<CONTOURS) n1++;
                    if (abs(image_comp[scan]-image_comp[s6])<CONTOURS) n1++;
                    if (abs(image_comp[scan]-image_comp[s7])<CONTOURS) n1++;
                    if (abs(image_comp[scan]-image_comp[s8])<CONTOURS) n1++;
// If contours
                    if (n1 > 3)
                    {
// Compare neatness with reference image contours
                        res2 = (image_ref[scan] << 3);
                        res2 -= image_ref[s0];
                        res2 -= image_ref[s1];
                        res2 -= image_ref[s2];
                        res2 -= image_ref[s3];
                        res2 -= image_ref[s5];
                        res2 -= image_ref[s6];
                        res2 -= image_ref[s7];
                        res2 -= image_ref[s8];
                        if (abs(res2) > (NEATNESS_PERCEPTION >> 1))
                        {
                            delta = (res > 0) ? (image_comp[scan] - image_ref[scan]) : (image_ref[scan] - image_comp[scan]);
                            if (delta >= NEATNESS_THRESHOLD2 && delta <= NEATNESS_THRESHOLD1)
                            {
                                if (image_mnhw) image_mnhw[scan] = 128 + delta;
                                suml += delta * delta;
                                sumal++;
                            }
                        }
                    }
                 }
                scan += channels;
            }
            sum += suml;
            suma += sumal;
            scan += channels;
            scan += channels;
        }
    }
    sum = (suma > 0.0f) ? (sum / suma) : 0.0f;
    neatness = sqrt(sum) / 255.0f;

    return neatness;
}

METRICSAPI float metrics_nhw_convolutional(unsigned char* image_ref, unsigned char* image_comp, unsigned char* image_mnhw, int height, int width, int channels)
{
    int y, x, res, res2, resd, comp, delta, dsq;
    size_t scan, lp, ln, s0, s1, s2, s3, s5, s6, s7, s8;
    int line = width * channels;
    float neatness, sum, suml, suma, sumal;

    sum = 0.0f;
    suma = 0.0f;
    for (comp = 0; comp < channels; comp++)
    {
        scan = line + channels + comp;
        for (y = 1; y < (height - 1); y++)
        {
            suml = 0.0f;
            sumal = 0.0f;
            for (x = 1; x < (width - 1); x++)
            {
                lp = scan - line;
                ln = scan + line;
                s0 = lp - channels;
                s1 = lp;
                s2 = lp + channels;
                s3 = scan - channels;
                s5 = scan + channels;
                s6 = ln - channels;
                s7 = ln;
                s8 = ln + channels;
                res = (image_comp[scan] << 3);
                res -= image_comp[s0];
                res -= image_comp[s1];
                res -= image_comp[s2];
                res -= image_comp[s3];
                res -= image_comp[s5];
                res -= image_comp[s6];
                res -= image_comp[s7];
                res -= image_comp[s8];
                res2 = (image_ref[scan] << 3);
                res2 -= image_ref[s0];
                res2 -= image_ref[s1];
                res2 -= image_ref[s2];
                res2 -= image_ref[s3];
                res2 -= image_ref[s5];
                res2 -= image_ref[s6];
                res2 -= image_ref[s7];
                res2 -= image_ref[s8];
                resd = (res > res2) ? (res - res2) : (res2 - res);
                if (resd > 0)
                {
                    delta = (image_comp[scan] > image_ref[scan]) ? (image_comp[scan] - image_ref[scan]) : (image_ref[scan] - image_comp[scan]);
                    if (delta > 0)
                    {
                        dsq = delta * delta * resd;
                        suml += (float)dsq;
                        dsq = sqrt(dsq) + 0.5f;
                        if (image_mnhw) image_mnhw[scan] = (unsigned char)((dsq < 255.0f) ? dsq : 255);
                    }
                    sumal += (float)resd;
                }
                scan += channels;
            }
            sum += suml;
            suma += sumal;
            scan += channels;
            scan += channels;
        }
    }
    sum = (suma > 0.0f) ? (sum / suma) : 0.0f;
    neatness = sqrt(sum) / 255.0f;

    return neatness;
}

METRICSAPI float metrics_nhw_relative(unsigned char* image_ref, unsigned char* image_comp, unsigned char* image_mnhw, int height, int width, int channels)
{
    int y, x, res, res2, resd, ress, comp, delta;
    size_t scan, lp, ln, s0, s1, s2, s3, s5, s6, s7, s8;
    int line = width * channels;
    float dsq, dres;
    float neatness, sum, suml, suma, sumal;

    sum = 0.0f;
    suma = 0.0f;
    for (comp = 0; comp < channels; comp++)
    {
        scan = line + channels + comp;
        for (y = 1; y < (height - 1); y++)
        {
            suml = 0.0f;
            sumal = 0.0f;
            for (x = 1; x < (width - 1); x++)
            {
                lp = scan - line;
                ln = scan + line;
                s0 = lp - channels;
                s1 = lp;
                s2 = lp + channels;
                s3 = scan - channels;
                s5 = scan + channels;
                s6 = ln - channels;
                s7 = ln;
                s8 = ln + channels;
                res = (image_comp[scan] << 3);
                res -= image_comp[s0];
                res -= image_comp[s1];
                res -= image_comp[s2];
                res -= image_comp[s3];
                res -= image_comp[s5];
                res -= image_comp[s6];
                res -= image_comp[s7];
                res -= image_comp[s8];
                res2 = (image_ref[scan] << 3);
                res2 -= image_ref[s0];
                res2 -= image_ref[s1];
                res2 -= image_ref[s2];
                res2 -= image_ref[s3];
                res2 -= image_ref[s5];
                res2 -= image_ref[s6];
                res2 -= image_ref[s7];
                res2 -= image_ref[s8];
                resd = (res > res2) ? (res - res2) : (res2 - res);
                if (resd > 0)
                {
                    ress = (res < 0) ? -res : res;
                    ress += (res2 < 0) ? -res2 : res2;
                    dres = 2.0f * (float)resd / (float)ress;
                    delta = (image_comp[scan] > image_ref[scan]) ? (image_comp[scan] - image_ref[scan]) : (image_ref[scan] - image_comp[scan]);
                    if (delta > 0)
                    {
                        dsq = delta * delta * dres;
                        suml += dsq;
                        dsq = sqrt(dsq) + 0.5f;
                        if (image_mnhw) image_mnhw[scan] = (unsigned char)((dsq < 255.0f) ? dsq : 255);
                    }
                    sumal += dres;
                }
                scan += channels;
            }
            sum += suml;
            suma += sumal;
            scan += channels;
            scan += channels;
        }
    }
    sum = (suma > 0.0f) ? (sum / suma) : 0.0f;
    neatness = sqrt(sum) / 255.0f;

    return neatness;
}

#endif /* METRICS_NHW_IMPLEMENTATION */

#endif // __METRICS_NHW__H
