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
#include "CGUIEditorHistory.h"
#include "Graphics2D/CGUIImporter.h"
#include "Editor/SpaceController/CGUIDesignController.h"
#include "Selection/CSelection.h"

namespace Skylicht
{
	namespace Editor
	{
		CGUIEditorHistory::CGUIEditorHistory(CCanvas* canvas) :
			m_canvas(canvas)
		{

		}

		CGUIEditorHistory::~CGUIEditorHistory()
		{
			freeCurrentObjectData();
		}

		void CGUIEditorHistory::doDelete(SHistoryData* historyData)
		{
			CGUIDesignController* controller = CGUIDesignController::getInstance();
			CCanvas* canvas = controller->getCanvas();

			size_t numObject = historyData->ObjectID.size();
			for (size_t i = 0; i < numObject; i++)
			{
				// object id
				std::string& id = historyData->ObjectID[i];

				// remove this object
				CGUIElement* element = canvas->getGUIByID(id.c_str());
				if (element)
					controller->removeGUIElement(element);
			}
		}

		void CGUIEditorHistory::doCreate(SHistoryData* historyData)
		{
			CGUIDesignController* controller = CGUIDesignController::getInstance();
			CGUIHierarchyController* hierarchyController = NULL;
			CCanvas* canvas = controller->getCanvas();

			if (controller->getSpaceHierarchy() != NULL)
				hierarchyController = controller->getSpaceHierarchy()->getController();

			size_t numObject = historyData->ObjectID.size();
			for (size_t i = 0; i < numObject; i++)
			{
				std::string& parentId = historyData->Container[i];
				if (parentId != "_")
				{
					CGUIElement* parent = canvas->getGUIByID(parentId.c_str());
					if (parent)
					{
						CGUIElement* ui = CGUIImporter::importGUI(canvas, parent, historyData->Data[i]);

						// update on GUI
						if (ui && hierarchyController)
						{
							CGUIHierachyNode* parentNode = hierarchyController->getNodeByObject(parent);
							if (parentNode)
								controller->createGUINode(parentNode, ui);
						}
					}
				}
			}
		}

		void CGUIEditorHistory::doModify(SHistoryData* historyData, bool undo)
		{
			CGUIDesignController* controller = CGUIDesignController::getInstance();
			CGUIHierarchyController* hierarchyController = NULL;
			CCanvas* canvas = controller->getCanvas();

			size_t numObject = historyData->DataModified.size();
			for (size_t i = 0; i < numObject; i++)
			{
				// object id
				std::string& id = historyData->ObjectID[i];

				// revert to old data
				CObjectSerializable* data = undo ? historyData->Data[i] : historyData->DataModified[i];

				// get object and undo data
				CGUIElement* element = canvas->getGUIByID(id.c_str());
				if (element != NULL)
					element->loadSerializable(data);

				controller->onHistoryModifyObject(element);

				// set current data for next action
				SGUIObjectHistory* objHistory = getObjectHistory(id);
				if (objHistory != NULL)
					objHistory->changeData(data);
			}
		}

		void CGUIEditorHistory::doStructure(SHistoryData* historyData, bool undo)
		{
			CGUIDesignController* controller = CGUIDesignController::getInstance();
			CCanvas* canvas = controller->getCanvas();

			size_t numObject = historyData->ObjectID.size();
			for (size_t i = 0; i < numObject; i++)
			{
				// object id
				std::string& id = historyData->ObjectID[i];

				std::string container = undo ? historyData->Container[i] : historyData->MoveData[i].TargetContainer;
				std::string before = undo ? historyData->BeforeID[i] : historyData->MoveData[i].To;

				// get object and move structure
				CGUIElement* guiObject = canvas->getGUIByID(id.c_str());
				CGUIElement* beforeObject = canvas->getGUIByID(before.c_str());

				if (guiObject)
				{
					CGUIElement* parentObject = canvas->getGUIByID(container.c_str());
					if (parentObject)
					{
						controller->onMoveStructure(guiObject, parentObject, beforeObject);
					}
					else
					{
						os::Printer::log("[CGUIEditorHistory::doStructure] failed - null parent");
					}

					// set current data for next action
					SGUIObjectHistory* objHistory = getObjectHistory(id);
					if (objHistory != NULL)
					{
						delete objHistory->ObjectData;
						saveHistory(objHistory, guiObject);
					}
				}
				else
				{
					os::Printer::log("[CGUIEditorHistory::doStructure] failed - null gui");
				}
			}
		}

