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

#ifndef RASPICAM_H
#define RASPICAM_H

#include <stdio.h>
#include <fstream>
#include <functional>
#include <Thread.h>
#include <Config.h>
#include "Camera.h"

#include "../../external/OpenMaxIL++/include/Camera.h"
#include "../../external/OpenMaxIL++/include/VideoEncode.h"

class Main;
class Link;

class Raspicam : public Camera, protected IL::Camera
{
public:
	Raspicam( Config* config, const std::string& conf_obj );
	~Raspicam();

	virtual void Pause();
	virtual void Resume();
	virtual void StartRecording();
	virtual void StopRecording();

	virtual const uint32_t brightness();
	virtual const int32_t contrast();
	virtual const int32_t saturation();
	virtual const bool nightMode();
	virtual void setBrightness( uint32_t value );
	virtual void setContrast( int32_t value );
	virtual void setSaturation( int32_t value );
	virtual void setNightMode( bool night_mode );

	virtual uint32_t* getFileSnapshot( const std::string& filename, uint32_t* width, uint32_t* height, uint32_t* bpp );

protected:
	bool LiveThreadRun();

	int LiveSend( char* data, int datalen );
	int RecordWrite( char* data, int datalen, int64_t pts = 0, bool audio = false );

	Config* mConfig;
	std::string mConfigObject;
	Link* mLink;
	IL::VideoEncode* mEncoder;
	HookThread<Raspicam>* mLiveThread;
	uint64_t mLiveFrameCounter;
	uint64_t mLiveTicks;
	uint64_t mRecordTicks;
	uint64_t mLedTick;
	uint64_t mHeadersTick;
	bool mLedState;
	bool mNightMode;
	bool mPaused;

	// Record
	bool mRecording;
	char* mRecordFrameData;
	int mRecordFrameDataSize;
	int mRecordFrameSize;
	uint32_t mRecordSyncCounter;

	FILE* mRecordStream; // TODO : use board-specific file instead
};

#endif // RASPICAM_H
