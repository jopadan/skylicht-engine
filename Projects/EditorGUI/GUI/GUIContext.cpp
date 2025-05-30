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
#include "GUIContext.h"
#include "GUI/Controls/CCanvas.h"

#include "GUI/Renderer/CSkylichtRenderer.h"
#include "GUI/Theme/CSkylichtTheme.h"
#include "GUI/Input/CSkylichtInput.h"
#include "GUI/Clipboard/CSkylichtClipboard.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			namespace Context
			{
				CBase* HoveredControl = NULL;
				CBase* MouseFocus = NULL;
				CBase* KeyboardFocus = NULL;

				CCanvas* g_rootCanvas = NULL;
				CInput* g_input = NULL;

				CSkylichtRenderer* g_renderer = NULL;
				CSkylichtTheme* g_theme = NULL;
				CSkylichtClipboard* g_clipboard = NULL;

				float g_guiUpdateTime = 0.0f;
				float g_guiDeltaTime = 0.0f;

				void initGUI(float width, float height)
				{
					g_rootCanvas = new CCanvas(width, height);

					g_renderer = new CSkylichtRenderer(width, height);
					CRenderer::setRenderer(g_renderer);

					g_theme = new CSkylichtTheme();
					CTheme::setTheme(g_theme);

					g_input = new CSkylichtInput();
					CInput::setInput(g_input);

					g_clipboard = new CSkylichtClipboard();
					CClipboard::setClipboard(g_clipboard);
				}

				void destroyGUI()
				{
					if (g_rootCanvas != NULL)
						delete g_rootCanvas;

					if (g_renderer != NULL)
						delete g_renderer;

					if (g_theme != NULL)
						delete g_theme;

					if (g_input != NULL)
						delete g_input;

					if (g_clipboard != NULL)
						delete g_clipboard;
				}

				void update(float time)
				{
					if (g_rootCanvas != NULL)
						g_rootCanvas->update();

					g_guiDeltaTime = time - g_guiUpdateTime;

					if (g_guiDeltaTime > 1000.0f / 10.0f)
						g_guiDeltaTime = 1000.0f / 10.0f;

					g_guiUpdateTime = time;
				}

				float getTime()
				{
					return g_guiUpdateTime;
				}

				float getDeltaTime()
				{
					return g_guiDeltaTime;
				}

				void render()
				{
					if (g_rootCanvas != NULL)
						g_rootCanvas->doRender();
				}

				void resize(float width, float height)
				{
					if (g_renderer != NULL)
						g_renderer->resize(width, height);

					if (g_rootCanvas != NULL)
					{
						g_rootCanvas->setBounds(0.0f, 0.0f, width, height);
						g_rootCanvas->closeMenu();
						g_rootCanvas->notifySaveDockLayout();
					}
				}

				CCanvas* getRoot()
				{
					return g_rootCanvas;
				}

				CRenderer* getRenderer()
				{
					return g_renderer;
				}
			}
		}
	}
}