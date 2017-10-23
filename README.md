# CoCo - v0.7.0
Core Collapse Supernovae templates
___
Developed by Szymon Prajs (S.Prajs@soton.ac.uk).     
Originally designed by Natasha Karpenka.  
Currently mainted by Szymon Prajs and Rob Firth.   

This package is still in an active development phase and currently contains a number of known issues.
We are working to fix them as quickly as possible! Full release is expected very shortly.

___

## Installation
Future releases of the code will include a `configure` file and follow the standard GNU autoconf routine.

## Dependencies
#### `pyCoCo`
- [Cython](https://www.continuum.io/downloads)
- [Minuit2](https://seal.web.cern.ch/seal/snapshot/work-packages/mathlibs/minuit/)

#### `LCFit`, `SpecFit`, `SpecPhase`
- [GSL](https://www.gnu.org/software/gsl/)
- [Minuit2](https://seal.web.cern.ch/seal/snapshot/work-packages/mathlibs/minuit/)
- [MultiNest](https://ccpforge.cse.rl.ac.uk/gf/project/multinest/)
- LAPACK (Required for MultiNest)

## Usage
If you are already have a set of supernova templates or are using the default templates only,
you should use the `pyCoCo` python binding around `lcsim`.

To create new supernova templates run `lcfit` -> `specfit` -> `specphase` in this order.

#### LCFit
```
lcfit lightcurve_file.*
```
`lightcurve_file.*` must have four columns: MJD Flux Flux_error filter.   

or
```
lightcurve_files_list.list
```
`lightcurve_files_list.list` is a list of ligh curve files, all of which must follow the above convention.

#### SpecFit
```
specfit spectrum_file.* SN_name MJD redshift
```
or
```
specfit spectra_setup_file.list
```
`spectra_setup_file.list` must have the following columns: path_to_spectrum SN_name MJD redshift.

#### SpecPhase
```
specphase SN_redshift_list.list filter_name
```
`SN_redshift_list.list` must have three columns: SN_name redshift dist_mod

#### LCSim
```
LCSim SN_name redshift abs_mag_offset MJD_peak MJD_filters.list
```
or
```
lcsim setup_list.list
```
`setup_list.list` must contain the following columns: SN_name redshift abs_mag_offset MJD_peak MJD_filters.list
`MJD_filters.list` must contain two columns: MJD filter

------

### MINUIT
The following has been adapted from the Minuit2 docs [here](https://seal.web.cern.ch/seal/snapshot/work-packages/mathlibs/minuit/gettingStarted/autoconf.html).

#### How to use Minuit with autoconf/make

For each release of MINUIT there will be a tar.gz available which contains the code and can be run using the autoconf tools. The necessary steps to follow are:

Download the Minuit-x-x-x.tar.gz from the download page

unzip and untar the file:

`tar -zxf Minuit-x-x-x.tar.gz`

step down to the created Minuit-x.x.x directory:

`cd Minuit-x.x.x/`

run the "configure" script:

`./configure --prefix=/usr/local`

run make to compile the source code:

`sudo make install`

If the compilation gets stuck with a message like "`Waiting for AnalyticalGradientCalculator.o.lock to be removed`" then you are probably trying to compile under AFS. In this case go to the previous step and run:

`./configure --disable-libtool-lock `

If the compilation fails with an error message like `MnUserTransformation.cpp: In method MnUserTransformation::MnUserTransformation (const vector > &, const vector > &)': MnUserTransformation.cpp:30: ::sprintf' undeclared (first use here) make[2]: *** [MnUserTransformation.lo] Error 1` then probably your gcc compiler is set to version 2.96 or older. Try to set it to 3.2 or 3.2.3.
run "`make check`" to create the executable example:
`make check`
Then run the executable example using the automatically created script:
`test/MnTutorial/test_Minuit_Quad4FMain`
and the produced output should look like this one here.
