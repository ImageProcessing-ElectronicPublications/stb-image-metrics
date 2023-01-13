#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "metricsum.h"
#include "ycbcr.h"
#include "metricspsnr.h"
#include "ssim.h"
#include "smallfry.h"
#include "metricsnhw.h"

void immetrics_usage(char* prog, char* metric)
{
    printf("StbImMetrics version %s.\n", METRICS_VERSION);
    printf("usage: %s [options] image_in_orig image_in_comp [metric_out.png]\n", prog);
    printf("options:\n");
    printf("\t-m STR\tmetric (default %s):\n", metric);
    printf("\t\tmse      -\t MSE / MAX^2, MAX = 255\n");
    printf("\t\tpsnr     -\t PSNR\n");
    printf("\t\tmse-c    -\t MSE / MAX^2 of correlation, MAX = 255\n");
    printf("\t\tpsnr-c   -\t PSNR of correlation\n");
    printf("\t\tssim     -\t SSIM\n");
    printf("\t\tsmallfry -\t SMALLFRY\n");
    printf("\t\tshbad    -\t SHARPENBAD\n");
    printf("\t\tcor      -\t correlation\n");
    printf("\t\tvifp1    -\t VIFP 1 layer\n");
    printf("\t\tnhw-n    -\t NHW neatness\n");
    printf("\t\tnhw-c    -\t NHW convolutional\n");
    printf("\t\tnhw-r    -\t NHW relative\n");
    printf("\t-q\tquiet mode\n");
    printf("\t-u\tUM mode (Universal scale of Metrics)\n");
    printf("\t-y\tYCbCr mode\n");
    printf("\t-h\tshow this help message and exit\n");
}

