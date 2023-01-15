# stb-image-metrics

This utility compares two graphics files based on metrics.

Includes metrics:

- MSE / MAX^2, MAX = 255
- PSNR
- SDSNR
- [SSIM](https://github.com/rolinh/VQMT)
- [VIFP1](https://github.com/rolinh/VQMT)
- [SMALLFRY](https://github.com/dwbuiten/smallfry)
- [SHARPENBAD](https://github.com/ImageProcessing-ElectronicPublications/libsmallfry)
- [Correlation](https://github.com/ImageProcessing-ElectronicPublications/libsmallfry)
- [NHW neatness](https://github.com/rcanut/NHW_Neatness_Metrics)
- [NHW convolutional](https://github.com/rcanut/NHW_Neatness_Metrics/issues/1)
- [NHW relative](https://github.com/rcanut/NHW_Neatness_Metrics/issues/1)

The value of MSE metrics is from 0.0 (minimum difference) to 1.0 (maximum difference).

The value of PSNR and SDSNR metrics is from 0.0 (maximum difference) to inf.

The value of [SSIM](https://github.com/rolinh/VQMT) and [VIFP1](https://github.com/rolinh/VQMT) metrics is from 1.0 (minimum difference) to 0.0 (maximum difference).

The value of [SHARPENBAD](https://github.com/ImageProcessing-ElectronicPublications/libsmallfry) metrics is from 1.0 (minimum difference) to -1.0 (maximum difference).

The value of [Correlation](https://github.com/ImageProcessing-ElectronicPublications/libsmallfry) metrics is from 1.0 (minimum difference) to 0.0 (maximum difference).

The value of [NHW metrics](https://github.com/rcanut/NHW_Neatness_Metrics/issues/1) is from 0.0 (minimum difference) to 1.0 (maximum difference).

## build

### load submodules

submodules:

- [stb](https://github.com/nothings/stb.git) -> [src/stb](src/stb)

```shell
$ git submodule init
$ git submodule update
```

### install dependencies

build dependencies:

- build-essential
- cmake

```shell
$ sudo apt-get install build-essential cmake
```

### compilation
```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
```
## use

The first and second parameters indicate the paths to the original and compared images. The third (optional) specifies the path to the metric image {PNG}.

```shell
stbnhwmetrics ${IMAGE_ORIG_PATH} ${IMAGE_COMPARE_PATH} [metric.out.png]
```

## structure

- `dependencies.c` - API [stb](https://github.com/nothings/stb.git)
- `metricsnhw.h` - [NHW metrics](https://github.com/rcanut/NHW_Neatness_Metrics/issues/1)
- `metricspsnr.h` - metrics PSNR and MSE
- `metricsum.h` - ["Universal Scale" of metrics (UM)](https://github.com/ImageProcessing-ElectronicPublications/jpeg-recompress)
- `smallfry.h` - metrics [SMALLFRY, SHARPENBAD and Correlation](https://github.com/ImageProcessing-ElectronicPublications/libsmallfry)
- `ssim.h` - metrics [SSIM and VIFP1](https://github.com/rolinh/VQMT)
- `stb/` - [stb](https://github.com/nothings/stb.git)
- `stbimmetrics.c` - CLI program.
- `ycbcr.h` - [YCbCr color space](https://en.wikipedia.org/wiki/YCbCr).

## Metrics

"Universal Scale" of metrics (UM):

```
  0.0
  ... (DIRTY) ...
  0.5
  ... (LOW) ...
  0.75
  ... (MEDIUM) ...
  0.875
  ... (SUBHIGH) ...
  0.9375
  ... (HIGH) ...
  0.96875
  ... (VERYHIGH) ...
  1.0
```

Trends:

```
  UM = 0.158 * sqrt(sqrt(1.0 / MSE)) - 0.63
  UM = 0.99 * sqrt(PNSR) - 5.28
  UM = 0.85 * sqrt(SDNSR) - 3.84
  UM = 9.14 * cor_sigma(cor_sigma(cor_sigma(SSIM))) + 0.30
  UM = 2.36 * cor_sigma(VIFP1) + 0.32
  UM = 0.0777 * SMALLFRY - 6.93
  UM = 1.53 * SHARPENBAD - 0.09
  UM = 4.14 * cor_sigma(cor_sigma(COR)) - 1.94
  UM = -0.055 * sqrt(sqrt(1.0 / NHW-N)) + 0.82
  UM = 1.43 * sqrt(sqrt(1.0 / NHW-C)) - 2.82
  UM = 1.03 * sqrt(sqrt(1.0 / NHW-R)) - 2.17

  cor_sigma(M) = 1.0 - sqrt(1.0 - M * M)
```

Origin and Compare:

![origin](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.png) ![compare](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.png)

Metrcics:

```shell
stbimmetrics lena.png lena.quant444.png lena.quant444.psnr.png 
Load: lena.png
image: 256x256:3
Load: lena.quant444.png
image: 256x256:3
type: psnr
metric: 18.663233
Save png: lena.quant444.psnr.png

```

* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.mse.png) MSE: 0.013604, UM: -0.167366
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.psnr.png) PSNR: 18.663233, UM: -1.003105
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.sdsnr.png) SDSNR: 13.888971, UM: -0.672228
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.cor.png) Corelation: 0.953666, UM: -0.760071
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.ssim.png) SSIM: 0.618509, UM: 0.302463
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.vifp1.png) VIFP1: 0.170903, UM: 0.354721
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.smallfry.png) SMALLFRY: 83.753342, UM: -0.422366
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.shbad.png) SHARPENBAD: -0.213132, UM: -0.416092
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.nhw-n.png) NHW-N: 0.072673, UM: 0.714070
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.nhw-c.png) NHW-C: 0.145290, UM: -0.503793
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.nhw-r.png) NHW-R: 0.116387, UM: -0.406557

---

See all [demo of stb-image-metrics](https://github.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo).

---
