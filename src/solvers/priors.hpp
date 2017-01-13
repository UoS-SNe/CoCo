#ifndef COCO_SOLVERS_PRIORS_HPP_
#define COCO_SOLVERS_PRIORS_HPP_


double skillingScalePrior(double);
double logPrior(double,double,double);
double modJeffPrior(double,double,double);
double flatPrior(double,double,double);
double powerPrior(double,double,double,double);
double gaussianPrior(double,double,double);
double sinPrior(double,double,double);
double cauchyPrior(double,double,double);
double logNormalPrior(double,double,double);
double dierfc(double);


#endif  // COCO_SOLVERS_PRIORS_HPP_
