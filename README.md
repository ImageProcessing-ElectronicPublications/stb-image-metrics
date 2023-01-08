# stb-image-metrics

This utility compares two graphics files based on metrics.

Includes metrics:

- MSE / MAX^2, MAX = 255
- PSNR
- [SMALLFRY](https://github.com/dwbuiten/smallfry)
- [SHARPENBAD](https://github.com/ImageProcessing-ElectronicPublications/libsmallfry)
- [Correlation](https://github.com/ImageProcessing-ElectronicPublications/libsmallfry)
- [NHW neatness](https://github.com/rcanut/NHW_Neatness_Metrics)
- [NHW convolutional](https://github.com/rcanut/NHW_Neatness_Metrics/issues/1)
- [NHW relative](https://github.com/rcanut/NHW_Neatness_Metrics/issues/1)

The value of MSE metrics is from 0.0 (minimum difference) to 1.0 (maximum difference).

The value of PSNR metrics is from 1.0 (maximum difference) to inf.

The value of SHARPENBAD metrics is from 1.0 (minimum difference) to -1.0 (maximum difference).

The value of Correlation metrics is from 1.0 (minimum difference) to 0.0 (maximum difference).

The value of NHW metrics is from 0.0 (minimum difference) to 1.0 (maximum difference) ([origin project code](https://github.com/rcanut/NHW_Neatness_Metrics/issues/1)).

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
  UM = -216.75*MPE+1.73
  UM = 1.10*sqrt(PNSR)-6.07
  UM = 2.38*cor_sigma(cor_sigma(cor_sigma(SSIM)))-0.24
  UM = 1.87*cor_sigma(cor_sigma(MS_SSIM))-0.02
  UM = 0.0747*SMALLFRY-6.91
  UM = 1.48*SHARPENBAD-0.26
  UM = 3.00*COR-1.50
  UM = 0.56*sqrt(sqrt(1.0/NHW-C))-0.95
  UM = 0.62*sqrt(sqrt(1.0/NHW-N))-1.05
  UM = 0.59*sqrt(sqrt(1.0/NHW-R))-1.12

  cor_sigma(M) = 1.0-sqrt(1.0-M*M)
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

* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.mse.png) MSE: 0.013604
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.psnr.png) PSNR: 18.663233
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.smallfry.png) SMALLFRY: 83.753342
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.shbad.png) SHARPENBAD: -0.213132
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.cor.png) Corelation: 0.953666
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.nhw-n.png) NHW-N: 0.072673
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.nhw-c.png) NHW-C: 0.145290
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.nhw-r.png) NHW-R: 0.116387

---

See all [demo of stb-image-metrics](https://github.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo).

---
