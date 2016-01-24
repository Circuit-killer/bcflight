#include <unistd.h>
#include <Main.h>
#include "L3GD20H.h"

int L3GD20H::flight_register( Main* main )
{
	Device dev = { 0x6b, L3GD20H::Instanciate };
	mKnownDevices.push_back( dev );
	return 0;
}


Gyroscope* L3GD20H::Instanciate()
{
	return new L3GD20H();
}


L3GD20H::L3GD20H()
	: Gyroscope()
	, mI2C( new I2C( 0x6b ) )
	, mCalibrationAccum( Vector4f() )
	, mOffset( Vector3f() )
{
	mNames = { "l3gd20", "l3gd20h" };
	mAxes[0] = true;
	mAxes[1] = true;
	mAxes[2] = true;

	mI2C->Write8( L3GD20_CTRL_REG2, 0b00000000 );
	mI2C->Write8( L3GD20_CTRL_REG3, 0b00001000 );
	mI2C->Write8( L3GD20_CTRL_REG4, 0b00110000 );
	mI2C->Write8( L3GD20_CTRL_REG5, 0b01000000 );
	mI2C->Write8( L3GD20_FIFO_CTRL_REG, 0b01000000 );
	mI2C->Write8( L3GD20_CTRL_REG1, 0b11111111 );
}


L3GD20H::~L3GD20H()
{
}


void L3GD20H::Calibrate( float dt, bool last_pass )
{
	Vector3f gyro;
	Read( &gyro, true );
	mCalibrationAccum += Vector4f( gyro, 1.0f );

	if ( last_pass ) {
		mOffset = mCalibrationAccum.xyz() / mCalibrationAccum.w;
		aDebug( "Offset", mOffset.x, mOffset.y, mOffset.z );
		mCalibrationAccum = Vector4f();
		mCalibrated = true;
	}
}


void L3GD20H::Read( Vector3f* v, bool raw )
{
	short sgyro[3] = { 0 };

	mI2C->Read( L3GD20_OUT_X_L | 0x80, sgyro, sizeof(sgyro) );
	v->x = 0.0703125f * (float)sgyro[0];
	v->y = 0.0703125f * (float)sgyro[1];
	v->z = 0.0703125f * (float)sgyro[2];

	v->operator-=( mOffset );
	ApplySwap( *v );

	mLastValues = *v;
}