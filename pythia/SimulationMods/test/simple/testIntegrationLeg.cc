////////////////////////////////////////////////////////////////////////
//
// File: testIntegrationLeg.cc
// $Id: testIntegrationLeg.cc,v 1.4 2001/04/23 16:03:57 paterno Exp $
// Purpose: Test program for the IntegrationLeg class.
//
// Note that functions relating to calling of functions from a
// CdfTangible are *not* tested here, because they introduce far too
// wide-spread physical coupling problems.
//
// Author:  Marc Paterno
// Created: 26 Feb 2001
// Modified:
//
////////////////////////////////////////////////////////////////////////

#include <vector>
#include <cassert>
#include <iostream>
#include <string>

#include "SimulationMods/IntegrationLeg.hh"
#include "GeometryBase/CdfTangible.hh"
#include "GeometryBase/CdfPhysicalVolume.hh"
#include "GeometryBase/CdfLogicalVolume.hh"
#include "GeometryBase/CdfMaterial.hh"
#include "GeometryBase/CdfShape.hh"
#include "GeometryBase/CdfBox.hh"
#include "GeometryBase/CdfSteppingProperties.hh"

typedef std::vector<sim::IntegrationLeg> vector;
typedef sim::IntegrationLeg::location_t location_t;
typedef sim::IntegrationLeg::point_t    point_t;

int main()
{
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
	HepPoint3D    p2(5.0, 5.0, 5.0);
	HepPoint3D    p4(10.0, 20.0, 20.0);
	CdfPhysicalVolume* phys1 = new CdfPhysicalVolume("phys1", HepTransform3D(HepRotation(), p1), log1, 0); // top of the world
	CdfPhysicalVolume* phys2 = new CdfPhysicalVolume("phys2", HepTransform3D(HepRotation(), p2), log2, log1);
	CdfPhysicalVolume* phys4 = new CdfPhysicalVolume("phys4", HepTransform3D(HepRotation(), p4), log4, log1);
	
	CdfTangible* t1 = new CdfTangible(0, phys1);
	CdfTangible* t2 = new CdfTangible(t1, phys2);
	CdfTangible* t4 = new CdfTangible(t1, phys4);



	sim::IntegrationLeg i0;
	sim::IntegrationData d0;
	location_t defaultLocation;
	assert ( i0.where() == defaultLocation );
	assert ( i0.current() == d0 );
	assert ( i0.cumulative() == d0 );

	sim::IntegrationLeg i1(defaultLocation, t1, p1);
	assert ( i1.where() == defaultLocation );
	assert ( i1.current() == d0 );
	assert ( i1.cumulative() == d0 );
	assert ( i1.startPoint() == p1 );
	assert ( i1.endPoint() == p1 );
	


	location_t loc2(numbers, letters, 1);
	sim::IntegrationData d2(1.0, 2.0, 3.0, 4.0, 2);
	sim::IntegrationLeg i2(loc2, t2, p2,  d2);
	assert ( i2.where() == loc2 );
	assert ( i2.current() == d2 );
	assert ( i2.cumulative() == d2 );
	assert ( i2.startPoint() == p2 );
	assert ( i2.endPoint() == p2 );

	sim::IntegrationData d3(11.0, 0.0, 20.0, 17.0, 3);
	point_t  p3(2.0, 3.0, 4.0);
	sim::IntegrationLeg i3(i2);
	assert ( i3 == i2 );
	i3.accumulate(p3, d3);
	assert ( i3.current() == (d2 + d3) );
	assert ( i3.cumulative() == (d2 + d3) );
	assert ( i3.startPoint() == p2 );
	assert ( i3.endPoint() == p3 );

	// Make sure these critters can go into a vector...
	typedef std::vector<sim::IntegrationLeg> vector;
	vector v0;
	v0.push_back(i0);
	v0.push_back(i1);
	v0.push_back(i2);
	vector v1(v0);

	// Test out the fancy constructor...
	
	location_t      loc3(numbers, letters, 2);
	sim::IntegrationData d4(100.0, 200.0, 300.0, 400.0, 500);
	sim::IntegrationData expected_integral = d4 + i3.cumulative();
	sim::IntegrationLeg i4(loc3, t4, p4, i3, d4);
	assert ( i4.where() == loc3);
	assert ( i4.current() == d4 );
	assert ( i4.cumulative() == expected_integral );
	if ( i4.startPoint() != i3.endPoint() )
		{
			std::cout << "i4 start: " << i4.startPoint() << '\n';
			std::cout << "i3 end:   " << i3.endPoint() << '\n';
		}
	assert ( i4.startPoint() == i3.endPoint() );
	assert ( i4.endPoint() == p4 );

	// How much stuff is leaked here? Lots! But the geometry is so
	// complex, it isn't easy to know what we own, and what is owned by
	// something else...
	return 0;
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

