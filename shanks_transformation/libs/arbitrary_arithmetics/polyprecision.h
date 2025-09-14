#ifndef INC_POLYNOMIAL
#define INC_POLYNOMIAL

/*
 *******************************************************************************
 *
 *
 *                       Copyright (c) 2007-2020
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
 * Module name     :   polyprecision.h
 * Module ID Nbr   :
 * Description     :   Polynomial Precision arithmetic template class
 *                     Works with regular base types, int_precision, float_precision,
 *						complex_precision & interval precision classes
 * --------------------------------------------------------------------------
 * Change Record   :
 *
 * Version	Author/Date		Description of changes
 * -------  -----------		----------------------
 * 01.01	HVE/Aug052007	Initial release
 * 01.02	HVE/Aug202016	Corrected a lack of casting issue in template<class _TY> std::string polynomial<_TY>::toString()
 *							Added (std::ostream&)strs << statements
 * 01.03    HVE/Jul-07-2019 Make the code more portable to a GCC environment
 * 01.04	HVE/12-Aug-2020	Change precision type from unsinged int to size_t to enable both 32 and 64b it target.
 *
 * End of Change Record
 * --------------------------------------------------------------------------
*/


/* define version string */
static char _VPolyP_[] = "@(#)polyprecision.h 01.04-- Copyright (C) Henrik Vestermark";

#include <typeinfo>
#include <vector>

#ifndef MAX
#define MAX(x,y)  ((x)>(y)?(x):(y))
#endif
#ifdef MIN
#define MIN(x,y)  ((x)>(y)?(y):(x))
#endif

//
// Polynomial class
// Coefficients are store with v[n]=a(n)x^n, v[n-1]=a(n-1)x^n-1,...,v[1]=a(1)x,v[1]=a(0) etc
//
template<class _TY> class polynomial {
   std::vector<_TY> v;
   public:
      typedef _TY value_type;

    // constructor for a zero polynomial at n degree
    polynomial() : v(0) {}				// degree 0
	polynomial(size_t n) : v(n+1) {}	// degree n
	 // constructor for any other type to _TY
    template<class _XTY> polynomial( const polynomial<_XTY>& a ) : v((_TY)(a)) {}

    // Coordinate functions
    bool empty()	const		{ return v.empty(); }
    size_t degree()	const		{ return v.size()-1; }
	size_t degree(size_t n)		{ v.resize(n+1); return v.size()-1; }
	size_t normalize()			{ while(v.size()>1&&v[v.size()-1]==_TY(0)) v.resize(v.size()-1); return v.size()-1; }
	size_t slice(size_t n)		{ for(;n>0&&v.size()>0;n--) v.erase(v.begin()); return v.size()-1; }
	_TY at(const int inx)		{ return v.at(inx); }
	_TY index(const size_t i)	{ return v[i]; }
	_TY index(const size_t i, const _TY e)	{ if(i>=v.size()) v.resize(i+1); return v[i]=e; }
	std::string toString();	// Convert Polynomial to a String

	 // Essential operators
	 _TY& operator[](size_t n) 				{ return v[n]; }
	 const _TY& operator[](size_t n) const 	{ return v[n]; }

	polynomial<_TY>& operator=( const polynomial<_TY>& );
    polynomial<_TY>& operator+=( const polynomial<_TY>& );
    polynomial<_TY>& operator-=( const polynomial<_TY>& );
    polynomial<_TY>& operator*=( const polynomial<_TY>& );
    polynomial<_TY>& operator/=( const polynomial<_TY>& );
	polynomial<_TY>& operator%=( const polynomial<_TY>& );

	template<class _TYF, class _TYX> _TYF value( _TYX x );  // Polynomial evaluated at point x
	template<class _TYF, class _TYX> complex_precision<_TYF> value( complex_precision<_TYX> x );  // Polynomial evaluated at complex point x
	template<class _TYR> polynomial<_TY> deflate( _TYR z );  // Polynomial deflated by root z
	polynomial<_TY> derivative();		// Derivative of Polynomial
	};


// Basic arithmetic operators
polynomial<class _TY> operator+( const polynomial<class _TY>&, const polynomial<class _TY>& );
polynomial<class _TY> operator+( const polynomial<class _TY>& );  // Unary
polynomial<class _TY> operator-( const polynomial<class _TY>&, const polynomial<class _TY>& );
polynomial<class _TY> operator-( const polynomial<class _TY>& );                 // Unary
polynomial<class _TY> operator*( const polynomial<class _TY>&, const polynomial<class _TY>& );
polynomial<class _TY> operator/( const polynomial<class _TY>&, const polynomial<class _TY>& );
polynomial<class _TY> operator%( const polynomial<class _TY>&, const polynomial<class _TY>& );

