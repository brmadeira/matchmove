#include "rcontext.h"
#include "relax_points.h"


typedef struct RelaxData{
 HomologPoint *hp; 
 Cameras *c;
} RelaxData;

static int relax_cost_func( const gsl_vector *x, void *params, gsl_vector *f);
static double relax_reproj_error( const gsl_vector *x, int i, RelaxData *d, int coord );


void relax_points( gsl_vector *xout, gsl_vector *x, HomologPoint *hp, Cameras *c )
{
 const gsl_multifit_fdfsolver_type *t = gsl_multifit_fdfsolver_lmsder;
 gsl_multifit_fdfsolver *s;
 gsl_multifit_function_fdf f;
 RelaxData d;
 
 d.hp = hp;
 d.c = c;
 
 f.f = &relax_cost_func;
 f.df = NULL;
 f.p = 3;
 f.n = 2*(hp->last_frame - hp->first_frame + 1);
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
}
 
 
int relax_cost_func(const gsl_vector *x, void *params, gsl_vector *f)
{
 int i, index;
 RelaxData *data = (RelaxData*)params;
  
 gsl_vector_set_zero(f);
 for( i = data->hp->first_frame; i <= data->hp->last_frame; i++ ){
   index = i - data->hp->first_frame;
   
   if( hp_is_inlier( data->hp, i ) ){
     gsl_vector_set( f, 2*index, relax_reproj_error( x, i, data, 0 ));
     gsl_vector_set( f, 2*index + 1, relax_reproj_error( x, i, data, 1 ));
   }
   else{
     gsl_vector_set( f, 2*index, 0. );
     gsl_vector_set( f, 2*index + 1, 0. );
   }
 }
    
 return GSL_SUCCESS;
}


double relax_reproj_error(const gsl_vector *x, int i, RelaxData *data, int coord)
{
    double error, proj_data[2], ref_proj_data[2];

    gsl_vector_view proj = gsl_vector_view_array(proj_data, 2);
    gsl_vector_view ref_proj = gsl_vector_view_array(ref_proj_data, 2);

    cameras_apply(data->c, x, &proj.vector, i);

    ref_proj_data[0] = gsl_matrix_get(data->hp->projs,
                                      i - data->hp->first_frame, 0);
    ref_proj_data[1] = gsl_matrix_get(data->hp->projs,
                                      i - data->hp->first_frame, 1);

    error = gsl_vector_get(&proj.vector, coord) -
            gsl_vector_get(&ref_proj.vector, coord);

    return error;
}


void relax_optimize( gsl_multifit_fdfsolver *s )
{
 int status, iter = 0;

 do{
    iter++;
    status = gsl_multifit_fdfsolver_iterate(s);
    if(status)
       break;
    status = gsl_multifit_test_delta(s->dx, s->x, LM_EPS, LM_EPS);
 }
 while( (status == GSL_CONTINUE) && (iter < LM_MAX_ITERATIONS ) );
}



