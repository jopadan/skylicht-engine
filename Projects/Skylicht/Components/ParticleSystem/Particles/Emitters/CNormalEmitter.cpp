/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CNormalEmitter.h"
#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/Zones/CZone.h"
#include "ParticleSystem/Particles/CGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		CNormalEmitter::CNormalEmitter() :
			CEmitter(Normal),
			m_inverted(false)
		{
		}

		CNormalEmitter::~CNormalEmitter()
		{

		}

		CObjectSerializable* CNormalEmitter::createSerializable()
		{
			CObjectSerializable* object = CEmitter::createSerializable();

			CBoolProperty* inverted = new CBoolProperty(object, "inverted", m_inverted);
			inverted->setUIHeader("Normal Emitter");
			object->autoRelease(inverted);

			return object;
		}

		void CNormalEmitter::loadSerializable(CObjectSerializable* object)
		{
			CEmitter::loadSerializable(object);
			m_inverted = object->get<bool>("inverted", false);
		}

		void CNormalEmitter::generateVelocity(CParticle& particle, float speed, CZone* zone, CGroup* group)
		{
			if (m_inverted)
				speed = -speed;

			particle.Velocity = zone->computeNormal(particle.Position, group);

			particle.Velocity = group->getTransformVector(particle.Velocity);
			particle.Velocity.normalize();
			particle.Velocity *= speed;
		}
	}
}