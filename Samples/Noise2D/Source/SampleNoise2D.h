#pragma once

#include "IApplicationEventReceiver.h"

class SampleNoise2D : public IApplicationEventReceiver
{
private:
	CScene* m_scene;
	CCamera* m_guiCamera;
	CCamera* m_camera;

	CMaterial* m_noiseMaterial;
	CMaterial* m_electricMaterial;
	CMaterial* m_electricLightningMaterial;
	CMaterial* m_burnMaterial;
	CMaterial* m_electricCircleMaterial;

	CGlyphFont* m_largeFont;

	CForwardRP* m_forwardRP;

	core::vector3df m_noiseOffset;
	core::vector3df m_explosionParam;

public:
	SampleNoise2D();
	virtual ~SampleNoise2D();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual void onResize(int w, int h);

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();
};