		void CGUIEditorHistory::undo()
		{
			int historySize = (int)m_history.size();
			if (historySize == 0)
				return;

			m_enable = false;

			CGUIDesignController* controller = CGUIDesignController::getInstance();

			// last history save
			SHistoryData* historyData = m_history[historySize - 1];

			switch (historyData->History)
			{
			case EHistory::Create:
			{
				doDelete(historyData);
			}
			break;
			case EHistory::Modify:
			{
				doModify(historyData, true);
			}
			break;
			case EHistory::Delete:
			{
				doCreate(historyData);
			}
			break;
			case EHistory::Structure:
			{
				doStructure(historyData, true);
			}
			break;
			default:
				break;
			}

			// apply history selection
			controller->deselectAllOnHierachy(false);
			if (historySize - 2 >= 0)
			{
				controller->applySelected(m_history[historySize - 2]->Selected);
			}

			// move history to redo action
			m_redo.push_back(historyData);
			m_history.erase(m_history.begin() + (historySize - 1));

			// refresh ui on editor
			CEditor::getInstance()->refresh();

			m_enable = true;
		}

		void CGUIEditorHistory::redo()
		{
			size_t historySize = m_redo.size();
			if (historySize == 0)
				return;

			m_enable = false;

			CGUIDesignController* controller = CGUIDesignController::getInstance();

			// last history save
			SHistoryData* historyData = m_redo[historySize - 1];

			switch (historyData->History)
			{
			case EHistory::Create:
			{
				doCreate(historyData);
			}
			break;
			case EHistory::Modify:
			{
				doModify(historyData, false);
			}
			break;
			case EHistory::Delete:
			{
				doDelete(historyData);
			}
			break;
			case EHistory::Structure:
			{
				doStructure(historyData, false);
			}
			break;
			default:
				break;
			}

			// apply history selection
			controller->deselectAllOnHierachy(false);
			controller->applySelected(historyData->Selected);

			// move history to redo action
			m_history.push_back(historyData);
			m_redo.erase(m_redo.begin() + (historySize - 1));

			// refresh ui on editor
			CEditor::getInstance()->refresh();

			m_enable = true;
		}

		void CGUIEditorHistory::freeCurrentObjectData()
		{
			for (SGUIObjectHistory* history : m_objects)
			{
				delete history->ObjectData;
				delete history;
			}
			m_objects.clear();
		}

		SGUIObjectHistory* CGUIEditorHistory::getObjectHistory(const std::string& id)
		{
			for (SGUIObjectHistory* history : m_objects)
			{
				if (history->ObjectID == id)
					return history;
			}
			return NULL;
		}

		void CGUIEditorHistory::saveHistory(SGUIObjectHistory* historyData, CGUIElement* guiObject)
		{
			historyData->ObjectID = guiObject->getID();
			historyData->ObjectData = guiObject->createSerializable();

			CGUIElement* parent = guiObject->getParent();
			if (parent)
			{
				historyData->ContainerID = parent->getID();

				CGUIElement* gui = parent->getChildBefore(guiObject);
				if (gui)
					historyData->BeforeID = gui->getID();
				else
					historyData->BeforeID = "";
			}
		}

		void CGUIEditorHistory::beginSaveHistory(CGUIElement* guiObject)
		{
			if (!m_enable || !m_enableSelectHistory)
				return;

			const std::string& objectID = guiObject->getID();

			for (SGUIObjectHistory* history : m_objects)
			{
				if (history->ObjectID == objectID)
				{
					delete history->ObjectData;
					saveHistory(history, guiObject);
					return;
				}
			}

			SGUIObjectHistory* objectData = new SGUIObjectHistory();
			saveHistory(objectData, guiObject);
			m_objects.push_back(objectData);
		}

		void CGUIEditorHistory::removeSaveHistory(CGUIElement* guiObject)
		{
			if (!m_enable || !m_enableSelectHistory)
				return;

			std::string objectID = guiObject->getID();

			std::vector<SGUIObjectHistory*>::iterator i = m_objects.begin(), end = m_objects.end();
			while (i != end)
			{
				SGUIObjectHistory* history = (*i);
				if (history->ObjectID == objectID)
				{
					delete history->ObjectData;
					delete history;
					m_objects.erase(i);
					return;
				}
				++i;
			}
		}

		void CGUIEditorHistory::addData(CGUIElement* guiObject,
			std::vector<std::string>& container,
			std::vector<std::string>& id,
			std::vector<std::string>& before,
			std::vector<CObjectSerializable*>& modifyData,
			std::vector<CObjectSerializable*>& objectData)
		{
			CObjectSerializable* currentData = guiObject->createSerializable();

			// parent container id
			CGUIElement* parent = guiObject->getParent();
			if (parent != NULL)
			{
				container.push_back(parent->getID());

				CGUIElement* gui = parent->getChildBefore(guiObject);
				if (gui)
					before.push_back(gui->getID());
				else
					before.push_back("");
			}
			else
			{
				// this is zone (no parent)
				container.push_back("_");
				before.push_back("");
			}

			// game object id
			id.push_back(guiObject->getID());

			// last data object
			objectData.push_back(currentData);

			// current data object
			modifyData.push_back(NULL);
		}

