/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CTreeFSController.h"
#include "CListFSController.h"
#include "CSearchAssetController.h"
#include "CSpaceAssets.h"

#include "Utils/CPath.h"
#include "Utils/CStringImp.h"

#include "Editor/Space/Hierarchy/CHierachyNode.h"
#include "Editor/SpaceController/CAssetCreateController.h"
#include "Editor/SpaceController/CSceneController.h"

#include "AssetManager/CAssetImporter.h"
#include "Scene/CSceneExporter.h"

namespace Skylicht
{
	namespace Editor
	{
		CTreeFSController::CTreeFSController(GUI::CCanvas* canvas, CSpaceAssets* space, GUI::CTreeControl* treeFS) :
			m_canvas(canvas),
			m_space(space),
			m_treeFS(treeFS),
			m_renameNode(NULL),
			m_nodeAssets(NULL),
			m_listController(NULL),
			m_searchController(NULL),
			m_msgBox(NULL)
		{
			m_assetManager = CAssetManager::getInstance();

			m_treeFS->OnKeyPress = std::bind(
				&CTreeFSController::OnKeyPress,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
			);

			m_treeFS->setMultiSelected(true);

			m_nodeAssets = m_treeFS->addNode(L"Assets", GUI::ESystemIcon::OpenFolder);
			m_nodeAssets->setIconColor(GUI::ThemeConfig::FolderColor);
			m_nodeAssets->tagString(m_assetManager->getAssetFolder());
			m_nodeAssets->OnExpand = BIND_LISTENER(&CTreeFSController::OnExpand, this);
			m_nodeAssets->OnCollapse = BIND_LISTENER(&CTreeFSController::OnCollapse, this);
			m_nodeAssets->OnSelectChange = BIND_LISTENER(&CTreeFSController::OnSelected, this);

			m_nodeAssets->setAlwayShowExpandButton(true);
			m_nodeAssets->expand(false);

			initDragDrop(m_nodeAssets);

			std::vector<SFileInfo> files;
			m_assetManager->getRoot(files);
			add(m_nodeAssets, files);
		}

		CTreeFSController::~CTreeFSController()
		{

		}

		void CTreeFSController::add(GUI::CTreeNode* node, std::vector<SFileInfo>& files)
		{
			for (const SFileInfo& f : files)
			{
				if (f.IsFolder == true)
				{
					GUI::CTreeNode* childNode = node->addNode(f.NameW.c_str(), GUI::ESystemIcon::Folder);
					childNode->setIconColor(GUI::ThemeConfig::FolderColor);
					childNode->tagString(f.FullPath);
					childNode->OnExpand = BIND_LISTENER(&CTreeFSController::OnExpand, this);
					childNode->OnCollapse = BIND_LISTENER(&CTreeFSController::OnCollapse, this);
					childNode->OnSelectChange = BIND_LISTENER(&CTreeFSController::OnSelected, this);

					initDragDrop(childNode);

					if (m_assetManager->isFolderEmpty(f.FullPath.c_str()) == false)
						childNode->setAlwayShowExpandButton(true);
				}
			}
		}

		void CTreeFSController::OnExpand(GUI::CBase* node)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(node);
			if (treeNode != NULL)
			{
				treeNode->setIcon(GUI::ESystemIcon::OpenFolder);

				const std::string& fullPath = treeNode->getTagString();

				std::vector<SFileInfo> files;
				m_assetManager->getFolder(fullPath.c_str(), files);

				add(treeNode, files);
			}
		}

