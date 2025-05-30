/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CGRenderMeshInstancing.h"

#include "Utils/CActivator.h"
#include "Components/CDependentComponent.h"

#include "GameObject/CGameObject.h"
#include "Transform/CWorldInverseTransformData.h"
#include "Entity/CEntityManager.h"
#include "Entity/CEntityChildsData.h"

namespace Skylicht
{
	namespace Editor
	{
		ACTIVATOR_REGISTER(CGRenderMeshInstancing);

		DEPENDENT_COMPONENT(CRenderMeshInstancing, CGRenderMeshInstancing);

		CGRenderMeshInstancing::CGRenderMeshInstancing() :
			m_renderMeshInstancing(NULL)
		{

		}

		CGRenderMeshInstancing::~CGRenderMeshInstancing()
		{

		}

		void CGRenderMeshInstancing::initComponent()
		{
			m_renderMeshInstancing = m_gameObject->getComponent<CRenderMeshInstancing>();

			updateSelectBBox();
		}

		void CGRenderMeshInstancing::updateComponent()
		{
			updateSelectBBox();
		}

		void CGRenderMeshInstancing::updateSelectBBox()
		{
			CEntityManager* entityMgr = m_gameObject->getEntityManager();

			core::array<CEntity*>& entities = m_renderMeshInstancing->getEntities();
			for (u32 i = 0, n = entities.size(); i < n; i++)
			{
				CEntity* entity = entities[i];

				CSelectObjectData* selectObjectData = GET_ENTITY_DATA(entity, CSelectObjectData);
				if (selectObjectData == NULL)
					selectObjectData = entity->addData<CSelectObjectData>();

				CWorldInverseTransformData* worldInv = GET_ENTITY_DATA(entity, CWorldInverseTransformData);
				if (worldInv == NULL)
					worldInv = entity->addData<CWorldInverseTransformData>();

				// select bbox data
				selectObjectData->GameObject = m_gameObject;
				selectObjectData->Entity = entity;

				CEntityChildsData* childs = GET_ENTITY_DATA(entity, CEntityChildsData);
				for (u32 j = 0, m = (u32)childs->Childs.size(); j < m; j++)
				{
					CEntity* renderEntity = childs->Childs[j];

					CRenderMeshData* renderMesh = GET_ENTITY_DATA(renderEntity, CRenderMeshData);
					if (renderMesh->getMesh())
					{
						core::aabbox3df bbox = renderMesh->getMesh()->getBoundingBox();

						CWorldTransformData* transform = GET_ENTITY_DATA(renderEntity, CWorldTransformData);
						transform->Relative.transformBoxEx(bbox);

						if (j == 0)
							selectObjectData->BBox = bbox;
						else
							selectObjectData->BBox.addInternalBox(bbox);
					}
				}
			}
		}
	}
}