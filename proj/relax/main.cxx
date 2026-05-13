#include "ba.h"
#include "ba_gsl.h"

#include "rcontext.h"
#include "relax_points.h"
#include "relax_cameras.h"


#define NRELAX 9
#define NCYCLES 10  /* Number of complete relax until changing tol */
#define FIRST_RANSAC_TOL 10
#define TOL_STEP 1


static void relax( RelaxationContext *rc, Cameras *cam, int niters,
            double first_tol, double tol_step );
static void optimize( RelaxationContext *rc, Cameras *cam, double tol );
static void refine_inliers( HomologPoint *hp, Cameras *c , double tol );


int main( int argc, char **argv )
{
 RelaxationContext *rc;
 HomologPoint *hp;
 Cameras *cam;
 int i, nframes;
 gsl_matrix *k;
 int m[200];
 
 k = gsl_matrix_alloc(3,3);

 FILE *kf = fopen( argv[1], "r" );
 FILE *cam_f = fopen( argv[3], "r" );
 FILE *fout = fopen( argv[4], "w" );
  
 gsl_matrix_fscanf( kf, k );
 cam = cameras_read( k, cam_f );
 rc = rc_alloc( argv[2], cam, FIRST_RANSAC_TOL );
   
 relax( rc, cam, NRELAX, FIRST_RANSAC_TOL, TOL_STEP );
 
 for( i = 0; i < cam->ncams; i++ ){
   fprintf( fout, "Frame %i\n", i );
   gsl_matrix_fprintf( fout, rc->c->r[i], "%f" );
   fprintf( fout, "\n" );
   gsl_vector_fprintf( fout, rc->c->t[i], "%f" );
   fprintf( fout, "\n" );
 }
 
 fclose( fout );
 fclose( kf );
 fclose( cam_f );
 gsl_matrix_free(k);
}


void relax( RelaxationContext *rc, Cameras *cam, int niters, 
            double first_tol, double tol_step )
{
  int i, j, total_ninliers;
  double tol = first_tol;
  
  for( j = 0; j < niters; j++ ){
     total_ninliers = 0;
     
     printf( "MAX REPROJ ERROR: %lf\n", tol );
     
     #pragma omp parallel for schedule(static) 
     for( i=0; i < rc->npoints; i++ ){
       refine_inliers( rc->hp[i], cam, tol );
       printf( "%i: [%i,%i], %i ninliers = %i\n", i, rc->hp[i]->first_frame, 
                      rc->hp[i]->last_frame,  
                      rc->hp[i]->last_frame - rc->hp[i]->first_frame + 1, 
                      rc->hp[i]->ninliers );  
       total_ninliers += rc->hp[i]->ninliers;
     }
     printf( "Total NINLIERS = %i\n\n\n", total_ninliers );
     
     for( i = 0; i < NCYCLES; i++ ){
        optimize( rc, cam, tol );
     }
     
     tol -= tol_step;
  } 
}    


void optimize( RelaxationContext *rc, Cameras *cam, double tol )
{
 int i; 
 
 #pragma omp parallel for schedule(static) 
 for( i = 0; i < rc->npoints; i++ ){
   double aux[3];
   gsl_vector_view view = gsl_vector_view_array( aux, 3 );
   
   gsl_vector *xpts = &view.vector;
   relax_points( xpts, rc->hp[i]->p, rc->hp[i], cam );
   gsl_vector_memcpy( rc->hp[i]->p, xpts ); 
 }
 
 #pragma omp parallel for schedule(static) 
 for( i=0; i < rc->c->ncams; i++ ){
    double aux[6];
    gsl_vector_view view = gsl_vector_view_array( aux, 6 );
   
    gsl_vector *xcams = &view.vector; 
    relax_cameras( xcams, rc, i );
    adjust_relax_context( rc, xcams, i );
 }   
 
}


void refine_inliers( HomologPoint *hp, Cameras *c , double tol )
{
 int i;
 hp->ninliers = 0;
 
 for( i = hp->first_frame; i <= hp->last_frame; i++ )
   if( hp_reproj_error( hp, i, c ) < tol ){
      hp_set_inlier( hp, i, TRUE );
      hp->ninliers++;  
   }
   else
      hp_set_inlier( hp, i, FALSE );
}


