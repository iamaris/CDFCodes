////////////////////////////////////////////////////////////////////////
//
// File: testIntegrationData.cc
// $Id: testIntegrationData.cc,v 1.4 2002/01/22 03:06:39 rico Exp $
// Purpose: Test program for the IntegrationData class.
//
// Author:  Marc Paterno
// Created: 26 Feb 2001
// Modified:
//
////////////////////////////////////////////////////////////////////////

#include "SimulationMods/IntegrationData.hh"

#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>

typedef std::vector<sim::IntegrationData> vector;

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
	bool verbose = ( argc > 1 );
	if ( verbose ) std::cout << "Running " << argv[0] << std::endl;

	sim::IntegrationData i1;
	sim::IntegrationData i2(1.0, 2.0, 3.0, 4.0, 1);
	assert ( i1 < i2 );
	assert ( ! (i2 < i1) );

	sim::IntegrationData i3(11.0, 0.0, 20.0, 17.0, 3);
	assert ( i2 < i3 );
	assert ( ! (i3 < i2) );
	assert ( i1 < i3 );	

	sim::IntegrationData i4(1.0, 2.0, 24.0, 17.0, 7);
	assert ( i3 < i4 );
	assert ( ! (i4 < i3) );

	vector v1;
	v1.push_back(i2);
	v1.push_back(i3);
	v1.push_back(i4);
	if ( verbose ) std::cout <<  "Size of v1: " << v1.size() << std::endl;
	assert (v1.size() == 3);
	std::sort(v1.begin(), v1.end());
	assert ( v1[0] < v1[1] );
	assert ( v1[0] < v1[2] );
	assert ( v1[1] < v1[2] );

	// Make sure addition gives the expected answer
	sim::IntegrationData sum = i2 + i3;
	sim::IntegrationData expected(12.0, 2.0, 23.0, 21.0, 4);
	if ( verbose )
		{
			std::cout << "i2      : " << i2 << std::endl;
			std::cout << "i3      : " << i3 << std::endl;
			std::cout << "Sum     : " << sum << std::endl;
			std::cout << "Expected: " << expected << std::endl;
		}
	assert ( sum == expected );

	// Make sure sequence of addition does the right thing; if we made
	// it here, we already know one addition does the right thing.
	sim::IntegrationData i2copy(i2);
	sim::IntegrationData i3copy(i3);
	sim::IntegrationData i4copy(i4);
	sum = i2 + i3 + i4;
	assert ( i2 == i2copy );
	assert ( i3 == i3copy );
	assert ( i4 == i4copy );
	
	expected = i2;
	expected += i3;
	expected += i4;
	if ( verbose )
		{
			std::cout << "Sum     : " << sum << std::endl;
			std::cout << "Expected: " << expected << std::endl;
		}
	assert ( sum == expected );

	// Make sure subtraction does the opposite of addition.
	expected -= i4;
	expected -= i3;
	expected -= i2;
	assert ( expected == i1 );

	return 0;
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

