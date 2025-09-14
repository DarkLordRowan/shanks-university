#ifndef INC_MATHPRECISION
#define INC_MATHPRECISION

/*
 *******************************************************************************
 *
 *
 *                       Copyright (c) 2002-2019
 *                       Henrik Vestermark
 *                       Denmark
 *
 *                       All Rights Reserved
 *
 *   This source file is subject to the terms and conditions of the
 *   Future Team Software License Agreement which restricts the manner
 *   in which it may be used.
 *   Mail: hve@hvks.com
 *
 *******************************************************************************
*/

/*
 *******************************************************************************
 *
 *
 * Module name     :   mathprecision.h
 * Module ID Nbr   :   
 * Description     :   Arbitrary floating poiint precision class
 * --------------------------------------------------------------------------
 * Change Record   :   
 *
 * Version	Author/Date		Description of changes
 * -------  -----------		----------------------
 * 01.01	HVE/030721		Initial release
 *
 * End of Change Record
 * --------------------------------------------------------------------------
*/


/* define version string */
static char _VM_[] = "@(#)mathprecision.h 01.01 -- Copyright (C) Henrik Vestermark";


#include "fprecision.h"


static float_precision sinh2( float_precision );
static float_precision cosh2( float_precision );
static float_precision tanh2( float_precision );

// sinh = ( exp(x) - exp(-x) ) / 2;
//
static float_precision sinh2( float_precision x )
   {
   float_precision v;
   float_precision c05(0.5);

   v.precision( x.precision() + 2 );
   v = x;
   v = exp( v );
   v -= v.inverse();
   v *= c05;

   // Round to same precision as argument and rounding mode
   v.mode( x.mode() );
   v.precision( x.precision() );  

   return v;
   }

// cosh = ( exp(x) + exp(-x) ) / 2;
//
static float_precision cosh2( float_precision x )
   {
   float_precision v;
   float_precision c05(0.5);

   v.precision( x.precision() + 1 );
   v = x;
   v = exp( v );
   v += v.inverse();
   v *= c05;

   // Round to same precision as argument and rounding mode
   v.mode( x.mode() );
   v.precision( x.precision() );  

   return v;
   }


// tanh = ( exp(x) - exp(-x) ) / ( exp( x) + exp(-x) );
// or 1 - 2 * exp(-x) / ((exp(x)+exp(-x))
//
static float_precision tanh2( float_precision x )
   {
   float_precision v, vinv;

   v.precision( x.precision() + 1 );
   vinv.precision( x.precision() + 1 );
   v = x;
   v = exp( v );
   vinv =v.inverse();
   v += vinv;
   v = float_precision( 1 ) - ( float_precision( 2 ) * vinv ) /  v;

   // Round to same precision as argument and rounding mode
   v.mode( x.mode() );
   v.precision( x.precision() );  

   return v;
   }


// n!
//
/*
static int_precision factorial( int_precision &x )
   {
   int n_fac, n, limit;
   int_precision np, np_fac, plimit;

   if( x < int_precision( 13 ) )
      {
      for( n = (int)x, n_fac = 1; n > 1; --n )
         n_fac *= n;
      return int_precision( n_fac );
      }
   else
      {
      n = 12;
      np_fac = 479001600;  // 12!
      np = int_precision( n );

      // From 12..MIN(x,65536)
      if( x > int_precision( 0xffff ) )
         plimit = int_precision( 0xffff );
      else
         plimit = x;

      limit = (int)plimit;
      for( ; np < plimit; )
         {
         for( n = (int)np, ++n, n_fac = 1; n_fac < 0x7fffffff / n && n <= limit; ++n )
            n_fac *= n;
         
         np_fac *= int_precision( n_fac );
         np = int_precision( n );
         }

      // From 65536..x
      for( ; np <= x; ++np )
         np_fac *= np;
      }

   return np_fac;
   }
   */
#endif