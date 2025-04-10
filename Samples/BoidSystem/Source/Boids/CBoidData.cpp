#include "pch.h"
#include "CBoidData.h"

IMPLEMENT_DATA_TYPE_INDEX(CBoidData);

CBoidData::CBoidData() :
	MaxSpeed(0.0f),
	MaxForce(0.0f),
	Alive(true)
{
	MaxSpeed = 2.5f;
	MaxForce = 0.02f;

	float rx = os::Randomizer::frand() * 2.0f - 1.0f;
	float rz = os::Randomizer::frand() * 2.0f - 1.0f;

	Velocity.set(rx, 0.0f, rz);

	Front = Velocity;
	Front.normalize();
}

CBoidData::~CBoidData()
{

}