#ifndef BA_CERES_H
#define BA_CERES_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>


/*----------------------
 Main entry (GSL interface)
----------------------*/
void ba_ext_exec(gsl_vector *xout,
                 gsl_vector *params,
                 gsl_matrix *projs,
                 gsl_matrix *k);
               
 
gsl_vector* ba_ext_param_alloc(int n_cameras, int n_points);
void ba_ext_get_rt(gsl_vector *r, gsl_vector *t, const gsl_vector *x, int n_points, int cam_index);
void ba_ext_set_camera(gsl_vector *x, gsl_vector *r, gsl_vector *t, int n_points, int cam_index);
void ba_ext_get_camera(gsl_matrix *camera, const gsl_vector *x, int n_points, int cam_index, gsl_matrix *k);
                 


#endif // BA_CERES_H