int main(int argc, char **argv)
{
    int height = 0, width = 0, channels = 0;
    int height_c = 0, width_c = 0, channels_c = 0;
    char *metric = "psnr";
    int fhelp = 0;
    int fquiet = 0;
    int fum = 0;
    int fycbcr = 0;
    int opt;
    const char *name_orig = NULL, *name_comp = NULL, *name_m = NULL;
    stbi_uc *img_orig = NULL, *img_comp = NULL;
    unsigned char *data_orig = NULL, *data_comp = NULL, *data_m = NULL;
    size_t ki = 0, kd = 0;
    float neatness = 0.0f;

    while ((opt = getopt(argc, argv, ":m:quyh")) != -1)
    {
        switch(opt)
        {
        case 'm':
            metric = optarg;
            break;
        case 'q':
            fquiet = 1;
            break;
        case 'u':
            fum = 1;
            break;
        case 'y':
            fycbcr = 1;
            break;
        case 'h':
            fhelp = 1;
            break;
        case ':':
            fprintf(stderr, "ERROR: option needs a value\n");
            return 2;
            break;
        case '?':
            fprintf(stderr, "ERROR: unknown option: %c\n", optopt);
            return 3;
            break;
        }
    }
    if(optind + 2 > argc || fhelp)
    {
        immetrics_usage(argv[0], metric);
        return 0;
    }
    name_orig = argv[optind];
    name_comp = argv[optind + 1];
    if(optind + 2 < argc)
    {
        name_m = argv[optind + 2];;
    }

    if (!fquiet) printf("Load: %s\n", name_orig);
    if (!(img_orig = stbi_load(name_orig, &width, &height, &channels, STBI_rgb_alpha)))
    {
        fprintf(stderr, "ERROR: not read image: %s\n", name_orig);
        return 1;
    }
    if (!fquiet) printf("image: %dx%d:%d\n", width, height, channels);
    if (!fquiet) printf("Load: %s\n", name_comp);
    if (!(img_comp = stbi_load(name_comp, &width_c, &height_c, &channels_c, STBI_rgb_alpha)))
    {
        fprintf(stderr, "ERROR: not read image: %s\n", name_comp);
        return 1;
    }
    if (!fquiet) printf("image: %dx%d:%d\n", width_c, height_c, channels_c);

    if ((width != width_c) || (height != height_c) || (channels != channels_c))
    {
        fprintf(stderr, "ERROR: Sizes don't match.\n");
        stbi_image_free(img_orig);
        stbi_image_free(img_comp);
        return 1;
    }

    if (!(data_orig = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char))))
    {
        fprintf(stderr, "ERROR: not use memmory.\n");
        return 1;
    }
    ki = 0;
    kd = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (int d = 0; d < channels; d++)
            {
                data_orig[kd + d] = (unsigned char)img_orig[ki + d];
            }
            ki += STBI_rgb_alpha;
            kd += channels;
        }
    }
    stbi_image_free(img_orig);
    if (!(data_comp = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char))))
    {
        fprintf(stderr, "ERROR: not use memmory.\n");
        return 1;
    }
    ki = 0;
    kd = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (int d = 0; d < channels; d++)
            {
                data_comp[kd + d] = (unsigned char)img_comp[ki + d];
            }
            ki += STBI_rgb_alpha;
            kd += channels;
        }
    }
    stbi_image_free(img_comp);
    if (name_m)
    {
        if (!(data_m = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char))))
        {
            fprintf(stderr, "ERROR: not use memmory.\n");
            return 1;
        }
    }

    if (!fquiet)
    {
        if (metric)
        {
            printf("type: %s\n", metric);
        }
    }
    if (fycbcr)
    {
        RGBtoYCbCrFilter(data_orig, height, width, channels, 1);
        RGBtoYCbCrFilter(data_comp, height, width, channels, 1);
        if (!fquiet)
        {
            printf("colorspace: YCbCr\n");
        }
    }
    if (metric)
    {
        if (strcmp(metric, "mse") == 0)
        {
            neatness = metrics_mse(data_orig, data_comp, data_m, height, width, channels);
        }
        else if (strcmp(metric, "psnr") == 0)
        {
            neatness = metrics_psnr(data_orig, data_comp, data_m, height, width, channels);
        }
        else if (strcmp(metric, "mse-c") == 0)
        {
            neatness = metrics_mse_cor(data_orig, data_comp, data_m, height, width, channels);
        }
        else if (strcmp(metric, "psnr-c") == 0)
        {
            neatness = metrics_psnr_cor(data_orig, data_comp, data_m, height, width, channels);
        }
        else if (strcmp(metric, "smallfry") == 0)
        {
            neatness = metric_smallfry(data_orig, data_comp, data_m, height, width, channels);
        }
        else if (strcmp(metric, "ssim") == 0)
        {
            neatness = metric_ssim(data_orig, data_comp, data_m, height, width, channels);
        }
        else if (strcmp(metric, "shbad") == 0)
        {
            neatness = metric_sharpenbad(data_orig, data_comp, data_m, height, width, channels);
        }
        else if (strcmp(metric, "cor") == 0)
        {
            neatness = metric_cor(data_orig, data_comp, data_m, height, width, channels);
        }
        else if (strcmp(metric, "vifp1") == 0)
        {
            neatness = metric_vifp1(data_orig, data_comp, data_m, height, width, channels);
        }
        else if (strcmp(metric, "nhw-c") == 0)
        {
            neatness = metrics_nhw_convolutional(data_orig, data_comp, data_m, height, width, channels);
        }
        else if (strcmp(metric, "nhw-n") == 0)
        {
            neatness = metrics_nhw_neatness(data_orig, data_comp, data_m, height, width, channels);
        }
        else if (strcmp(metric, "nhw-r") == 0)
        {
            neatness = metrics_nhw_relative(data_orig, data_comp, data_m, height, width, channels);
        }
        else
        {
            fprintf(stderr, "ERROR: unknow metric %s.\n", metric);
            return 3;
        }
    }
    if (fum)
    {
        neatness = metric_um(metric, neatness);
        if (name_m)
        {
            (void)metric_um_image(data_m, height, width, channels, metric, neatness);
        }
    }
    if (!fquiet)
    {
        if (!fum)
        {
            printf("metric: %f\n", neatness);
        }
        else
        {
            printf("UM: %f\n", neatness);
        }
    }
    else
    {
        printf("%f    %s\n", neatness, name_comp);
    }
    if (name_m)
    {
        if (fycbcr)
        {
            RGBtoYCbCrFilter(data_m, height, width, channels, -1);
        }
        if (!fquiet) printf("Save png: %s\n", name_m);
        if (!(stbi_write_png(name_m, width, height, channels, data_m, width * channels)))
        {
            fprintf(stderr, "ERROR: not write image: %s\n", name_m);
            return 1;
        }
        free(data_m);
    }

    free(data_comp);
    free(data_orig);

    return 0;
}
