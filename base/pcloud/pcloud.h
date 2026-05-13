#ifndef POINT_CLOUD_H
#define POINT_CLOUD_H

#include "ba.h"
#include "defs.h"
extern "C"{
#include "calib.h"
}
#include "cameras.h"
#include <gsl/gsl_multifit_nlin.h>

#define LM_EPS 1e-1
#define LM_MAX_ITERATIONS 20

typedef struct PointCloud{
  int frame;
  gsl_matrix *m;
} PointCloud;


/* Basic functions */
void pcloud_init( PointCloud *pc, int frame, int npoints );
void pcloud_free( PointCloud *pc );
void pcloud_get_point( gsl_vector *v, PointCloud *pc, int i );
void pcloud_set_point( PointCloud *pc, int i,  gsl_vector *v );
int pcloud_get_npoints( PointCloud *pc );
void pcloud_read( PointCloud *pc, FILE *f );

/* 3D Reconstruct Functions */
gsl_matrix* pcloud_calib( PointCloud *pc, Cameras *cam, int nframes );

/* Reconstruction using Non Linear Optimization */
void pcloud_nlin_optimize( gsl_vector *xout, gsl_vector *x, Cameras *cams,
                        PointCloud *pc, int nframes, int point );                      

#endif
