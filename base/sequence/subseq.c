#include "sequence.h"

void subseq_extrinsic_calibrate( SubSeqNode *s, gsl_matrix *k )
{
 int i;
 
 subseq_dlt_calib( s, k );
 for( i = 5; i > 2; i-- ){
   subseq_extrinsic_ba( s, k );
   subseq_recalibrate( s, i );
 } 
}

