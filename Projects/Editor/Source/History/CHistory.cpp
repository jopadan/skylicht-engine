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
#include "CHistory.h"
#include "Selection/CSelection.h"

namespace Skylicht
{
	namespace Editor
	{
		CHistory::CHistory() :
			m_enable(true),
			m_enableSelectHistory(true)
		{

		}

		CHistory::~CHistory()
		{
			clearHistory();
			clearRedo();
		}

		void CHistory::clearHistory()
		{
			for (SHistoryData* history : m_history)
			{
				for (CSelectObject* obj : history->Selected)
					delete obj;
				for (CObjectSerializable* data : history->Data)
					delete data;
				for (CObjectSerializable* data : history->DataModified)
					delete data;
				delete history;
			}
			m_history.clear();
		}

		void CHistory::clearRedo()
		{
			for (SHistoryData* history : m_redo)
			{
				for (CSelectObject* obj : history->Selected)
					delete obj;
				for (CObjectSerializable* data : history->Data)
					delete data;
				for (CObjectSerializable* data : history->DataModified)
					delete data;
				delete history;
			}
			m_redo.clear();
		}

		void CHistory::addHistory(EHistory history,
			const std::vector<std::string>& container,
			const std::vector<std::string>& id,
			const std::vector<std::string>& before,
			const std::vector<CSelectObject*>& selected,
			const std::vector<CObjectSerializable*>& dataModified,
			const std::vector<CObjectSerializable*>& data)
		{
			if (!m_enable)
				return;

			SHistoryData* historyData = new SHistoryData();
			historyData->History = history;
			historyData->Container = container;
			historyData->ObjectID = id;
			historyData->Selected = selected;
			historyData->BeforeID = before;
			historyData->DataModified = dataModified;
			historyData->Data = data;
			m_history.push_back(historyData);

			clearRedo();
		}

		void CHistory::addStrucureHistory(const std::vector<std::string>& container,
			const std::vector<std::string>& id,
			const std::vector<std::string>& before,
			const std::vector<CSelectObject*>& selected,
			const std::vector<SMoveCommand> moveCmd)
		{
			if (!m_enable)
				return;

			SHistoryData* historyData = new SHistoryData();
			historyData->History = Structure;
			historyData->Container = container;
			historyData->ObjectID = id;
			historyData->Selected = selected;
			historyData->BeforeID = before;
			historyData->MoveData = moveCmd;

			m_history.push_back(historyData);

			clearRedo();
		}

		void CHistory::addSelectHistory()
		{
			if (!m_enable || !m_enableSelectHistory)
				return;

			SHistoryData* historyData = new SHistoryData();
			historyData->History = Selected;
			historyData->Selected = getSelected();

			if (m_history.size() > 0)
			{
				// no need to save the same objects selected
				SHistoryData* back = m_history.back();
				if (back->History == Selected)
				{
					if (back->Selected.size() == historyData->Selected.size())
					{
						bool same = true;
						for (u32 i = 0, n = (u32)historyData->Selected.size(); i < n; i++)
						{
							if (back->Selected[i]->getID() != historyData->Selected[i]->getID())
							{
								same = false;
								break;
							}
						}

						if (same)
						{
							delete historyData;
							return;
						}
					}
				}
			}

			m_history.push_back(historyData);

			clearRedo();
		}

		std::vector<CSelectObject*> CHistory::getSelected()
		{
			std::vector<CSelectObject*> result;

			std::vector<CSelectObject*>& currentSelection = CSelection::getInstance()->getAllSelected();
			for (CSelectObject* obj : currentSelection)
			{
				result.push_back(new CSelectObject(obj->getType(), obj->getID()));
			}

			return result;
		}
	}
}