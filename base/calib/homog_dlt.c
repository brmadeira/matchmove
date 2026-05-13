#include "calib.h"


void calib_homog_estimate(gsl_matrix *H, 
                          gsl_matrix *p, gsl_matrix *q)
{
 int i, j, n = p->size1;
 double px, py, qx, qy;
 gsl_matrix *A, *V;
 gsl_vector *S, *w;
 
 A = gsl_matrix_alloc( 2*n, 9 );

 for(i = 0; i < n; i++){
   px = gsl_matrix_get(p, i, 0);
   py = gsl_matrix_get(p, i, 1);
   qx = gsl_matrix_get(q, i, 0);
   qy = gsl_matrix_get(q, i, 1);

   gsl_matrix_set(A, 2*i,   0,  0);
   gsl_matrix_set(A, 2*i,   1,  0);
   gsl_matrix_set(A, 2*i,   2,  0);
   gsl_matrix_set(A, 2*i,   3, -px);
   gsl_matrix_set(A, 2*i,   4, -py);
   gsl_matrix_set(A, 2*i,   5, -1);
   gsl_matrix_set(A, 2*i,   6,  qy * px);
   gsl_matrix_set(A, 2*i,   7,  qy * py);
   gsl_matrix_set(A, 2*i,   8,  qy);

   gsl_matrix_set(A, 2*i+1, 0,  px);
   gsl_matrix_set(A, 2*i+1, 1,  py);
   gsl_matrix_set(A, 2*i+1, 2,  1);
   gsl_matrix_set(A, 2*i+1, 3,  0);
   gsl_matrix_set(A, 2*i+1, 4,  0);
   gsl_matrix_set(A, 2*i+1, 5,  0);
   gsl_matrix_set(A, 2*i+1, 6, -qx * px);
   gsl_matrix_set(A, 2*i+1, 7, -qx * py);
   gsl_matrix_set(A, 2*i+1, 8, -qx);
 }

 V = gsl_matrix_alloc(9, 9);
 S = gsl_vector_alloc(9);
 w = gsl_vector_alloc(9);

 gsl_linalg_SV_decomp(A, V, S, w);

 for(i = 0; i < 3; i++){
  for(j = 0; j < 3; j++){
    gsl_matrix_set(H, i, j, 
         gsl_matrix_get(V, i*3 + j, 8));
  }
 }

 gsl_matrix_free(A);
 gsl_matrix_free(V);
 gsl_vector_free(S);
 gsl_vector_free(w);
}
