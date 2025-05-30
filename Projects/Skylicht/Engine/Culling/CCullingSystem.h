/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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

#pragma once

#include "CCullingData.h"
#include "CVisibleData.h"
#include "Entity/CEntityGroup.h"
#include "Entity/IRenderSystem.h"
#include "Transform/CWorldTransformData.h"
#include "Transform/CWorldInverseTransformData.h"
#include "RenderMesh/CRenderMeshData.h"

namespace Skylicht
{
	struct SBBoxAndMaterial
	{
		CEntity* Entity;

		CCullingData* Culling;

		CWorldTransformData* Transform;

		core::aabbox3df* BBox;

		// Material to check render pipeline cull
		ArrayMaterial* Materials;

		SBBoxAndMaterial()
		{
			Entity = NULL;
			BBox = NULL;
			Culling = NULL;
			Materials = NULL;
			Transform = NULL;
		}
	};

	class SKYLICHT_API CCullingSystem : public IRenderSystem
	{
	protected:
		CFastArray<SBBoxAndMaterial> m_bboxAndMaterials;

		CEntityGroup* m_group;

	public:
		CCullingSystem();

		virtual ~CCullingSystem();

		virtual void beginQuery(CEntityManager* entityManager);

		virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity);

		virtual void init(CEntityManager* entityManager);

		virtual void update(CEntityManager* entityManager);

		virtual void render(CEntityManager* entityManager);

		virtual void postRender(CEntityManager* entityManager);

		static void useCacheCulling(bool b);

		static bool useCacheCulling();
	};
}