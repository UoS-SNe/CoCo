import sys
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

compile_args = ['-O3', '-std=c++11', '-stdlib=libc++']
link_args = ['-std=c++11', '-stdlib=libc++']
if sys.platform == 'darwin':
    compile_args.append('-mmacosx-version-min=10.7')
    link_args.append('-mmacosx-version-min=10.7')

setup(
  name = 'pyCoCo',
  ext_modules=[
    Extension("pyCoCo",
              sources=["python/pyCoCo.pyx",
                       "python/CoCo.cpp"
                       ],
              extra_compile_args=compile_args,
              extra_link_args=link_args,
              language="c++"),
    ],
  cmdclass = {'build_ext': build_ext},
)

    #    "src/core/Cosmology.cpp",
    #    "src/core/Filters.cpp",
    #    "src/core/SN.cpp",
    #    "src/core/Model.cpp",
    #    "src/core/Solver.cpp"
