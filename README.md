# stb-image-metrics

This utility compares two graphics files based on metrics.

Includes metrics:
- MSE / MAX^2, MAX = 255
- PSNR
- NHW neatness
- NHW convolutional
- NHW relative

The value of MSE metrics is from 0.0 (minimum difference) to 1.0 (maximum difference).

The value of PSNR metrics is from 1.0 (maximum difference) to inf.

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
- `metricsnhw.h` - NHW metrics ([origin project code](https://github.com/rcanut/NHW_Neatness_Metrics/issues/1))
- `metricspsnr.h` - metrics PSNR and MSE
- `stb/` - [stb](https://github.com/nothings/stb.git)
- `stbimmetrics.c` - CLI program.
- `ycbcr.h` - YCbCr color space.

## Metrics

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
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.nhw-n.png) NHW-N: 0.072673
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.nhw-c.png) NHW-C: 0.145290
* ![metric](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo/main/images/lena.quant444.nhw-r.png) NHW-R: 0.116387

---

See all [demo of stb-image-metrics](https://github.com/ImageProcessing-ElectronicPublications/stb-image-metrics-demo).

---
