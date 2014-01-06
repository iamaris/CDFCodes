////////////////////////////////////////////////////////////////////////
//
// File: testPathIntegral.cc
// $Id: testPathIntegral.cc,v 1.4 2001/04/23 16:03:57 paterno Exp $
// Purpose: Test program for the PathIntegral class.
//
// Author:  Marc Paterno
// Created: 9 March 2001
// Modified:
//
////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
using namespace std;


#include "SimulationMods/PathIntegral.hh"
#include "GeometryBase/CdfTangible.hh"
#include "GeometryBase/CdfPhysicalVolume.hh"
#include "GeometryBase/CdfLogicalVolume.hh"
#include "GeometryBase/CdfMaterial.hh"
#include "GeometryBase/CdfShape.hh"
#include "GeometryBase/CdfBox.hh"
#include "GeometryBase/CdfSteppingProperties.hh"

typedef sim::PathIntegral::location_t location_t;
typedef sim::PathIntegral::point_t    point_t;

int main(int argc, char* argv[])
{	

	bool verbose = (argc > 1);
	if ( verbose ) cout << "Running " << argv[0] << endl;

	//
	// These arrays are used to create location_t instances
	//
	int numbers[] = {10, 20};
	char letters[] = { 'a', 'a', 'a', 'a',
                     'b', 'b', 'b', 'b'};

	CdfSteppingProperties* prop = new CdfSteppingProperties();
	Bfield*  bfield = 0;
	CdfMaterial* stuff1 = new CdfMaterial("stuff1", 0.1, 0.2, 0.3, 1);
	CdfMaterial* stuff2 = new CdfMaterial("stuff2", 1.1, 2.2, 3.3, 1);
	CdfMaterial* stuff4 = new CdfMaterial("stuff3", 2.2, 3.3, 4.4, 1);
	CdfShape*    box1 = new CdfBox("box1", 10.0, 10.0, 10.0);
	CdfShape*    box2 = new CdfBox("box2", 5.0, 5.0, 5.0);
	CdfShape*    box4 = new CdfBox("box3", 1.0, 2.0, 3.0);
	CdfLogicalVolume* log1 = new CdfLogicalVolume("logical_1", box1, stuff1, "log1", prop, bfield);
	CdfLogicalVolume* log2 = new CdfLogicalVolume("logical_2", box2, stuff2, "log2", prop, bfield);
	CdfLogicalVolume* log4 = new CdfLogicalVolume("logical_4", box4, stuff4, "log4", prop, bfield);
	HepPoint3D    p1;
	HepPoint3D    p2(1.0, 1.0, 1.0);
	HepPoint3D    p4(2.0, 2.0, 10.0);
	CdfPhysicalVolume* phys1 = new CdfPhysicalVolume("phys1", HepTransform3D(HepRotation(), p1), log1, 0); // top of the world
	CdfPhysicalVolume* phys2 = new CdfPhysicalVolume("phys2", HepTransform3D(HepRotation(), p2), log2, log1);
	CdfPhysicalVolume* phys4 = new CdfPhysicalVolume("phys4", HepTransform3D(HepRotation(), p4), log4, log1);
	
	CdfTangible* t1 = new CdfTangible(0, phys1);
	CdfTangible* t2 = new CdfTangible(t1, phys2);
	CdfTangible* t4 = new CdfTangible(t1, phys4);


	// Start with an empty integral.
	sim::PathIntegral path;
	location_t defaultLoc;
	assert ( path.location() == defaultLoc );
	assert ( path.integral() == sim::IntegrationData() );
	assert ( path.numLegs() == 0 );

	// Append one step.
	location_t  loc1(numbers, letters, 1);
	sim::IntegrationData d1(1.0, 2.0, 3.0, 4.0, 1);
	path.append(loc1, t1, p1, d1);
	assert ( path.numLegs() == 1 );
	assert ( path.leg(0) == d1 );
	assert ( path.integral() == d1 );
	assert ( path.integral(0, 0) == d1 );
	assert ( path.location(0) == loc1 );
	assert ( path.startPoint() == p1 );
	assert ( path.endPoint() == p1 );
	assert ( path.startPoint(0) == p1 );
	assert ( path.endPoint(0) == p1 );

	// Append another step in the same tangible.
	sim::IntegrationData d2(2.0, 4.0, 6.0, 8.0, 1);
	sim::IntegrationData sum_12 = d1 + d2;


	path.append(loc1, t1, p2, d2);
	assert ( path.numLegs() == 1 );
	assert ( path.leg(0) == sum_12 );
	assert ( path.integral() == sum_12 );
	assert ( path.integral(0, 0) == sum_12 );
	assert ( path.location() == loc1 );
	assert ( path.location(0) == loc1 );


	// Append a step in a new tangible

	location_t  loc2(numbers, letters, 2);
	sim::IntegrationData d4(1.0, 3.0, 5.0, 7.0, 2);
	sim::IntegrationData sum_14 = sum_12 + d4;

	path.append(loc2, t4, p4, d4);
	assert ( path.numLegs() == 2 );
	assert ( path.leg(0) == sum_12 );
	assert ( path.leg(1) == d4 );

	assert ( path.location() == loc2 );
	assert ( path.location(0) == loc1 );
	assert ( path.location(1) == loc2 );

	assert ( path.startPoint(0) == p1 );
	assert ( path.startPoint(0) == path.startPoint() );
	assert ( path.endPoint(0) == path.startPoint(1) );
	assert ( path.endPoint(1) == p4 );
	assert ( path.endPoint(1) == path.endPoint() );


	assert ( path.integral(0, 0) == sum_12 );
	if ( path.integral(0, 1) != sum_14 )
		{
			cout << "Expected: " << sum_14 << endl;
			cout << "Actual:   " << path.integral(0, 1) << endl;

			cout << path << endl;
		}
	assert ( path.integral() == sum_14 );

	if ( verbose )
		{
			cout << "Here is what a PathIntegral looks like:\n"
					 << path
					 << endl;
		}

	return 0;
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

