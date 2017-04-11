import sys
import numpy
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

compile_args = ['-O3', '-std=c++11']
link_args = ['-std=c++11']
inc = [numpy.get_include(), ".", "/usr/local/include", "/usr/local/include/", "/cvmfs/lsst.opensciencegrid.org/fnal/products/Linux64/external/gsl/1.15+1/include/"]
lib = [".", "/usr/local/lib", "/usr/local/lib/", "/cvmfs/lsst.opensciencegrid.org/fnal/products/Linux64/external/gsl/1.15+1/lib/"]

if sys.platform == 'darwin':
    compile_args.append('-mmacosx-version-min=10.7')
    link_args.append('-mmacosx-version-min=10.7')

setup(
  ext_modules=[
            Extension("pyCoCo",
              sources=["python/pyCoCo.pyx",
                       "python/CoCo.cpp"
                       ],
              include_dirs=inc,
              extra_compile_args=compile_args,
              extra_link_args=link_args,
              runtime_library_dirs=lib,
              library_dirs=lib,
              libraries=["coco", "gsl"],
              language="c++"),
            ],
  cmdclass = {'build_ext': build_ext},
)