		void CTreeFSController::OnCollapse(GUI::CBase* node)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(node);
			if (treeNode != NULL)
			{
				treeNode->setIcon(GUI::ESystemIcon::Folder);
				treeNode->removeAllTreeNode();
			}
		}

		void CTreeFSController::OnSelected(GUI::CBase* node)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(node);
			if (treeNode != NULL)
			{
				treeNode->setIcon(GUI::ESystemIcon::OpenFolder);

				const std::string& fullPath = treeNode->getTagString();

				std::vector<SFileInfo> files;
				m_assetManager->getFolder(fullPath.c_str(), files);

				if (m_searchController != NULL)
					m_searchController->hideSearchUI();

				CAssetCreateController::getInstance()->setActivateSpace(m_space);

				if (m_listController != NULL)
					m_listController->add(fullPath, files, true);
			}
		}

		GUI::CTreeNode* CTreeFSController::expand(const std::string& folder)
		{
			GUI::CTreeNode* node = NULL;
			std::string shortPath = m_assetManager->getShortPath(folder.c_str());
			if (shortPath.size() > 0)
			{
				std::vector<std::string> result;
				CStringImp::splitString(shortPath.c_str(), "/", result);

				node = m_nodeAssets;

				wchar_t wname[512];

				for (u32 i = 0, n = (u32)result.size(); i < n; i++)
				{
					const std::string name = result[i];
					CStringImp::convertUTF8ToUnicode(name.c_str(), wname);

					GUI::CTreeNode* child = node->getChildNodeByLabel(wname);
					if (child == NULL)
						break;

					if (child->haveChild())
						child->expand(false);
					else
						child->expand(true);

					node = child;
				}

				if (node != NULL)
				{
					m_nodeAssets->deselectAll(false);
					node->forceLayout();
					node->setSelected(true, false);
					m_treeFS->getScrollControl()->scrollToItem(node);
				}
			}
			return node;
		}

		void CTreeFSController::selectNode(GUI::CTreeNode* node)
		{
			OnSelected(node);

			// fix the listfs will reset because cancel search
			if (m_searchController != NULL)
				m_searchController->unchange();
		}

		void CTreeFSController::OnKeyPress(GUI::CBase* control, int key, bool press)
		{
			GUI::CTreeControl* tree = dynamic_cast<GUI::CTreeControl*>(control);
			if (tree == NULL)
				return;

			if (key == GUI::KEY_F2)
			{
				rename(tree->getChildSelected());
			}
		}

		void CTreeFSController::rename(GUI::CTreeNode* node)
		{
			if (node != NULL)
			{
				m_renameNode = node;
				m_renameRevert = node->getText();

				node->getTextEditHelper()->beginEdit(
					BIND_LISTENER(&CTreeFSController::OnRename, this),
					BIND_LISTENER(&CTreeFSController::OnCancelRename, this)
				);
			}
		}

		void CTreeFSController::removePath(const char* path)
		{
			removePath(m_treeFS, path);
		}

		bool CTreeFSController::removePath(GUI::CTreeNode* node, const char* path)
		{
			std::list<GUI::CTreeNode*> childNodes = node->getChildNodes();
			for (GUI::CTreeNode* node : childNodes)
			{
				const std::string& tagPath = node->getTagString();
				if (tagPath == path)
				{
					node->remove();
					return true;
				}

				if (node->isExpand())
				{
					if (removePath(node, path))
						return true;
				}
			}

			return false;
		}

		void CTreeFSController::OnRename(GUI::CBase* control)
		{
			GUI::CTextBox* textbox = dynamic_cast<GUI::CTextBox*>(control);

			std::wstring newNameW = textbox->getString();
			std::string newName = CStringImp::convertUnicodeToUTF8(newNameW.c_str());

			const std::string& path = m_renameNode->getTagString();
			std::string newPath = CPath::getFolderPath(path);
			newPath += "/";
			newPath += newName;

			if (m_assetManager->isExist(newPath.c_str()))
			{
				m_renameNode->setText(m_renameRevert);
				m_msgBox = new GUI::CMessageBox(m_canvas, GUI::CMessageBox::OK);
				m_msgBox->setMessage("File or folder with the new name already exists!", newName.c_str());
				m_msgBox->getMessageIcon()->setIcon(GUI::ESystemIcon::Alert);
				return;
			}

			if (m_assetManager->renameAsset(path.c_str(), newName.c_str()))
			{
				GUI::CTreeNode* parentNode = m_renameNode->getParentNode();
				m_renameNode = NULL;

				refresh(parentNode);
				expand(newPath);
			}

			m_treeFS->focus();
		}

		void CTreeFSController::OnCancelRename(GUI::CBase* control)
		{
			m_treeFS->focus();
		}

		void CTreeFSController::refresh(GUI::CTreeNode* node)
		{
			node->removeAllTreeNode();
			OnExpand(node);
		}

		void CTreeFSController::refresh()
		{
			std::list<std::string> listExpand;

			if (m_nodeAssets->isExpand() == false)
				return;

			GUI::CTreeNode* selectNode = m_treeFS->getChildSelected();
			std::string selectePath;
			if (selectNode != NULL)
				selectePath = selectNode->getTagString();

			std::list<GUI::CTreeNode*> stack = m_nodeAssets->getChildNodes();
			while (stack.size() > 0)
			{
				GUI::CTreeNode* top = stack.front();
				stack.pop_front();

				if (top->isExpand())
				{
					const std::string& path = top->getTagString();
					listExpand.push_back(path);

					std::list<GUI::CTreeNode*> child = top->getChildNodes();
					if (child.size() > 0)
						stack.insert(stack.begin(), child.begin(), child.end());
				}
			}

			// refresh root
			refresh(m_nodeAssets);

			// expand child
			for (const std::string& path : listExpand)
			{
				expand(path);
			}

			// focus the select
			if (!selectePath.empty())
			{
				expand(selectePath);
			}
		}

		void CTreeFSController::initDragDrop(GUI::CTreeNode* node)
		{
			std::string folderPath = node->getTagString();

			GUI::CTreeRowItem* row = node->getRowItem();
			row->OnAcceptDragDrop = [node](GUI::SDragDropPackage* data)
				{
					return false;
				};

			row->OnDrop = [node](GUI::SDragDropPackage* data, float mouseX, float mouseY)
				{

				};

			GUI::SDragDropPackage* dragDrop = row->setDragDropPackage("TreeFSItem", row);
			dragDrop->UserData = node;
			dragDrop->DrawControl = node->getTextItem();

			row->OnAcceptDragDrop = [node](GUI::SDragDropPackage* data)
				{
					// accept hierarchy node
					if (data->Name == "HierarchyNode")
					{
						CHierachyNode* dragNode = (CHierachyNode*)data->UserData;
						if (dragNode->getTagDataType() == CHierachyNode::Container ||
							dragNode->getTagDataType() == CHierachyNode::GameObject)
						{
							return true;
						}
					}
					return false;
				};
			row->OnDrop = [&, node](GUI::SDragDropPackage* data, float mouseX, float mouseY)
				{
					if (data->Name == "HierarchyNode")
					{
						CHierachyNode* dragNode = (CHierachyNode*)data->UserData;
						if (dragNode->getTagDataType() == CHierachyNode::Container ||
							dragNode->getTagDataType() == CHierachyNode::GameObject)
						{
							CGameObject* obj = (CGameObject*)dragNode->getTagData();

							const std::string folder = node->getTagString();
							CSceneController::getInstance()->onCreateTemplate(obj, folder.c_str());
						}
					}
				};
		}
	}
}