/*
https://github.com/rolinh/VQMT
*/

/***************************************************************************
****************************************************************************
*  Metrics SSIM                                                            *
*  file: ssim.h                                                            *
*  version: 0.4.0                                                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#ifndef SSIM_H_
#define SSIM_H_

#define METRICS_SSIM_VERSION "0.4.0"

#ifdef METRICS_STATIC
#define METRICSAPI static
#else
#define METRICSAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
METRICSAPI float metric_ssim(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels);
METRICSAPI float metric_vifp1(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels);
#ifdef __cplusplus
}
#endif

#ifdef METRICS_SSIM_IMPLEMENTATION

static float gaussblur(float *vec, float *gaussmat, int iradius)
{
    int j;
    float imc, sc;

    //gauss 1d
    imc = vec[iradius - 1];
    sc = imc * gaussmat[0];
    for (j = 1; j < iradius; j++)
    {
        imc = vec[iradius - 1 - j];
        sc += imc * gaussmat[j];
        imc = vec[iradius - 1 + j];
        sc += imc * gaussmat[j];
    }

    return sc;
}

METRICSAPI float metric_ssim(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels)
{
    size_t k, kx[13], ky[13], kf, line, size;
    int y, x, d, yf, xf;
    int i, j, j0, iradius = 7;
    float imc, sc, radius = 1.5f;
    float gaussmat[7], vec1[13], v21[13], v22[13], v23[13], v24[13], v25[13];
    float sum, suml, t, tt;
    float g1, g2, s1, s2, s12;
    float iq1, iq2, iq12, gq1, gq2, gq12;
    float t1, t2, fs, fg, ssim = 0.0f;
    float C1 = 6.5025f, C2 = 58.5225f;

    line = width * channels;
    size = height * line;

    //gaussmat
    for (i = 0; i < iradius; i++)
    {
        sum = 0;
        for (j = 0; j <= 50; j++)
        {
            t = (float)i;
            t -= 0.5f;
            t += 0.02f * j;
            tt = -(t * t) / (2.0f * radius * radius);
            sum += exp(tt);
        }
        gaussmat[i] = sum / 50.0f;
    }
    sum = gaussmat[0];
    for (i = 1; i < iradius; i++)
    {
        sum += 2.0f * gaussmat[i];
    }
    for (i = 0; i < iradius; i++)
    {
        gaussmat[i] /= sum;
    }

    //ssim
    sum = 0.0f;
    for (d = 0; d < channels; d++)
    {
        k = d;
        for (y = 0; y < height; y++)
        {
            ky[6] = k;
            for (i = 1; i < 7; i++)
            {

                ky[6 - i] = ky[6 - i + 1] - (((y - i) < 0) ? 0 : line);
                ky[6 + i] = ky[6 + i - 1] + (((y + i) < height) ? line : 0);
            }
            suml = 0.0f;
            for (x = 0; x < width; x++)
            {
                kx[6] = k - ky[6];
                for (j = 1; j < 7; j++)
                {

                    kx[6 - j] = kx[6 - j + 1] - (((x - j) < 0) ? 0 : channels);
                    kx[6 + j] = kx[6 + j - 1] + (((x + j) < width) ? channels : 0);
                }
                //optimize gauss blur
                j0 = (x < 13) ? x : 12;
                for (j = 0; j < j0; j++)
                {
                    v21[j] = v21[j + 1];
                    v22[j] = v22[j + 1];
                    v23[j] = v23[j + 1];
                    v24[j] = v24[j + 1];
                    v25[j] = v25[j + 1];
                }
                //gauss of ref = g1
                for (j = j0; j < 13; j++)
                {
                    for (i = 0; i < 13; i++)
                    {
                        kf = ky[i] + kx[j];
                        vec1[i] = (float)ref[kf];
                    }
                    //gauss 1d
                     v21[j] = gaussblur(vec1, gaussmat, 7);
                }
                //gauss 1d
                g1 = gaussblur(v21, gaussmat, 7);
                //gauss of cmp = g2
                for (j = j0; j < 13; j++)
                {
                    for (i = 0; i < 13; i++)
                    {
                        kf = ky[i] + kx[j];
                        vec1[i] = (float)cmp[kf];
                    }
                    //gauss 1d
                     v22[j] = gaussblur(vec1, gaussmat, 7);
                }
                //gauss 1d
                g2 = gaussblur(v22, gaussmat, 7);
                //gauss of ref*ref = s1
                for (j = j0; j < 13; j++)
                {
                    for (i = 0; i < 13; i++)
                    {
                        kf = ky[i] + kx[j];
                        vec1[i] = (float)ref[kf] * (float)ref[kf];
                    }
                    //gauss 1d
                     v23[j] = gaussblur(vec1, gaussmat, 7);
                }
                //gauss 1d
                s1 = gaussblur(v23, gaussmat, 7);
                //gauss of cmp*cmp = s2
                for (j = j0; j < 13; j++)
                {
                    for (i = 0; i < 13; i++)
                    {
                        kf = ky[i] + kx[j];
                        vec1[i] = (float)cmp[kf]*(float)cmp[kf];
                    }
                    //gauss 1d
                     v24[j] = gaussblur(vec1, gaussmat, 7);
                }
                //gauss 1d
                s2 = gaussblur(v24, gaussmat, 7);
                //gauss of ref*cmp = s12
                for (j = j0; j < 13; j++)
                {
                    for (i = 0; i < 13; i++)
                    {
                        kf = ky[i] + kx[j];
                        vec1[i] = (float)ref[kf]*(float)cmp[kf];
                    }
                    //gauss 1d
                     v25[j] = gaussblur(vec1, gaussmat, 7);
                }
                //gauss 1d
                s12 = gaussblur(v25, gaussmat, 7);
                // ssim_map = fs * fg;
                // fs = (2*s12 + C2))/(s1 + s2 + C2);
                // fg = (2*gq12 + C1)/(gq1 + gq2 + C1);
                iq1 = (float)ref[k] * (float)ref[k];
                iq2 = (float)cmp[k] * (float)cmp[k];
                iq12 = (float)ref[k] * (float)cmp[k];
                gq1 = g1 * g1;
                gq2 = g2 * g2;
                gq12 = g1 * g2;
                s1 -= gq1;
                s2 -= gq2;
                s12 -= gq12;
                t1 = 2.0f * s12 + C2;
                t2 = s1 + s2 + C2;
                fs = (t2 > 0.0f) ? (t1 / t2) : 1.0f;
                t1 = 2.0f * gq12 + C1;
                t2 = gq1 + gq2 + C1;
                fg = (t2 > 0.0f) ? (t1 / t2) : 1.0f;
                ssim = fs * fg;
                suml += ssim;
                if (delta)
                {
                    ssim *= 255.0f;
                    delta[k] = (unsigned char)((ssim < 0.0f) ? 0 : (ssim < 255.0f) ? ssim : 255);
                }
                k += channels;
            }
            sum += suml;
        }
    }
    sum /= (float)k;

    return sum;
}

METRICSAPI float metric_vifp1(unsigned char *ref, unsigned char *cmp, unsigned char* delta, int height, int width, int channels)
{
    size_t k, kx[13], ky[13], kf, line, size;
    int y, x, d, yf, xf;
    int i, j, j0, iradius = 7;
    float imc, sc, radius = 1.5f;
    float gaussmat[7], vec1[13], v21[13], v22[13], v23[13], v24[13], v25[13];
    float sum, suml, t, tt;
    float g1, g2, s1, s2, s12;
    float iq1, iq2, iq12, gq1, gq2, gq12;
    float g, sv, nd, dd, numl, denl, num, den, vifp1 = 0.0f;;
    float eps = 1e-10f, sigma_nsq = 2.0f;

    line = width * channels;
    size = height * line;

    //gaussmat
    for (i = 0; i < iradius; i++)
    {
        sum = 0;
        for (j = 0; j <= 50; j++)
        {
            t = (float)i;
            t -= 0.5f;
            t += 0.02f * j;
            tt = -(t * t) / (2.0f * radius * radius);
            sum += exp(tt);
        }
        gaussmat[i] = sum / 50.0f;
    }
    sum = gaussmat[0];
    for (i = 1; i < iradius; i++)
    {
        sum += 2.0f * gaussmat[i];
    }
    for (i = 0; i < iradius; i++)
    {
        gaussmat[i] /= sum;
    }

    //vifp1
    k = 0;
    num = 0.0f;
    den = 0.0f;
    for (d = 0; d < channels; d++)
    {
        k = d;
        for (y = 0; y < height; y++)
        {
            ky[6] = k;
            for (i = 1; i < 7; i++)
            {

                ky[6 - i] = ky[6 - i + 1] - (((y - i) < 0) ? 0 : line);
                ky[6 + i] = ky[6 + i - 1] + (((y + i) < height) ? line : 0);
            }
            numl = 0.0f;
            denl = 0.0f;
            for (x = 0; x < width; x++)
            {
                kx[6] = k - ky[6];
                for (j = 1; j < 7; j++)
                {

                    kx[6 - j] = kx[6 - j + 1] - (((x - j) < 0) ? 0 : channels);
                    kx[6 + j] = kx[6 + j - 1] + (((x + j) < width) ? channels : 0);
                }
                //optimize gauss blur
                j0 = (x < 13) ? x : 12;
                for (j = 0; j < j0; j++)
                {
                    v21[j] = v21[j + 1];
                    v22[j] = v22[j + 1];
                    v23[j] = v23[j + 1];
                    v24[j] = v24[j + 1];
                    v25[j] = v25[j + 1];
                }
                //gauss of ref = g1
                for (j = j0; j < 13; j++)
                {
                    for (i = 0; i < 13; i++)
                    {
                        kf = ky[i] + kx[j];
                        vec1[i] = (float)ref[kf];
                    }
                    //gauss 1d
                     v21[j] = gaussblur(vec1, gaussmat, 7);
                }
                //gauss 1d
                g1 = gaussblur(v21, gaussmat, 7);
                //gauss of cmp = g2
                for (j = j0; j < 13; j++)
                {
                    for (i = 0; i < 13; i++)
                    {
                        kf = ky[i] + kx[j];
                        vec1[i] = (float)cmp[kf];
                    }
                    //gauss 1d
                     v22[j] = gaussblur(vec1, gaussmat, 7);
                }
                //gauss 1d
                g2 = gaussblur(v22, gaussmat, 7);
                //gauss of ref*ref = s1
                for (j = j0; j < 13; j++)
                {
                    for (i = 0; i < 13; i++)
                    {
                        kf = ky[i] + kx[j];
                        vec1[i] = (float)ref[kf] * (float)ref[kf];
                    }
                    //gauss 1d
                     v23[j] = gaussblur(vec1, gaussmat, 7);
                }
                //gauss 1d
                s1 = gaussblur(v23, gaussmat, 7);
                //gauss of cmp*cmp = s2
                for (j = j0; j < 13; j++)
                {
                    for (i = 0; i < 13; i++)
                    {
                        kf = ky[i] + kx[j];
                        vec1[i] = (float)cmp[kf]*(float)cmp[kf];
                    }
                    //gauss 1d
                     v24[j] = gaussblur(vec1, gaussmat, 7);
                }
                //gauss 1d
                s2 = gaussblur(v24, gaussmat, 7);
                //gauss of ref*cmp = s12
                for (j = j0; j < 13; j++)
                {
                    for (i = 0; i < 13; i++)
                    {
                        kf = ky[i] + kx[j];
                        vec1[i] = (float)ref[kf]*(float)cmp[kf];
                    }
                    //gauss 1d
                     v25[j] = gaussblur(vec1, gaussmat, 7);
                }
                //gauss 1d
                s12 = gaussblur(v25, gaussmat, 7);
                iq1 = (float)ref[k] * (float)ref[k];
                iq2 = (float)cmp[k] * (float)cmp[k];
                iq12 = (float)ref[k] * (float)cmp[k];
                gq1 = g1 * g1;
                gq2 = g2 * g2;
                gq12 = g1 * g2;
                s1 -= gq1;
                s2 -= gq2;
                s12 -= gq12;
                //prefilter
                s1 = (s1 < 0.0f) ? 0.0f : s1;
                s2 = (s2 < 0.0f) ? 0.0f : s2;
                g = s12 / (s1 + eps);
                sv = s2 - g * s12;
                g = (s1 < eps) ? 0.0f : g;
                sv = (s1 < eps) ? s2 : sv;
                s1 = (s1 < eps) ? 0.0f : s1;
                g = (s2 < eps) ? 0.0f : g;
                sv = (s2 < eps) ? 0.0f : sv;
                sv = (g < 0.0f) ? s2 : sv;
                g = (g < 0.0f) ? 0.0f : g;
                sv = (sv < eps) ? eps : sv;
                //vifp1
                // num=num+sum(sum(log10(1+g.^2.*s1./(sv+sigma_nsq))));
                nd = log10(1.0f + g * g * s1 /(sv + sigma_nsq)) / log(10.0f);
                numl += nd;
                // den=den+sum(sum(log10(1+s1./sigma_nsq)));
                dd = log10(1.0f + s1 / sigma_nsq) / log(10.0f);
                denl += dd;
                if (delta)
                {
                    vifp1 = (dd > 0.0f) ? (nd / dd) : 1.0f;
                    vifp1 *= 255.0f;
                    delta[k] = (unsigned char)((vifp1 < 0.0f) ? 0 : (vifp1 < 255.0f) ? vifp1 : 255);
                }
                k += channels;
            }
            num += numl;
            den += denl;
        }
    }
    sum = (den > 0.0f) ? (num / den) : 1.0f;

    return sum;
}

#endif /* METRICS_SSIM_IMPLEMENTATION */

#endif /* SSIM_H_ */
