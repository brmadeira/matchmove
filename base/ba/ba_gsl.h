#ifndef BA_GSL_H
#define BA_GSL_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_blas.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "calib.h"
#ifdef __cplusplus
}
#endif

#define LM_EPS 1e-6
#define LM_MAX_ITERATIONS 10

void ba_get_proj( gsl_vector *prj, gsl_matrix *projs, int cam_index, int point_index );
void ba_set_proj( gsl_matrix *projs, gsl_vector *prj, int cam_index, int point_index );
void ba_get_point( gsl_vector *v, const gsl_vector *x, int point_index );
void ba_set_point( gsl_vector *x, gsl_vector *v, int point_index );
void ba_set_lm_max_iterations( int n );
void ba_optimize( gsl_multifit_fdfsolver *s );

void ba_axis_angle_to_r( gsl_matrix *r, gsl_vector *axis_angle );
void ba_r_to_axis_angle( gsl_vector *axis_angle, gsl_matrix *r );

#endif