// Output Operator <<
//
/*template<class _TY> std::ostream& operator<<( std::ostream& strm, polynomial<_TY>& a )
	{
	strm << a.toString();
	return strm;
	}
*/
template<class _TY> std::ostream& operator<<( std::ostream& strm, polynomial<_TY> a )
	{
	strm << a.toString();
	return strm;
	}

// Works for all class types
//
template<class _TY> polynomial<_TY>& polynomial<_TY>::operator=( const polynomial<_TY>& rhs )
   {
   v=rhs.v;
   return *this;
   }


// Works all other classes.
//
template<class _TY> polynomial<_TY>& polynomial<_TY>::operator+=( const polynomial<_TY>& rhs )
   {
   size_t na=rhs.degree();
   this->degree(MAX(this->degree(),na));
   for(size_t i=0;i<=na;i++)
	   v[i]+=rhs[i];
   return *this;
   }

// Works all other classes.
//
template<class _TY> polynomial<_TY>& polynomial<_TY>::operator-=( const polynomial<_TY>& rhs )
   {
   size_t na=rhs.degree();
   this->degree(MAX(this->degree(),na));
   for(size_t i=0;i<=na;i++)
	   v[i]-=rhs[i];
   return *this;
   }

// Works all other classes.
//
template<class _TY> polynomial<_TY>& polynomial<_TY>::operator*=( const polynomial<_TY>& rhs )
   {
   size_t na=this->degree();
   size_t nb=rhs.degree();
   polynomial<_TY> c(na+nb);
   for(size_t i=0;i<=na;i++)
	  for(size_t j=0;j<=nb;j++)
		{ c[i+j]+=v[i]*rhs[j];
		}
   this->v=c.v;
   return *this;
   }


// Works all other classes.
//
template<class _TY> polynomial<_TY>& polynomial<_TY>::operator/=( const polynomial<_TY>& rhs )
   {
   size_t na=this->degree();
   size_t nb=rhs.degree();
   size_t nq=na-nb;

   if(nq<=0) { v.resize(0); return *this; }

   polynomial<_TY> q(nq);
   polynomial<_TY> r(*this);
   for(size_t k=nq;k>=0;k--)
		{
		q[k]=r[nb+k]/rhs[nb];
		for (size_t j = nb; j >= 0; j--) 
			{
			r[j + k] -= q[k] * rhs[j]; if (j == 0) break;
			}
		if (k == 0) break;
		}
   q.normalize();
   v=q.v;
   return *this;
   }

// Works all other classes.
//
template<class _TY> polynomial<_TY>& polynomial<_TY>::operator%=( const polynomial<_TY>& rhs )
   {
   size_t na=this->degree();
   size_t nb=rhs.degree();
   size_t nq=na-nb;

   if(nq<=0) {v.resize(0); return *this;}

   polynomial<_TY> q(nq), r(*this);
   for(size_t k=nq;k>=0;k--)
		{
		q[k]=r[nb+k]/rhs[nb];
		for (size_t j = nb; j >= 0; j--) 
			{
			r[j + k] -= q[k] * rhs[j]; if (j == 0) break;
			}
		if (k == 0) break;
		}
   r.degree(nb);
   r.normalize();
   v=r.v;
   return *this;
   }

//template<class _Ty> complex_precision<_Ty> operator+( const complex_precision<_Ty>& x, const complex_precision<_Ty>& y )
// Binary + operator
// Works for all classes
//
template<class _TY> polynomial<_TY> operator+( const polynomial<_TY>& a, const polynomial<_TY>& b )
   {
   polynomial<_TY> lhs(a);

   lhs += b;
   return lhs;
   }


// Unary + operator
// Works for all classes
//
template<class _TY> polynomial<_TY> operator+( const polynomial<_TY>& a )
   {
   return a;
   }

// Binary - operator
// Works for all classes
//
template<class _TY> polynomial<_TY> operator-( const polynomial<_TY>& a, const polynomial<_TY>& b )
   {
   polynomial<_TY> lhs(a);

   lhs -= b;
   return lhs;
   }


// Unary - operator
// Works for all classes
//
template<class _TY> polynomial<_TY> operator-( const polynomial<_TY>& a )
   {
   polynomial<_TY> lhs(a);
   int nc=lhs.degree();
   for(int i=0;i<nc;i++)
	lhs[i] = -lhs[i];
   return lhs;
   }

