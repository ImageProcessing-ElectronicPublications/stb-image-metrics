/***************************************************************************
****************************************************************************
*  Metrics UM                                                              *
*  file: metricsum.h                                                       *
*  version: 0.3.0                                                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef __METRICS_UM__H
#define __METRICS_UM__H

#define METRICS_VERSION "0.3.0"

#ifdef METRICS_STATIC
#define METRICSAPI static
#else
#define METRICSAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
METRICSAPI float metric_um(char* metric, float value);
METRICSAPI float metric_um_image(unsigned char* delta, int height, int width, int channels, char* metric, float umval);
#ifdef __cplusplus
}
#endif

#ifdef METRICS_UM_IMPLEMENTATION

float metricssigma(float cor)
{
    float sigma;

    cor = (cor < 0.0f) ? -cor : cor;
    sigma = cor;
    if (cor > 1.0f)
    {
        cor = 1.0f / cor;
        sigma = 1.0f - sqrt(1.0f - cor * cor);
        sigma = 1.0f / sigma;
    } else {
        sigma = 1.0f - sqrt(1.0f - cor * cor);
    }

    return sigma;
}

/* Universal scale of Metrics */
METRICSAPI float metric_um(char* metric, float value)
{
    if ((strcmp(metric, "mse") == 0) || (strcmp(metric, "mse-c") == 0))
    {
        if (value > 0.0f)
        {
            value = 1.0f / value;
            value = sqrt(value);
            value = sqrt(value);
            value *= 0.148f;
            value -= 0.53f;
        }
        else
        {
            value = 1.0f;
        }
    }
    else if ((strcmp(metric, "psnr") == 0) || (strcmp(metric, "psnr-c") == 0))
    {
        value = sqrt(value);
        value *= 0.94f;
        value -= 4.94f;
    }
    else if (strcmp(metric, "ssim") == 0)
    {
        value = metricssigma(value);
        value = metricssigma(value);
        value = metricssigma(value);
        value *= 8.57f;
        value += 0.34f;
    }
    else if (strcmp(metric, "vifp1") == 0)
    {
        value = metricssigma(value);
        value *= 2.21f;
        value += 0.36f;
    }
    else if (strcmp(metric, "smallfry") == 0)
    {
        value *= 0.0735f;
        value -= 6.50f;
    }
    else if (strcmp(metric, "shbad") == 0)
    {
        value *= 1.46f;
        value -= 0.04f;
    }
    else if (strcmp(metric, "cor") == 0)
    {
        value = metricssigma(value);
        value = metricssigma(value);
        value *= 3.92f;
        value -= 1.79f;
    }
    else if (strcmp(metric, "nhw-c") == 0)
    {
        if (value > 0.0f)
        {
            value = 1.0f / value;
            value = sqrt(value);
            value = sqrt(value);
            value *= 1.36f;
            value -= 2.61f;
        }
        else
        {
            value = 1.0f;
        }
    }
    else if (strcmp(metric, "nhw-n") == 0)
    {
        if (value > 0.0f)
        {
            value = 1.0f / value;
            value = sqrt(value);
            value = sqrt(value);
            value *= -0.049f;
            value += 0.83f;
        }
        else
        {
            value = 1.0f;
        }
    }
    else if (strcmp(metric, "nhw-r") == 0)
    {
        if (value > 0.0f)
        {
            value = 1.0f / value;
            value = sqrt(value);
            value = sqrt(value);
            value *= 0.96f;
            value -= 1.98f;
        }
        else
        {
            value = 1.0f;
        }
    }

    return value;
}

METRICSAPI float metric_um_image(unsigned char* delta, int height, int width, int channels, char* metric, float umval)
{
    float value, suml, sum = 0.0f;
    size_t k;
    int y, x, d, cval;


    if (delta)
    {
        k = 0;
        sum = 0;
        for (y = 0; y < height; y++)
        {
            suml = 0.0f;
            for (x = 0; x < width; x++)
            {
                for (d = 0; d < channels; d++)
                {
                    value = (float)delta[k];
                    value /= 255;
                    if ((strcmp(metric, "mse") == 0) || (strcmp(metric, "mse-c") == 0))
                    {
                        value *= value;
                    }
                    else if ((strcmp(metric, "psnr") == 0) || (strcmp(metric, "psnr-c") == 0))
                    {
                        value *= 255.0f;
                    }
                    else if (strcmp(metric, "shbad") == 0)
                    {
                        value -= 0.5f;
                        value *= 2.0f;
                    }
                    else if (strcmp(metric, "smallfry") == 0)
                    {
                        value *= 255.0f;
                    }
                    else if (strcmp(metric, "nhw-n") == 0)
                    {
                        value -= 0.5f;
                    }
                    value = metric_um(metric, value);
                    value -= umval;
                    value *= 127.5;
                    value += 0.5f;
                    cval = (int)((value < 0.0f) ? 0 : (value < 255.0f) ? value : 255);
                    value -= 0.5f;
                    suml += (value > cval) ? (value - (float)cval) : ((float)cval - value);
                    if (delta) delta[k] = (unsigned char)cval;
                    k++;
                }
            }
            sum += suml;
        }
        sum /= (float)k;
        sum /= 255.0f;
    }

    return sum;
}

#endif /* METRICS_UM_IMPLEMENTATION */

#endif // __METRICS_UM__H
