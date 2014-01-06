////////////////////////////////////////////////////////////////////////
//
// File: testVolumeNamePrinter.cc
// $Id: testVolumeNamePrinter.cc,v 1.1 2001/04/09 14:54:35 paterno Exp $
// Purpose: Test of VolumeNamePrinter struct.
//
// Author:  Marc Paterno
// Created: 27 Mar 2001
// Modified:
//
////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <algorithm>

#include "SimulationMods/VolumeNamePrinter.hh"

using namespace std;

struct fourchar
{
	char chars[4];
};

void
fill_fc (fourchar& fc, const char input[4])
{
	copy(input + 0, input + 4, fc.chars);	
}

union G3Name
{
	int      ival;
	fourchar chars;

};

void do_output(int i)
{
	sim::VolumeNamePrinter p(&i);
	cout << p << endl;
}

int main(int argc, char* argv[])
{
	bool verbose = ( argc > 1 );
	if ( verbose ) cout << "Running " << argv[0] << endl;

	G3Name n1, n2;
	fill_fc(n1.chars, "abcd");
	fill_fc(n2.chars, "WXYZ");

	do_output(n1.ival);
	do_output(n2.ival);
	
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

