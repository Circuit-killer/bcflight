/*
 * BCFlight
 * Copyright (C) 2016 Adrien Aubry (drich)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <unistd.h>
#include "ADS1015.h"

int ADS1015::flight_register( Main* main )
{
	Device dev;
	dev.iI2CAddr = 0x48;
	dev.name = "ADS1015";
	dev.fInstanciate = ADS1015::Instanciate;
	mKnownDevices.push_back( dev );
	return 0;
}


Sensor* ADS1015::Instanciate( Config* config, const string& object )
{
	return new ADS1015();
}


ADS1015::ADS1015()
	: mI2C( new I2C( 0x48 ) )
	, mRingBuffer{ 0.0f }
	, mRingSum( 0.0f )
	, mRingIndex( 0 )
{
	mNames = { "ADS1015" };
}


ADS1015::~ADS1015()
{
	delete mI2C;
}


void ADS1015::Calibrate( float dt, bool last_pass )
{
	(void)dt;
}


float ADS1015::Read( int channel )
{
	uint16_t _ret = 0;
	uint32_t ret = 0;
	uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
					  ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
					  ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
					  ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
					  ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
					  ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

	config |= ADS1015_REG_CONFIG_PGA_6_144V;
	config |= ( ADS1015_REG_CONFIG_MUX_SINGLE_0 + ( channel << 12 ) );
	config |= ADS1015_REG_CONFIG_OS_SINGLE;
	config = ( ( config << 8 ) & 0xFF00 ) | ( ( config >> 8 ) & 0xFF );

	mI2C->Write16( ADS1015_REG_POINTER_CONFIG, config );
	usleep( 8000 );
	mI2C->Read16( ADS1015_REG_POINTER_CONVERT, &_ret );

	ret = _ret;
	ret = ( ( ret << 8 ) & 0xFF00 ) | ( ( ret >> 8 ) & 0xFF );
	float fret = (float)( ret * 6.144f / 32768.0f );

	return fret;
}


string ADS1015::infos()
{
	return "I2Caddr = " + to_string( mI2C->address() ) + ", " + "Resolution = \"12 bits\", " + "Channels = 4";
}
