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
#include "CShaderMaterial.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	CMaterial* g_material = NULL;
	SColorf g_colorIntensity(1.0f, 1.0f, 1.0f, 1.0f);

	void CShaderMaterial::setMaterial(CMaterial* material)
	{
		g_material = material;
	}

	CMaterial* CShaderMaterial::getMaterial()
	{
		return g_material;
	}

	void CShaderMaterial::setColorIntensity(const SColorf& color)
	{
		g_colorIntensity = color;
	}

	SColorf CShaderMaterial::getColorIntensity()
	{
		return g_colorIntensity;
	}

	CShaderMaterial::CShaderMaterial()
	{

	}

	CShaderMaterial::~CShaderMaterial()
	{

	}

	void CShaderMaterial::OnSetConstants(CShader* shader, SUniform* uniform, IMaterialRenderer* matRender, bool vertexShader)
	{
		switch (uniform->Type)
		{
		case MATERIAL_PARAM:
		{
			if (g_material != NULL)
			{
				float* f = g_material->getShaderParams().getParamData(uniform->ValueIndex);
				if (vertexShader == true)
					matRender->setShaderVariable(uniform->UniformShaderID, f, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform->UniformShaderID, f, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
			}
		}
		break;
		case COLOR_INTENSITY:
		{
			float* f = &g_colorIntensity.r;
			if (vertexShader == true)
				matRender->setShaderVariable(uniform->UniformShaderID, f, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform->UniformShaderID, f, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
		}
		break;
		default:
			break;
		}
	}
}