		void CGUIEditorHistory::saveCreateHistory(CGUIElement* guiObject)
		{
			if (!m_enable)
				return;

			std::vector<CGUIElement*> guiObjects;
			guiObjects.push_back(guiObject);
			saveCreateHistory(guiObjects);
		}

		void CGUIEditorHistory::saveCreateHistory(std::vector<CGUIElement*> guiObjects)
		{
			if (!m_enable)
				return;

			if (guiObjects.size() == 0)
				return;

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<std::string> before;
			std::vector<CObjectSerializable*> modifyData;
			std::vector<CObjectSerializable*> objectData;

			for (CGUIElement* guiObject : guiObjects)
			{
				addData(guiObject, container, id, before, modifyData, objectData);
			}

			// save history
			addHistory(EHistory::Create, container, id, before, getSelected(), modifyData, objectData);
		}

		void CGUIEditorHistory::saveDeleteHistory(CGUIElement* guiObject)
		{
			if (!m_enable)
				return;

			std::vector<CGUIElement*> guiObjects;

			guiObjects.push_back(guiObject);
			guiObject->getAllChilds(guiObjects);

			saveDeleteHistory(guiObjects);
		}

		void CGUIEditorHistory::saveDeleteHistory(std::vector<CGUIElement*> guiObjects)
		{
			if (!m_enable)
				return;

			if (guiObjects.size() == 0)
				return;

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<std::string> before;
			std::vector<CObjectSerializable*> modifyData;
			std::vector<CObjectSerializable*> objectData;

			for (CGUIElement* guiObject : guiObjects)
			{
				addData(guiObject, container, id, before, modifyData, objectData);
			}

			// save history
			addHistory(EHistory::Delete, container, id, before, getSelected(), modifyData, objectData);
		}

		bool CGUIEditorHistory::saveModifyHistory(CGUIElement* guiObject)
		{
			if (!m_enable)
				return false;

			std::vector<CGUIElement*> guiObjects;
			guiObjects.push_back(guiObject);
			return saveModifyHistory(guiObjects);
		}

		bool CGUIEditorHistory::saveModifyHistory(std::vector<CGUIElement*> guiObjects)
		{
			if (!m_enable)
				return false;

			if (guiObjects.size() == 0)
				return false;

			bool success = true;

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<std::string> before;
			std::vector<CObjectSerializable*> modifyData;
			std::vector<CObjectSerializable*> objectData;

			for (CGUIElement* guiObject : guiObjects)
			{
				SGUIObjectHistory* historyData = getObjectHistory(guiObject->getID());
				if (historyData == NULL)
				{
					os::Printer::log("[CGUIEditorHistory::saveHistory] failed, call CGUIEditorHistory::beginSaveHistory first!");
					success = false;
					break;
				}

				CObjectSerializable* currentData = guiObject->createSerializable();

				id.push_back(guiObject->getID());
				container.push_back(historyData->ContainerID);
				before.push_back(historyData->BeforeID);
				objectData.push_back(historyData->ObjectData->clone());
				modifyData.push_back(currentData);
				historyData->changeData(currentData);
			}

			if (success)
			{
				addHistory(EHistory::Modify, container, id, before, getSelected(), modifyData, objectData);
			}
			else
			{
				for (CObjectSerializable* objData : modifyData)
					delete objData;
			}

			return success;
		}

		void CGUIEditorHistory::saveStructureHistory(std::vector<CGUIElement*> guiObjects)
		{
			if (!m_enable)
				return;

			if (guiObjects.size() == 0)
				return;

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<std::string> before;
			std::vector<SMoveCommand> moveCmd;

			for (CGUIElement* guiObject : guiObjects)
			{
				SGUIObjectHistory* historyData = getObjectHistory(guiObject->getID());
				if (historyData == NULL)
				{
					os::Printer::log("[CGUIEditorHistory::saveStructureHistory] failed, call CGUIEditorHistory::beginSaveHistory first!");
					break;
				}

				// old location
				id.push_back(guiObject->getID());
				container.push_back(historyData->ContainerID);
				before.push_back(historyData->BeforeID);

				// update new location
				delete historyData->ObjectData;
				saveHistory(historyData, guiObject);

				SMoveCommand moveCmdData;
				moveCmdData.TargetContainer = historyData->ContainerID;
				moveCmdData.To = historyData->BeforeID;
				moveCmd.push_back(moveCmdData);
			}

			addStrucureHistory(container, id, before, getSelected(), moveCmd);
		}

		void CGUIEditorHistory::endSaveHistory()
		{
			freeCurrentObjectData();
		}
	}
}