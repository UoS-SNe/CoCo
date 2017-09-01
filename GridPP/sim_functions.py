"""

"""

try:
    reload  # Python 2.7
except NameError:
    try:
        from importlib import reload  # Python 3.4+
    except ImportError:
        from imp import reload  # Python 3.0 - 3.3


import sys
import numpy as np
import pandas as pd

from astropy.cosmology import LambdaCDM
from astropy.coordinates import SkyCoord, Distance
from astropy.table import Table
from astropy import units as u

from scipy.interpolate import InterpolatedUnivariateSpline

_coco_root_path = "./"
_default_info_path = "data/info/info.dat"

if sys.version_info < (3,):
    def b(x):
        return x
else:
    import codecs
    def b(x):
        return codecs.latin_1_encode(x)[0]

def choose_subtype(return_string=True):
    """
    1|Ib | 11|0.328|
    2|Ic | 9.7|0.290|
    3|IIb | 12.8|0.382|
    total - 33.5
    """
    n = 33.5 * np.random.random()

    if n <= 11:
        if return_string:
            return "Ib"
        else:
            return 1

    if 11 < n <= 11 + 9.7:
        if return_string:
            return "Ic"
        else:
            return 2

    if 11 + 9.7 < n <= 11 + 9.7 + 12.8:
        if return_string:
            return "IIb"
        else:
            return 3
    pass


def choose_extinction_host(x0=0.0, sigma=0.2, n=10000):
    """
    """
    return np.fabs(np.random.normal(loc=x0, scale=sigma, size=n))


def choose_magoffset(x0=0.0, sigma=0.2, n=10000):
    """
    """
    return np.random.normal(loc=x0, scale=sigma, size=n)


def calculate_SFR(z):
    """

    :param z:
    :return:
    """
    a = 0.0170
    b = 0.13
    c = 3.3
    d = 5.3
    h = 0.7

    sfr = (a + b * z) * h / (1. + (z / c) ** d)

    return sfr


def string_format_for_mapping(x):
    return b("LSST_" + x)


def choose_z_flat(z_max=0.6, n=1):
    """

    :param n:
    :param z_max:
    :return:
    """
    return z_max * np.random.random(n)


def choose_z_volume_SFR(n_req=10000, zmax=8.0, binsize=0.01):
    """

    :return:
    """


    z = np.arange(0.0, zmax, binsize)
    z_dz = np.arange(0.0 + binsize, zmax + binsize, binsize)

    cosmo = LambdaCDM(H0=70, Om0=0.3, Ode0=0.7)

    v_z = cosmo.comoving_volume(z)
    v_z_dz = cosmo.comoving_volume(z_dz)

    v_dz = v_z_dz - v_z

    norm_v_dz = v_dz / np.nanmax(v_dz)

    sfr_z = calculate_SFR(z)
    sfr_norm = sfr_z / np.nanmax(sfr_z)

    volumetric_rate = norm_v_dz * sfr_norm
    normed_volumetric_rate = volumetric_rate / np.nanmax(volumetric_rate)

    pdf = InterpolatedUnivariateSpline(z, normed_volumetric_rate)


    n = 0
    z_sim = []
    while n < n_req:
        x = np.random.random() * zmax
        y = np.random.random()

        if y <= pdf(x):
            z_sim.append(x)
            n += 1


    return z_sim


def choose_MJDmax(opsimdf, n=1, ndays_extra=100):
    """

    :param obslog:
    :param n:
    :return:
    """

    random_MJD = (opsimdf.expMJD.max() + 2*ndays_extra - opsimdf.expMJD.min()) * np.random.random(n) + opsimdf.expMJD.min() - ndays_extra
    return random_MJD


def load_df(inpath):
    return df

def simulate_out_to_ap_table(mjd_to_sim, flux, dflux, filters_to_sim,
                             names = ('MJD', 'flux', 'flux_err', 'filter')):
    return Table([mjd_to_sim, flux, dflux, filters_to_sim.astype(str)], names = names)


class InfoClass():
    """

    """

    def __init__(self):
        pass

    def load(self, path = False):
        if not path:
            path = _default_info_path

        self._data = Table.read(path, format = "ascii.commented_header")

        self.table = self._data

        self.table.meta["success"] = True
        self.snname = self.table["snname"]
        self.z_obs = self.table["z_obs"]
        self.distmod = self.table["mu"]
        self.distance = Distance(distmod = self.table["mu"])
        self.table["z_distmod"] = [i.z for i in self.distance]

        self.RA = self.table["RA"]
        self.Dec = self.table["Dec"]

        self.table["SkyCoords"] = SkyCoord(self.table["RA"], self.table["Dec"], unit=(u.hourangle, u.deg))
        self.coords = self.table["SkyCoords"]

        self.type = self.table["Type"]

    def get_sn_info(self, snname):
        try:
            w = np.where(self.snname == snname)
        except:
            print("foo")

        return self.table[w]
