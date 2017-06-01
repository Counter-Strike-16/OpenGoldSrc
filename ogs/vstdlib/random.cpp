//======== (C) Copyright 1999, 2000 Valve, L.L.C. All rights reserved. ========
//
// Purpose: Random number generator
//
// $Workfile: $
// $NoKeywords: $
//=============================================================================

/// @file

#include "vstdlib/random.h"
#include <math.h>

#define IA 16807
#define IM 2147483647
#define IQ 127773
#define IR 2836
#define NDIV (1+(IM-1)/NTAB)
#define MAX_RANDOM_RANGE 0x7FFFFFFFUL

// fran1 -- return a random floating-point number on the interval [0,1)
//
#define AM (1.0/IM)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

//-----------------------------------------------------------------------------
// globals
//-----------------------------------------------------------------------------
static CUniformRandomStream s_UniformStream;
static CGaussianRandomStream s_GaussianStream;
static IUniformRandomStream *s_pUniformStream = &s_UniformStream;

//-----------------------------------------------------------------------------
// Installs a global random number generator, which will affect the Random functions above
//-----------------------------------------------------------------------------
void InstallUniformRandomStream( IUniformRandomStream *pStream )
{
	s_pUniformStream = pStream ? pStream : &s_UniformStream;
};

//-----------------------------------------------------------------------------
// A couple of convenience functions to access the library's global uniform stream
//-----------------------------------------------------------------------------
void RandomSeed(int iSeed)
{
	s_pUniformStream->SetSeed( iSeed );
};

float RandomFloat(float flMinVal, float flMaxVal)
{
	return s_pUniformStream->RandomFloat( flMinVal, flMaxVal );
};

float RandomFloatExp(float afMinVal, float afMaxVal, float afExponent)
{
	return 0.0f;
};

int RandomInt(int iMinVal, int iMaxVal)
{
	return s_pUniformStream->RandomInt( iMinVal, iMaxVal );
};

float RandomGaussianFloat(float flMean, float flStdDev)
{
	return s_GaussianStream.RandomFloat( flMean, flStdDev );
};

//-----------------------------------------------------------------------------
//
// Implementation of the uniform random number stream
//
//-----------------------------------------------------------------------------
CUniformRandomStream::CUniformRandomStream()
{
	SetSeed(0);
}

void CUniformRandomStream::SetSeed( int iSeed )
{
	m_idum = ( ( iSeed < 0 ) ? iSeed : -iSeed );
	m_iy = 0;
}

int CUniformRandomStream::GenerateRandomNumber()
{
	int j;
	int k;
	
	if (m_idum <= 0 || !m_iy)
	{
		if (-(m_idum) < 1) 
			m_idum=1;
		else 
			m_idum = -(m_idum);

		for ( j=NTAB+7; j>=0; j--)
		{
			k = (m_idum)/IQ;
			m_idum = IA*(m_idum-k*IQ)-IR*k;
			if (m_idum < 0) 
				m_idum += IM;
			if (j < NTAB)
				m_iv[j] = m_idum;
		}
		m_iy=m_iv[0];
	}
	k=(m_idum)/IQ;
	m_idum=IA*(m_idum-k*IQ)-IR*k;
	if (m_idum < 0) 
		m_idum += IM;
	j=m_iy/NDIV;
	m_iy=m_iv[j];
	m_iv[j] = m_idum;

	return m_iy;
}

float CUniformRandomStream::RandomFloat( float flLow, float flHigh )
{
	// float in [0,1)
	float fl = AM * GenerateRandomNumber();
	if (fl > RNMX) 
		fl = RNMX;

	return (fl * (flHigh-flLow)) + flLow; // float in [low,high)
}

int CUniformRandomStream::RandomInt( int iLow, int iHigh )
{
	//ASSERT(lLow <= lHigh);
	unsigned int maxAcceptable;
	unsigned int x = iHigh-iLow+1;
	unsigned int n;
	if (x <= 1 || MAX_RANDOM_RANGE < x-1)
	{
		return iLow;
	}

	// The following maps a uniform distribution on the interval [0,MAX_RANDOM_RANGE]
	// to a smaller, client-specified range of [0,x-1] in a way that doesn't bias
	// the uniform distribution unfavorably. Even for a worst case x, the loop is
	// guaranteed to be taken no more than half the time, so for that worst case x,
	// the average number of times through the loop is 2. For cases where x is
	// much smaller than MAX_RANDOM_RANGE, the average number of times through the
	// loop is very close to 1.
	//
	maxAcceptable = MAX_RANDOM_RANGE - ((MAX_RANDOM_RANGE+1) % x );
	do
	{
		n = GenerateRandomNumber();
	} while (n > maxAcceptable);

	return iLow + (n % x);
}


//-----------------------------------------------------------------------------
//
// Implementation of the gaussian random number stream
// We're gonna use the Box-Muller method (which actually generates 2
// gaussian-distributed numbers at once)
//
//-----------------------------------------------------------------------------
CGaussianRandomStream::CGaussianRandomStream( IUniformRandomStream *pUniformStream )
{
	AttachToStream( pUniformStream );
}


//-----------------------------------------------------------------------------
// Attaches to a random uniform stream
//-----------------------------------------------------------------------------
void CGaussianRandomStream::AttachToStream( IUniformRandomStream *pUniformStream )
{
	m_pUniformStream = pUniformStream;
	m_bHaveValue = false;
}


//-----------------------------------------------------------------------------
// Generates random numbers
//-----------------------------------------------------------------------------
float CGaussianRandomStream::RandomFloat( float flMean, float flStdDev )
{
	IUniformRandomStream *pUniformStream = m_pUniformStream ? m_pUniformStream : s_pUniformStream;
	float fac,rsq,v1,v2;

	if (!m_bHaveValue)
	{
		// Pick 2 random #s from -1 to 1
		// Make sure they lie inside the unit circle. If they don't, try again
		do
		{
			v1 = 2.0f * pUniformStream->RandomFloat() - 1.0f;
			v2 = 2.0f * pUniformStream->RandomFloat() - 1.0f;
			rsq = v1*v1 + v2*v2;
		} while ((rsq > 1.0f) || (rsq == 0.0f));

		// The box-muller transformation to get the two gaussian numbers
		fac = sqrtf( -2.0f * log(rsq) / rsq );

		// Store off one value for later use
		m_flRandomValue = v1 * fac;
		m_bHaveValue = true;

		return flStdDev * (v2 * fac) + flMean;
	}
	else
	{
		m_bHaveValue = false;
		return flStdDev * m_flRandomValue + flMean;
	}
}


//-----------------------------------------------------------------------------
// Creates a histogram (for testing)
//-----------------------------------------------------------------------------
