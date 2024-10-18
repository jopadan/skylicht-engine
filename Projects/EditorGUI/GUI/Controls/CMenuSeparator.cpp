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
#include "CMenuSeparator.h"
#include "GUI/Theme/ThemeConfig.h"
#include "GUI/Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CMenuSeparator::CMenuSeparator(CBase *parent) :
				CBase(parent)
			{
				setHeight(9.0f);
			}

			CMenuSeparator::~CMenuSeparator()
			{
			}

			void CMenuSeparator::layout()
			{
				setWidth(m_parent->width());
				CBase::layout();
			}

			void CMenuSeparator::renderUnder()
			{
				SRect renderBounds = getRenderBounds();
				renderBounds.X = 1.0f;
				renderBounds.Y = 5.0f;
				renderBounds.Height = 1.0f;
				renderBounds.Width = renderBounds.Width - 2.0f;

				CRenderer::getRenderer()->drawFillRect(renderBounds, ThemeConfig::MenuItemSeparatorColor);
			}
		}
	}
}