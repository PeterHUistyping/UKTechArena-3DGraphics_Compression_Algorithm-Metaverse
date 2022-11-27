
import argparse
import numpy as np
import imageio.v3 as iio

from sewar.full_ref import uqi
from sewar.full_ref import psnr

def image_quality_metric(
    ref_image,
    mod_image
):
    ref_im = iio.imread(ref_image)
    mod_im = iio.imread(mod_image)
    #return uqi(ref_im, mod_im)
    return psnr(ref_im, mod_im)

if __name__ == '__main__':
    # Read and parse arguments from command line
    parser = argparse.ArgumentParser()

    parser.add_argument("-r", "--Ref", help = "Original Image File")
    parser.add_argument("-m", "--Mod", help = "Modified Image File")

    args = parser.parse_args()

    quality = image_quality_metric(args.Ref,args.Mod)
    if quality is np.inf:
        quality = 100
    print(quality)
