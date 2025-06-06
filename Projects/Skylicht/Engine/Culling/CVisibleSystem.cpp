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
#include "CVisibleSystem.h"
#include "RenderPipeline/IRenderPipeline.h"
#include "Culling/CCullingSystem.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	CVisibleSystem::CVisibleSystem() :
		m_group(NULL)
	{

	}

	CVisibleSystem::~CVisibleSystem()
	{

	}

	void CVisibleSystem::beginQuery(CEntityManager* entityManager)
	{
		if (!m_group)
		{
			// get visible group
			// CVisibleGroup added in CEntityManager constructor
			const u32 visibleGroupType[] = GET_LIST_ENTITY_DATA(CVisibleData);
			m_group = entityManager->findGroup(visibleGroupType, 1);
		}
	}

	void CVisibleSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{

	}

	void CVisibleSystem::init(CEntityManager* entityManager)
	{

	}

	void CVisibleSystem::update(CEntityManager* entityManager)
	{
		if (CCullingSystem::useCacheCulling())
			return;

		IRenderPipeline* rp = entityManager->getRenderPipeline();
		if (rp == NULL)
			return;

		CCamera* camera = entityManager->getCamera();
		if (camera == NULL)
			return;

		u32 cullingMask = camera->getCullingMask();

		CVisibleData* visible;
		CEntity** entities = m_group->getEntities();
		u32 numEntity = m_group->getEntityCount();
		CEntity* entity;

		for (u32 i = 0; i < numEntity; i++)
		{
			entity = entities[i];

			visible = GET_ENTITY_DATA(entity, CVisibleData);
			visible->Culled = !(visible->CullingLayer & cullingMask);
		}
	}

	void CVisibleSystem::render(CEntityManager* entityManager)
	{

	}
}