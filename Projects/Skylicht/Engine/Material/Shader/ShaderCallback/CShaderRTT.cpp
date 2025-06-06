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
#include "CShaderRTT.h"
#include "Camera/CCamera.h"
#include "GameObject/CGameObject.h"
#include "RenderPipeline/CRenderToTextureRP.h"

namespace Skylicht
{
	CShaderRTT::CShaderRTT()
	{

	}

	CShaderRTT::~CShaderRTT()
	{

	}

	void CShaderRTT::OnSetConstants(CShader* shader, SUniform* uniform, IMaterialRenderer* matRender, bool vertexShader)
	{
		IVideoDriver* driver = getVideoDriver();

		switch (uniform->Type)
		{
		case RENDER_TEXTURE_MATRIX:
		{
			const float* matrix = CRenderToTextureRP::getMatrix(uniform->ValueIndex);
			if (vertexShader == true)
				matRender->setShaderVariable(uniform->UniformShaderID, matrix, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform->UniformShaderID, matrix, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
		}
		break;
		default:
			break;
		}
	}

	ITexture* g_lastFrame = NULL;
	ITexture* g_rttTexture[video::MATERIAL_MAX_TEXTURES] = { NULL };

	void CShaderRTT::setRTTTexture(int id, ITexture* texture)
	{
		if (id >= 0 && id < video::MATERIAL_MAX_TEXTURES)
			g_rttTexture[id] = texture;
	}

	ITexture* CShaderRTT::getRTTTexture(int id)
	{
		if (id >= 0 && id < video::MATERIAL_MAX_TEXTURES)
			return g_rttTexture[id];
		return NULL;
	}

	void CShaderRTT::setLastFrameTexture(ITexture* texture)
	{
		g_lastFrame = texture;
	}

	ITexture* CShaderRTT::getLastFrameTexture()
	{
		return g_lastFrame;
	}
}