/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CGUIEditor.h"
#include "Editor/CEditor.h"
#include "Editor/Space/GUIDesign/CSpaceGUIDesign.h"
#include "Editor/SpaceController/CGUIDesignController.h"
#include "Selection/CSelection.h"

namespace Skylicht
{
	namespace Editor
	{
		CGUIEditor::CGUIEditor() :
			m_gui(NULL),
			m_guiData(NULL),
			m_isChanged(false),
			m_objVisible(true)
		{

		}

		CGUIEditor::~CGUIEditor()
		{

		}

		void CGUIEditor::closeGUI()
		{
			if (m_isChanged && m_gui)
			{
				// save history
				CGUIDesignController::getInstance()->getHistory()->saveModifyHistory(m_gui);
			}

			if (m_guiData)
				delete m_guiData;

			m_gui = NULL;
			m_guiData = NULL;
			CSerializableEditor::closeGUI();
		}

		void CGUIEditor::initGUI(CGUIElement* gui, CSpaceProperty* ui)
		{
			m_isChanged = false;
			m_gui = gui;
			m_guiData = m_gui->createSerializable();
			m_objVisible = gui->isVisible();

			// hide id field
			m_guiData->getProperty("id")->setHidden(true);
		}

		void CGUIEditor::onUpdateValue(CObjectSerializable* object)
		{
			if (object)
				m_isChanged = true;

			m_gui->loadSerializable(m_guiData);

			// notify to update on GUI
			CSelection* selection = CSelection::getInstance();
			selection->notify(m_gui, this);

			// also update on current select gizmos
			CSelectObject* selectObject = selection->getLastSelected();
			if (selectObject != NULL)
			{
				if (selectObject->getID() == m_gui->getID())
				{
					CSpace* space = CEditor::getInstance()->getWorkspaceByName(std::wstring(L"GUI Design"));
					if (space)
					{
						CSpaceGUIDesign* guiDesign = dynamic_cast<CSpaceGUIDesign*>(space);
						CGUITransformGizmos* gizmos = guiDesign->getGizmos();

						// update transform first
						m_gui->getCanvas()->updateEntities();

						// apply for Gizmos
						gizmos->setTransform(
							m_gui->getAlignPosition(),
							m_gui->getRotation(),
							m_gui->getScale(),
							m_gui->getRect()
						);
					}
				}
			}

			// update visible for all selected guis
			if (m_gui->isVisible() != m_objVisible)
			{
				m_objVisible = m_gui->isVisible();

				std::vector<CSelectObject*>& selected = selection->getAllSelected();
				for (CSelectObject* sel : selected)
				{
					if (sel->getType() == CSelectObject::GUIElement)
					{
						CGUIElement* selectGUI = m_gui->getCanvas()->getGUIByID(sel->getID().c_str());
						if (selectGUI != m_gui)
						{
							selectGUI->setVisible(m_objVisible);
							selection->notify(selectGUI, this);
						}
					}
				}
			}
		}

		void CGUIEditor::showEditorRect(bool b)
		{
			m_guiData->getProperty("rectX")->setHidden(!b);
			m_guiData->getProperty("rectY")->setHidden(!b);
			m_guiData->getProperty("rectW")->setHidden(!b);
			m_guiData->getProperty("rectH")->setHidden(!b);
		}

		void CGUIEditor::showEditorTransform(bool b)
		{
			m_guiData->getProperty("position")->setHidden(!b);
			m_guiData->getProperty("scale")->setHidden(!b);
			m_guiData->getProperty("rotation")->setHidden(!b);
		}

		void CGUIEditor::showEditorAlign(bool b)
		{
			m_guiData->getProperty("alignVertical")->setHidden(!b);
			m_guiData->getProperty("alignHorizontal")->setHidden(!b);
		}

		void CGUIEditor::showEditorMargin(EGUIDock dock)
		{
			CValueProperty* pLeft = m_guiData->getProperty("marginLeft");
			CValueProperty* pTop = m_guiData->getProperty("marginTop");
			CValueProperty* pRight = m_guiData->getProperty("marginRight");
			CValueProperty* pBottom = m_guiData->getProperty("marginBottom");

			CValueProperty* pMaterial = m_guiData->getProperty("material");
			pMaterial->setHidden(!m_gui->isEnableMaterial());

			CValueProperty* pMaterialId = m_guiData->getProperty("materialId");
			pMaterialId->setHidden(!m_gui->isEnableMaterial());

			switch (dock)
			{
			case EGUIDock::DockFill:
				pLeft->setHidden(false);
				pTop->setHidden(false);
				pRight->setHidden(false);
				pBottom->setHidden(false);
				break;
			case EGUIDock::DockLeft:
				pLeft->setHidden(false);
				pTop->setHidden(false);
				pRight->setHidden(true);
				pBottom->setHidden(false);
				break;
			case EGUIDock::DockRight:
				pLeft->setHidden(true);
				pTop->setHidden(false);
				pRight->setHidden(false);
				pBottom->setHidden(false);
				break;
			case EGUIDock::DockTop:
				pLeft->setHidden(false);
				pTop->setHidden(false);
				pRight->setHidden(false);
				pBottom->setHidden(true);
				break;
			case EGUIDock::DockBottom:
				pLeft->setHidden(false);
				pTop->setHidden(true);
				pRight->setHidden(false);
				pBottom->setHidden(false);
				break;
			default:
				// hide all
				pLeft->setHidden(true);
				pTop->setHidden(true);
				pRight->setHidden(true);
				pBottom->setHidden(true);
			}
		}
	}
}