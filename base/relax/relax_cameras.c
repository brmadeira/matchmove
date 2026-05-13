#include "relax_cameras.h"


typedef struct RelaxCamData{
 int cam_id;
 RelaxationContext *rc; 
} RelaxCamData;


static void encoder_camera( gsl_vector *xout, gsl_matrix *r, gsl_vector *t );
static void decoder_camera( gsl_matrix *r, gsl_vector *t, gsl_vector *x );
static int relax_cam_cost_func( const gsl_vector *x, void *params, gsl_vector *f);
double relax_cam_reproj_error( gsl_vector *x, gsl_matrix *k, gsl_matrix *r, gsl_vector *t,
                               int cam_id, int point_id, RelaxationContext *rc, int coord );


void relax_cameras( gsl_vector *xout, RelaxationContext *rc, int cam_id )
{
 int i, nprojs = 0;
 const gsl_multifit_fdfsolver_type *t = gsl_multifit_fdfsolver_lmsder;
 gsl_multifit_fdfsolver *s;
 gsl_multifit_function_fdf f;
 RelaxCamData d;
 gsl_vector *x;
 
 x = gsl_vector_alloc(6);
 encoder_camera( x, rc->c->r[cam_id], rc->c->t[cam_id] );
 
 d.cam_id = cam_id;
 d.rc = rc;
          
 for( i = 0; i < rc->npoints; i++ )
   if( hp_is_inlier( rc->hp[i], cam_id ) )
     nprojs++;
 
 f.f = &relax_cam_cost_func;
 f.df = NULL;
  
 f.p = 6;
 f.n = 2*nprojs;
 if( f.n < f.p ){
   gsl_vector_memcpy( xout, x );
   return;
 }
 f.params = &d;
  
 s = gsl_multifit_fdfsolver_alloc(t, f.n, f.p);
 gsl_multifit_fdfsolver_set(s, &f, x);
 relax_optimize(s);
 gsl_vector_memcpy( xout, s->x );
 gsl_multifit_fdfsolver_free(s);
 gsl_vector_free(x);
}
 
 
int relax_cam_cost_func(const gsl_vector *x, void *params, gsl_vector *f)
{
 int i, j = 0;
 RelaxCamData *data = (RelaxCamData*)params;
 gsl_matrix *r;
 gsl_vector *t;
 
 r = gsl_matrix_alloc( 3, 3 );
 t = gsl_vector_alloc( 3 );
  
 decoder_camera( r, t, (gsl_vector*)x );
  
 for( i = 0; i < data->rc->npoints; i++ ){
   if( hp_is_inlier( data->rc->hp[i], data->cam_id ) ){    
      gsl_vector_set( f, j, relax_cam_reproj_error( data->rc->hp[i]->p, 
               data->rc->c->k , r, t, data->cam_id, i, data->rc, 0 ));   
      gsl_vector_set( f, j+1, relax_cam_reproj_error( data->rc->hp[i]->p, 
               data->rc->c->k , r, t, data->cam_id, i, data->rc, 1 ));         
      j+=2;
   }
 }
      
 gsl_matrix_free( r );    
 gsl_vector_free( t );          
 return GSL_SUCCESS;
}


double relax_cam_reproj_error(gsl_vector *x, gsl_matrix *k, gsl_matrix *r, gsl_vector *t,
                              int cam_id, int point_id, RelaxationContext *rc, int coord)
{
    double error, proj_data[2], ref_proj_data[2], p_data[12]; 

    gsl_vector_view proj = gsl_vector_view_array(proj_data, 2);
    gsl_vector_view ref_proj = gsl_vector_view_array(ref_proj_data, 2);
    gsl_matrix_view p = gsl_matrix_view_array(p_data, 3, 4);

    HomologPoint *hp = rc->hp[point_id];

    calib_pmatrix_make(&p.matrix, k, r, t);
    calib_apply_P(&p.matrix, x, &proj.vector);

    ref_proj_data[0] = gsl_matrix_get(hp->projs,
                                     cam_id - hp->first_frame, 0);
    ref_proj_data[1] = gsl_matrix_get(hp->projs,
                                     cam_id - hp->first_frame, 1);

    error = gsl_vector_get(&proj.vector, coord) -
            gsl_vector_get(&ref_proj.vector, coord);

    return error;
}



void decoder_camera( gsl_matrix *r, gsl_vector *t, gsl_vector *x )
{
 gsl_vector *aux;
 
 aux = gsl_vector_alloc(3);
 gsl_vector_set( aux, 0, gsl_vector_get( x, 0 ) );
 gsl_vector_set( aux, 1, gsl_vector_get( x, 1 ) );
 gsl_vector_set( aux, 2, gsl_vector_get( x, 2 ) ); 
 ba_axis_angle_to_r( r, aux );
 gsl_vector_set( t, 0, gsl_vector_get( x, 3 ) );
 gsl_vector_set( t, 1, gsl_vector_get( x, 4 ) );
 gsl_vector_set( t, 2, gsl_vector_get( x, 5 ) ); 
 
 gsl_vector_free( aux );
}


void encoder_camera( gsl_vector *xout, gsl_matrix *r, gsl_vector *t )
{
 gsl_vector *axis_angle;
 
 axis_angle = gsl_vector_alloc(3);
 ba_r_to_axis_angle( axis_angle, r );
 gsl_vector_set( xout, 0, gsl_vector_get( axis_angle, 0 ) );
 gsl_vector_set( xout, 1, gsl_vector_get( axis_angle, 1 ) );
 gsl_vector_set( xout, 2, gsl_vector_get( axis_angle, 2 ) );
 gsl_vector_set( xout, 3, gsl_vector_get( t, 0 ) );
 gsl_vector_set( xout, 4, gsl_vector_get( t, 1 ) );
 gsl_vector_set( xout, 5, gsl_vector_get( t, 2 ) ); 
 
 gsl_vector_free( axis_angle );
}


void adjust_relax_context( RelaxationContext *rc, gsl_vector *xout, int cam_id )
{
 decoder_camera( rc->c->r[cam_id], rc->c->t[cam_id], xout );
}


