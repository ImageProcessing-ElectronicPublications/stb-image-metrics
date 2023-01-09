/***************************************************************************
****************************************************************************
*  Metrics UM                                                              *
*  file: metricsum.h                                                       *
*  version: 0.2.8                                                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef __METRICS_UM__H
#define __METRICS_UM__H

#define METRICS_VERSION "0.2.8"

#ifdef METRICS_STATIC
#define METRICSAPI static
#else
#define METRICSAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
METRICSAPI float metric_um(char* metric, float value);
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
    if (strcmp(metric, "mse") == 0)
    {
        value *= -216.75f;
        value += 1.73f;
    }
    else if (strcmp(metric, "psnr") == 0)
    {
        value = sqrt(value);
        value *= 1.10f;
        value -= 6.07f;
    }
    else if (strcmp(metric, "ssim") == 0)
    {
        value = metricssigma(value);
        value = metricssigma(value);
        value = metricssigma(value);
        value *= 2.38f;
        value -= 0.24f;
    }
    else if (strcmp(metric, "ms-ssim") == 0)
    {
        value = metricssigma(value);
        value = metricssigma(value);
        value *= 1.87f;
        value -= 0.02f;
    }
    else if (strcmp(metric, "smallfry") == 0)
    {
        value *= 0.0747f;
        value -= 6.91f;
    }
    else if (strcmp(metric, "shbad") == 0)
    {
        value *= 1.48f;
        value -= 0.26f;
    }
    else if (strcmp(metric, "cor") == 0)
    {
        value = metricssigma(value);
        value = metricssigma(value);
        value *= 3.00f;
        value -= 1.50f;
    }
    else if (strcmp(metric, "nhw-c") == 0)
    {
        if (value > 0.0f)
        {
            value = 1.0f / value;
            value = sqrt(value);
            value = sqrt(value);
            value *= 0.56f;
            value -= 0.95f;
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
            value *= 0.62f;
            value -= 1.05f;
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
            value *= 0.59f;
            value -= 1.12f;
        }
        else
        {
            value = 1.0f;
        }
    }

    return value;
}

#endif /* METRICS_UM_IMPLEMENTATION */

#endif // __METRICS_UM__H
