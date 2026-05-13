#include "calib.h"

#define CREATE_ROW_N( R, P, XMAT, I, J )        \
 gsl_matrix_get_row( aux1, P, 2 );             \
 gsl_vector_scale( aux1, gsl_matrix_get( XMAT, I, J ) ); \
 gsl_matrix_get_row( aux2, P, J );             \
 gsl_vector_sub( aux1, aux2 );                 \
 gsl_matrix_set_row( a, R, aux1 );

static void copy_row_cam(gsl_matrix *dst, int dst_row,
                         gsl_matrix *src, int src_row);
                         

void calib_get_3dpoint_n( gsl_matrix *p, gsl_matrix *x2d, int n, gsl_vector *x )
{
 double i, j, h;
 gsl_vector *aux1, *aux2, *s, *w, *r;
 gsl_matrix *a, *v, *pi;

 a = gsl_matrix_alloc( 2*n, 4 );
 v = gsl_matrix_alloc( 4, 4 );
 pi = gsl_matrix_alloc( 3, 4 );

 aux1 = gsl_vector_alloc( 4 );
 aux2 = gsl_vector_alloc( 4 );
 s = gsl_vector_alloc( 4 );
 w = gsl_vector_alloc( 4 );
 r = gsl_vector_alloc( 4 );

 for ( i = 0; i < n; i++ ){
   for (j = 0; j < 3; j++) 
     copy_row_cam( pi, j, p, 3*i + j );
   
   CREATE_ROW_N( 2*i,   pi, x2d, i, 0 );
   CREATE_ROW_N( 2*i+1, pi, x2d, i, 1 );
 }

 gsl_linalg_SV_decomp( a, v, s, w );
 gsl_matrix_get_col( r, v, 3 );

 h = gsl_vector_get( r, 3 );
 gsl_vector_set(x, 0, gsl_vector_get(r, 0)/h);
 gsl_vector_set(x, 1, gsl_vector_get(r, 1)/h);
 gsl_vector_set(x, 2, gsl_vector_get(r, 2)/h);

 gsl_matrix_free( a );
 gsl_matrix_free( v );
 gsl_matrix_free( pi );
 gsl_vector_free( aux1 );
 gsl_vector_free( aux2 );
 gsl_vector_free( s );
 gsl_vector_free( w );
 gsl_vector_free( r );
}


void copy_row_cam(gsl_matrix *dst, int dst_row,
                  gsl_matrix *src, int src_row)
{
 int k;
    
 for(k = 0; k < 4; k++){
   gsl_matrix_set(dst, dst_row, k,
       gsl_matrix_get(src, src_row, k));
 }
}
