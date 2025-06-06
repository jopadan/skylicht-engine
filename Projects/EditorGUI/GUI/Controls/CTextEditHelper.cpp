/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CTextEditHelper.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTextEditHelper::CTextEditHelper(CBase* base, CTextBox* textBox, CTextContainer* textContainer) :
				m_base(base),
				m_textBox(textBox),
				m_textContainer(textContainer)
			{
				m_textBox->setHidden(true);
				m_textBox->OnTextChanged = BIND_LISTENER(&CTextEditHelper::onChar, this);
				m_textBox->OnESC = BIND_LISTENER(&CTextEditHelper::onESC, this);
				m_textBox->OnEnter = BIND_LISTENER(&CTextEditHelper::onEnter, this);
				m_textBox->OnLostKeyboardFocus = BIND_LISTENER(&CTextEditHelper::onLostFocus, this);
			}

			CTextEditHelper::~CTextEditHelper()
			{

			}

			void CTextEditHelper::beginEdit(CBase::Listener onEndEdit, CBase::Listener onCancel)
			{
				updateTextBoxSize();

				m_oldValue = m_textContainer->getString();

				m_textBox->setString(m_oldValue);
				m_textBox->setWrapMultiline(m_textContainer->isWrapMultiline());

				m_textBox->setHidden(false);
				m_textBox->focus();

				m_textContainer->setHidden(true);

				m_onEndEdit = onEndEdit;
				m_onCancel = onCancel;

				m_end = false;
			}

			void CTextEditHelper::cancelEdit()
			{
				if (m_end == true)
					return;

				m_textBox->setHidden(true);
				m_textContainer->setHidden(false);
				m_textContainer->setString(m_oldValue);

				m_end = true;

				if (m_onCancel != nullptr)
					m_onCancel(m_textBox);
			}

			void CTextEditHelper::endEdit()
			{
				m_textContainer->setString(m_textBox->getString());

				m_textBox->setHidden(true);
				m_textContainer->setHidden(false);

				m_end = true;

				if (m_onEndEdit != nullptr)
					m_onEndEdit(m_textBox);
			}

			void CTextEditHelper::onChar(CBase* textBox)
			{
				if (m_end == true)
					return;

				// auto calc text size
				m_textContainer->setString(m_textBox->getString());
				m_textContainer->layout();

				updateTextBoxSize();

				// fix for scroll view
				m_textContainer->setString(m_oldValue);
			}

			void CTextEditHelper::updateTextBoxSize()
			{
				SPoint p1 = m_textContainer->localPosToCanvas();
				SPoint p2 = m_base->localPosToCanvas();

				SRect bound = m_textContainer->getBounds();
				bound.Y -= 3.0f;
				bound.Height += 5.0f;
				bound.Width += 8.0f;

				SRect maxBound = m_base->getBounds();

				float x1 = p1.X + bound.Width;
				float x2 = p2.X + maxBound.Width - 4.0f;

				if (x1 > x2)
				{
					float delta = x1 - x2;
					bound.Width -= delta;
				}

				m_textBox->setBounds(bound);
			}

			void CTextEditHelper::onESC(CBase* textBox)
			{
				cancelEdit();
			}

			void CTextEditHelper::onEnter(CBase* textBox)
			{
				endEdit();
			}

			void CTextEditHelper::onLostFocus(CBase* textBox)
			{
				if (m_textBox->isOnCharEvent())
				{
					// ignore event lostFocus when press Enter or ESC
					return;
				}

				// process event when mouse click out the textbox
				cancelEdit();
			}
		}
	}
}