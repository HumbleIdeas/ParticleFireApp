// Particle Fire Container class - source 

// This file is part of Particle Fire.
// 
// Particle Fire is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Particle Fire is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Particle Fire.  If not, see <http://www.gnu.org/licenses/>.

#include <time.h>
#include <math.h>
#include <windows.h>
#include <stdio.h>

#include <CStr.h>
#include "ParticleFire.h"
#include <Timer.h>
#include <Basis.h>
#include <Reg.h>

#include "defines.h"

#include "ParticleContainer.hpp"

// External functions
extern void error_print (char *buff);

ParticleContainer::ParticleContainer ()
{
	Init ();
}

ParticleContainer::~ParticleContainer ()
{

}
	
void ParticleContainer::Init ()
{
	wcscpy (QuoteFilename, L"\0");

	XMouse = 0;
	YMouse = 0;

//	TimeStart; Time; LastTime;		// Not init'd

//	p;				// Not init'd
//	pe; cf; ct;		// Not init'd

	// Clear before setting
	tdata = NULL;
	tdata2 = NULL;

	// Save parent pointers for sub-classes
	this->screen.parent = this;
	this->particle.parent = this;
	this->registry.parent = this;
}

// QPC helper
static double QpcMs(LARGE_INTEGER a, LARGE_INTEGER b, LARGE_INTEGER freq)
{
	return (double)(b.QuadPart - a.QuadPart) * 1000.0 / (double)freq.QuadPart;
}


void ParticleContainer::Frame ()
{
	static LARGE_INTEGER freq = []() { LARGE_INTEGER f; QueryPerformanceFrequency(&f); return f; }();

	static int frames = 0;
	static double simMsSum = 0.0;
	static double drawMsSum = 0.0;
	static double totalMsSum = 0.0;

	LARGE_INTEGER t0{}, t1{}, t2{}, t3{};

	QueryPerformanceCounter(&t0);

	// 1) simulation / math
	QueryPerformanceCounter(&t1);

	this->HandleParticleStyle ();

	QueryPerformanceCounter(&t2);

	// 2) drawing (this is where your visible-span cull affects work)

	this->HandleScreen ();

	QueryPerformanceCounter(&t3);

	const double simMs = QpcMs(t1, t2, freq);
	const double drawMs = QpcMs(t2, t3, freq);
	const double totMs = QpcMs(t0, t3, freq);

	simMsSum += simMs;
	drawMsSum += drawMs;
	totalMsSum += totMs;
	frames++;

	// Log every ~120 frames (~5 seconds at ~24 FPS (Frames Per Second))
	if (frames % 120 == 0) {
		PF_LogFmtW(L"PF PERF: avg over %d frames: sim=%.3fms draw=%.3fms total=%.3fms\n",
			frames, simMsSum / frames, drawMsSum / frames, totalMsSum / frames);
	}

}

void ParticleContainer::HandleParticleStyle ()
{
	particle.Frame ();
}

void ParticleContainer::HandleScreen ()
{
	screen.HandleText ();
	screen.Draw ();
}
