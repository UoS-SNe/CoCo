# CoCo - v0.6.0
Core Collapse Supernovae templates
___
Developed by Szymon Prajs (S.Prajs@soton.ac.uk).     
Originally designed by Natasha Karpenka.  
Currently mainted by Szymon Prajs and Rob Firth.   

This package is still in an active development phase and currently contains **some** known issues. We are working hard to fix them as quickly as possible! Full release is expected in the coming months.

___

## Installation
Future releases of the code will include a `configure` file and follow the standard GNU autoconf routine.

## Dependencies
- [GSL](https://www.gnu.org/software/gsl/)
- [MultiNest](https://ccpforge.cse.rl.ac.uk/gf/project/multinest/)
- LAPACK (Required for MultiNest)

## Usage
To create new supernova templates run `lcfit` -> `specfit` -> `specphase` in this order.

If you are already have a set of supernova templates or are using the default templates only you can use `lcsim` as a stand alone package.

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
`SN_redshift_list.list` must have two columns: SN_name redshift

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
