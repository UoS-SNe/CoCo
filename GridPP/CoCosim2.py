# Electric Boogaloo
import os
import time
import numpy as np
import pandas as pd
from scipy.interpolate import InterpolatedUnivariateSpline
# import matplotlib.pyplot as plt

from sim_functions import *

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
    """
    arg1 = path to opsim csv
    arg2 = n_sne required
    """
    print[i for i in sys.argv]

    infile = sys.argv[1]
    n_req = np.int(sys.argv[2])
    print n_req
    plot = False
    verbose = False

    # n_req = 100
    z_max = 0.1

    z_binsize = 0.01
    z = np.arange(0.0, z_max, z_binsize)
    z_dz = np.arange(0.0 + z_binsize, z_max + z_binsize, z_binsize)

    cosmo = LambdaCDM(H0=70, Om0=0.3, Ode0=0.7)

    v_z = cosmo.comoving_volume(z)
    v_z_dz = cosmo.comoving_volume(z_dz)

    v_dz = v_z_dz - v_z

    norm_v_dz = v_dz / np.nanmax(v_dz)

    sfr_z = calculate_SFR(z)
    sfr_norm = sfr_z / np.nanmax(sfr_z)

    volumetric_rate = norm_v_dz * sfr_norm
    normed_volumetric_rate = volumetric_rate / np.nanmax(volumetric_rate)
    ## Generate probability density function
    pdf = InterpolatedUnivariateSpline(z, normed_volumetric_rate)

    obs_df = pd.read_csv(infile)
    filter_path = "data/filters"
    coco_root_path = "./"
    # filter_path = "/Users/berto/Code/CoCo/data/filters"
    # coco_root_path = "/Users/berto/Code/CoCo"

    coco = pyCoCo(b(filter_path), b(coco_root_path))

    n_sne = 0
    n = 0

    info = InfoClass()
    info.load()
    time_list = []

    ##
    print n_req
    while n_sne < n_req:
        print n_sne, n_req
        start = time.time()

        ##

        # z_obs = 0.007
        # mjdmax = 100.0
        # mjd_to_sim = np.array([90.0, 91.0, 92.0, 93.0, 94.0, 95.0, 96.0, 97.0, 98.0, 99.0, 100.0, 105.0, 110.0, 115.0, 120.0])
        # filters_to_sim = np.array([b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r', b'SDSS_r'])

        w = np.random.choice(obs_df["fieldID"].unique())
        run_df = obs_df.loc[obs_df["fieldID"] == w]

        mjd_to_sim = run_df.expMJD.values
        filters_to_sim = run_df.long_filter.map(b).values

        n_z = 0
        while n_z < 1:
            x = np.random.random() * z_max
            y = np.random.random()

            if y <= pdf(x):
                z_sim = (x)
                n_z += 1

        mjdmax = choose_MJDmax(run_df, n=1)[0]

        if verbose:
            print z_sim
            print mjdmax

        w_trim = np.logical_and(run_df["expMJD"] < mjdmax + 200, run_df["expMJD"] > mjdmax - 50)

        # print w_trim
        mjd_to_sim = mjd_to_sim[w_trim.values]
        filters_to_sim = filters_to_sim[w_trim.values]
        # mjdmax = mjdmax - np.nanmin(mjd_to_sim)
        # mjd_to_sim = mjd_to_sim - np.nanmin(mjd_to_sim)
        # out = coco.simulate(b"SN2009jf",
        # out = coco.simulate(b"SN2009jf",
        flux, dflux = coco.simulate(b"SN1998bw",
                                    z_sim, -0.0, 0.1, 0.1, 3.1,
                                    mjdmax, mjd_to_sim,
                                    filters_to_sim)

        # out = coco.simulate(b"SN2009jf",
        # out = coco.simulate(b"SN2011dh",
        #                     z_sim, -0.0, 0.1, 0.1, 3.1,
        #                     mjdmax, mjd_to_sim,
        #                     filters_to_sim)
        # out = [0,0]
        # print out[0]
        # print out[1]

        #print flux
        #print dflux

        phot = simulate_out_to_ap_table(mjd_to_sim, flux, dflux, filters_to_sim)

        #print phot

        outpath = os.path.join(os.pardir, "SN_" + str(w) + "_" + str(n_sne + 1).rjust(9, "0") + ".dat")

        if verbose:
            print outpath

        phot.write(outpath, format="ascii.fast_commented_header", overwrite=True)

        end = time.time()
        time_list.append(end - start)

        n_sne += 1
        n += 1

        if plot:
            fig = plt.figure(figsize=[8, 4])
            fig.subplots_adjust(left=0.09, bottom=0.13, top=0.95,
                                right=0.92, hspace=0, wspace=0)
            ax = fig.add_subplot(111)
            ax.scatter(mjd_to_sim, out[0])
            ax.plot([mjdmax, mjdmax], [0, 1.1 * np.nanmax(out[0])], ls=":", color="Black")
            ax.set_ylim(0, 1.05 * np.nanmax(out[0]))

            plt.show()

    timefile = os.path.join(os.pardir, "average_time.dat")
    ofile = open(timefile, "w")
    print(n_sne, np.nanmean(time_list), np.median(time_list), np.std(time_list), np.sum(time_list))
    ofile.write(str(n_sne) + " " + str(np.nanmean(time_list)) + " " + str(np.median(time_list)) + " " + str(
        np.std(time_list)) + " " + str(np.sum(time_list)))
    ofile.close()


else:
    pass
