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

#include "pch.h"
#include "CWorldTransformData.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CWorldTransformData);

	IMPLEMENT_DATA_TYPE_INDEX(CWorldTransformData);

	CWorldTransformData::CWorldTransformData() :
		IsWorldTransform(false),
		HasChanged(true),
		NeedValidate(true),
		HasLateChanged(false),
		NeedValidateForLate(false),
		Depth(0),
		ParentIndex(-1),
		AttachParentIndex(-1),
		Parent(NULL)
	{
	}

	CWorldTransformData::~CWorldTransformData()
	{

	}

	bool CWorldTransformData::serializable(CMemoryStream* stream)
	{
		stream->writeString(Name);
		stream->writeInt(ParentIndex);
		stream->writeInt(Depth);
		stream->writeFloatArray(Relative.pointer(), 16);

		return true;
	}

	bool CWorldTransformData::deserializable(CMemoryStream* stream, int version)
	{
		Name = stream->readString();
		ParentIndex = stream->readInt();
		Depth = stream->readInt();
		stream->readFloatArray(Relative.pointer(), 16);

		HasChanged = true;
		return true;
	}

	void CWorldTransformData::attachParent(CWorldTransformData* parent)
	{
		AttachParentIndex = parent->EntityIndex;
		HasChanged = true;
		Entity->getEntityManager()->notifyUpdateSortEntities();
	}
}