#ifndef RELAX_POINTS_H
#define RELAX_POINTS_H

#include "homol_point.h"
#include "cameras.h"
#include <gsl/gsl_multifit_nlin.h>


void relax_points( gsl_vector *xout, gsl_vector *x, HomologPoint *hp, Cameras *c );
void relax_optimize( gsl_multifit_fdfsolver *s );

#define LM_EPS 1e-6
#define LM_MAX_ITERATIONS 20
             
#endif
