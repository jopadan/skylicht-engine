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

#include "CAnimationTimeline.h"
#include "CAnimationTransformData.h"
#include "Entity/CEntityPrefab.h"
#include "Animation/CAnimationClip.h"

namespace Skylicht
{
	class SKYLICHT_API CSkeleton
	{
	public:
		enum EAnimationType
		{
			KeyFrame = 0,
			Blending,
		};

		enum EAnimationLayerType
		{
			DefaultBlending,
			Replace,
			Addtive,
		};

	protected:
		CEntityPrefab m_entities;

		std::vector<CAnimationTransformData*> m_entitiesData;

		core::array<CAnimationTransformData*> m_entitiesActivated;

		CAnimationTransformData* m_root;

		int m_id;

		bool m_enable;

		bool m_needUpdateActivateEntities;

		CAnimationTimeline m_timeline;

		EAnimationType m_animationType;

		CAnimationClip* m_clip;

		EAnimationLayerType m_layerType;

	protected:

		CSkeleton* m_target;

		std::vector<CSkeleton*> m_blending;

	public:
		CSkeleton(int id);

		virtual ~CSkeleton();

		void initSkeleton(core::array<CEntity*>& entities);

		void releaseAllEntities();

		void update();

		void updateActivateEntities();

		int simulateTransform(float timeSecond, core::matrix4 origin, core::matrix4* transforms, int numTransform);

		void getBoneIdMap(std::map<std::string, int>& bones);

		void applyTransform();

		void syncAnimationByTimeScale();

		void setAnimation(CAnimationClip* clip, bool loop, bool pause = false);

		void setAnimation(CAnimationClip* clip, bool loop, float from, float duration, bool pause = false);

		void setJointWeights(float weight);

		void setJointWeights(const char* name, float weight, bool includeChild = true);

		void getJoints(const char* name, std::vector<CAnimationTransformData*>& joints);

		void setJointWeights(std::vector<CAnimationTransformData*>& joints, float weight);

		CAnimationTransformData* getJoint(const char* name);

		inline std::vector<CAnimationTransformData*>& getJoints()
		{
			return m_entitiesData;
		}

		inline CAnimationClip* getCurrentAnimation()
		{
			return m_clip;
		}

		inline int getID()
		{
			return m_id;
		}

		inline bool isEnable()
		{
			return m_enable;
		}

		inline void setEnable(bool b)
		{
			m_enable = b;
		}

		inline CAnimationTimeline& getTimeline()
		{
			return m_timeline;
		}

		inline void setAnimationType(EAnimationType type)
		{
			m_animationType = type;
		}

		inline EAnimationType getAnimationType()
		{
			return m_animationType;
		}

		inline void setLayerType(EAnimationLayerType type)
		{
			m_layerType = type;
		}

		inline EAnimationLayerType getLayerType()
		{
			return m_layerType;
		}

		inline void notifyUpdateActivateEntities()
		{
			m_needUpdateActivateEntities = true;
		}

		void setTarget(CSkeleton* skeleton);

		void drawDebug(const core::matrix4& transform, const SColor& c);

		void drawDebugDefaultSkeleton(const core::matrix4& transform, const SColor& c);

	protected:

		void setAnimationData();

		void updateTrackKeyFrame();

		void updateBlending();

		void doBlending(CSkeleton* skeleton, bool first);

		void doAddtive(CSkeleton* skeleton, bool first);

		void doReplace(CSkeleton* skeleton, bool first);

		void addBlending(CSkeleton* skeleton);

		void removeBlending(CSkeleton* skeleton);
	};
}