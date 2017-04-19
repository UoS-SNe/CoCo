import os
import numpy as np
from pyCoCo import pyCoCo

import sys
if sys.version_info < (3,):
    def b(x):
        return x
else:
    import codecs
    def b(x):
        return codecs.latin_1_encode(x)[0]

if __name__ == "__main__":
    filter_path = "data/filters"
    coco_root_path = "./"
    # filter_path = "/Users/berto/Code/CoCo/data/filters"
    # coco_root_path = "/Users/berto/Code/CoCo"

    coco = pyCoCo(b(filter_path), b(coco_root_path))

    z_obs = 0.007
    mjdmax = 100.0
    mjd_to_sim = np.array([90.0, 91.0, 92.0, 93.0, 94.0, 95.0, 96.0, 97.0, 98.0, 99.0, 100.0, 105.0, 110.0, 115.0, 120.0])
    filters_to_sim = np.array([b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r'])


    out = coco.simulate(b"SN2009jf",
                        z_obs, -0.0, 0.1, 0.1, 3.1,
                        mjdmax, mjd_to_sim,
                        filters_to_sim)

    print out[0]
    print out[1]

else:
    pass