// Binary * operator
// Works for all classes
//
template<class _TY> polynomial<_TY> operator*( const polynomial<_TY>& a, const polynomial<_TY>& b )
   {
   polynomial<_TY> lhs(a);

   lhs *= b;
   return lhs;
   }

// Binary / operator
// Works for all classes
//
template<class _TY> polynomial<_TY> operator/( const polynomial<_TY>& a, const polynomial<_TY>& b )
   {
   polynomial<_TY> lhs(a);

   lhs /= b;
   return lhs;
   }

// Binary % operator
// Works for all classes
//
template<class _TY> polynomial<_TY> operator%( const polynomial<_TY>& a, const polynomial<_TY>& b )
   {
   polynomial<_TY> lhs(a);

   lhs %= b;
   return lhs;
   }


template<class _TY> polynomial<_TY> pow( const polynomial<_TY> x, const int y )
   {
   polynomial<_TY> p(x);
   polynomial<_TY> r(0);

   r[0]= (_TY)1;
   for(int n=y; n>0; n>>=1)
		{
		if((n&0x1)!=0) r*=p;  // Odd
		p*=p;
		}
   return r;
   }


// toString method
// Work on all classes
//
template<class _TY> std::string polynomial<_TY>::toString()
	{
	std::string s;
	std::ostringstream strs, tmp_strs;

	for( size_t i=this->degree(); i>=0; i-- )
		{
		tmp_strs.str("");  // Clear it
		(std::ostream &)tmp_strs << this->index(i);
		if(i<this->degree() && ( tmp_strs.str()[0] != '-' && tmp_strs.str()[0] != '+' ) )
			{
			(std::ostream&)strs << std::string("+");
			}
		(std::ostream&)strs << tmp_strs.str();
		if(i>0)
			{
			(std::ostream &)strs << std::string("x");
			if (i > 1) { (std::ostream &)strs << std::string("^") << i; }
			}
		else break;  // break when i==0
		}
	s=strs.str();
	return s;
	}


// Works all other classes.
//
template<class _TY> polynomial<_TY> polynomial<_TY>::derivative()
   {
   size_t na=this->degree();
   polynomial<_TY> d;

   if(na>1)
		{
		d.degree(na-1);
		for(size_t k=na;k>0;k--)
			d[k-1]=(_TY)k*this->index(k);
		}

   return d;
   }

// Specialization for complex x on real polynomials.
//
template<class _TYP>
template<class _TYF, class _TYX> complex_precision<_TYF> polynomial<_TYP>::value( complex_precision<_TYX> x )
   {
   size_t n=this->degree();
   _TYF fz;

	std::string sp;
   sp = typeid(_TYP).name(); sp=sp.substr(0,11);
   if(sp.compare("class compl")!=0) // evaluate a real polynomial at a complex point using real arithmetic.
		{
		_TYP p, q, s, r, t;
		p=-(_TYP)2 * (_TYP)x.real(); q=(_TYP)x.norm(); s=(_TYP)0; r=this->index(n);
		for(size_t i = n-1; i > 0; --i )
			{ t=this->index(i)-p*r-q*s; s=r; r=t;	}
		fz.real( this->index(0)+ r * (_TYP)x.real() - ( q * s ) );
		fz.imag( r * (_TYP)x.imag() );

		return fz;
		}
	// Evaluate a complex polynomial at a complex point.
	fz=(_TYF)this->index(n);
	for (size_t i = n - 1; i >= 0; --i)
		{
		fz = fz * (_TYF)x + (_TYF)this->index(i); if (i == 0) break;
		}
	return fz;
   }

// Works all other classes.
//
template<class _TYP>
template<class _TYF, class _TYX> _TYF polynomial<_TYP>::value( _TYX x )
   {
   size_t n=this->degree();
   _TYF fz;

	fz=(_TYF)this->index(n);
	for (size_t i = n - 1; i >= 0; --i)
		{
		fz = fz * (_TYF)x + (_TYF)this->index(i); if (i == 0) break;
		}
	return fz;
   }

// Works all other classes.
//
template<class _TY>
template<class _TYR> polynomial<_TY> polynomial<_TY>::deflate( _TYR x )
   {
   polynomial<_TY> d(this->degree()-1);
   _TY r(0);

   if( x !=(_TYR)0 )
		for( size_t k=this->degree();k>0;k--)
			d[k-1]=r=r*x+this->index(k);

   return d;
   }

#endif
