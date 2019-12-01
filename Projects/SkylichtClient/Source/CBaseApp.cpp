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
#include "CBaseApp.h"

#include "Graphics/CGraphics.h"

#ifdef _DEBUG
#ifdef USE_VISUAL_LEAK_DETECTOR
#include "vld.h"
#pragma comment(lib,"vld.lib")
#endif
#endif

namespace Skylicht
{
	CBaseApp::CBaseApp()
	{
		m_device = NULL;
		m_driver = NULL;

		m_timeStep = 1.0f;
		m_limitFPS = -1;

		m_clearColor.set(255, 0, 0, 0);

#ifdef USE_VISUAL_LEAK_DETECTOR
		VLDEnable();
#endif

		m_clearScreenTime = 0.0f;
		m_enableRender = true;

#if defined(_DEBUG) && defined(_WIN32)
#if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
		AllocConsole();
		freopen("con", "w", stdout);
		freopen("con", "w", stderr);
#endif
#endif		
	}

	CBaseApp::~CBaseApp()
	{
		m_eventReceivers.clear();

#ifdef USE_VISUAL_LEAK_DETECTOR
		VLDDisable();
#endif
	}

	void CBaseApp::registerEvent(std::string name, IEventReceiver *pEvent)
	{
		std::vector<eventType>::iterator i = m_eventReceivers.begin(), end = m_eventReceivers.end();
		while (i != end)
		{
			if ((*i).second == pEvent)
				return;
			i++;
		}

		m_eventReceivers.push_back(eventType(name, pEvent));
	}

	void CBaseApp::unRegisterEvent(IEventReceiver *pEvent)
	{
		std::vector<eventType>::iterator i = m_eventReceivers.begin(), end = m_eventReceivers.end();
		while (i != end)
		{
			if ((*i).second == pEvent)
			{
				m_eventReceivers.erase(i);
				return;
			}
			i++;
		}
	}

	void CBaseApp::registerAppEvent(std::string name, IApplicationEventReceiver *pEvent)
	{
		std::vector<appEventType>::iterator i = m_appEventReceivers.begin(), end = m_appEventReceivers.end();
		while (i != end)
		{
			if ((*i).second == pEvent)
				return;
			i++;
		}

		m_appEventReceivers.push_back(appEventType(name, pEvent));
	}

	void CBaseApp::unRegisterAppEvent(IApplicationEventReceiver *pEvent)
	{
		std::vector<appEventType>::iterator i = m_appEventReceivers.begin(), end = m_appEventReceivers.end();
		while (i != end)
		{
			if ((*i).second == pEvent)
			{
				m_appEventReceivers.erase(i);
				return;
			}
			i++;
		}
	}

	int CBaseApp::getWidth()
	{
		return (int)m_driver->getScreenSize().Width;
	}

	int CBaseApp::getHeight()
	{
		return (int)m_driver->getScreenSize().Height;
	}

	float CBaseApp::getSizeRatio()
	{
		core::dimension2d<u32> size = m_driver->getScreenSize();

		float ratio = (float)size.Width / (float)size.Height;
		return ratio;
	}

	bool CBaseApp::isHD()
	{
#ifdef LINUX_SERVER	
		return false;
#else
		return CGraphics::getInstance()->isHD();
#endif
	}

	bool CBaseApp::isWideScreen()
	{
#ifndef LINUX_SERVER
		return CGraphics::getInstance()->isWideScreen();
#else
		return false;
#endif
	}

	void CBaseApp::autoScaleUI()
	{
#ifndef LINUX_SERVER
		core::dimension2du screenSize = getApplication()->getDriver()->getScreenSize();
		core::dimension2du baseSize(1920, 1080);

		float screenRatio = screenSize.Width / (float)screenSize.Height;
		if (screenRatio < 1.5f)
			baseSize.set(1920, 1440);

		float scale = baseSize.Width / (float)screenSize.Width;

		CGraphics::getInstance()->setScale(scale);
#endif
	}

	void CBaseApp::alertError(wchar_t *lpString)
	{
		// implement on Editor
	}

	bool CBaseApp::yesNoQuestion(wchar_t *lpString)
	{
		// implement on Editor
		return false;
	}

	void CBaseApp::setStatusText(int part, wchar_t *lpString)
	{
		// implement on Editor
	}

}