#pragma once

#include "SkylichtEngine.h"
#include "ViewManager/CView.h"
#include "Emscripten/CGetFileURL.h"

class CViewInit : public CView
{
public:
	enum EInitState
	{
		DownloadBundles,
		InitScene,
		Error,
		Finished
	};

protected:
	CGetFileURL* m_getFile;

	bool m_bakeSHLighting;

	EInitState m_initState;
	unsigned int m_downloaded;

	CGameObject* m_helmet;
	std::vector<CGameObject*> m_spheres;

	CGameObject* m_guiObject;
	CGUIText* m_textInfo;
	CGlyphFont* m_font;
	CGlyphFont* m_largeFont;

protected:
	io::path getBuiltInPath(const char* name);

public:
	CViewInit();

	virtual ~CViewInit();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

protected:

	void initScene();

	void createCanvasText(CZone* zone, const char* text, const core::vector3df& position);
};
