/*
!@
MIT License

CopyRight (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CHandlesRenderer.h"
#include "Handles/CHandles.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	namespace Editor
	{
		CHandlesRenderer::CHandlesRenderer() :
			m_screenFactor(1.0f),
			m_allowAxisFlip(true)
		{
			m_data = new CHandlesData();
			m_data->LineBuffer->getMaterial().ZBuffer = ECFN_DISABLED;
			m_data->LineBuffer->getMaterial().ZWriteEnable = false;

			m_data->PolygonBuffer->getMaterial().ZBuffer = ECFN_DISABLED;
			m_data->PolygonBuffer->getMaterial().ZWriteEnable = false;

			m_directionUnary[0] = core::vector3df(1.f, 0.f, 0.f);
			m_directionUnary[1] = core::vector3df(0.f, 1.f, 0.f);
			m_directionUnary[2] = core::vector3df(0.f, 0.f, 1.f);

			m_directionColor[0].set(0xFF0000AA);
			m_directionColor[1].set(0xFFAA0000);
			m_directionColor[2].set(0xFF00AA00);

			for (int i = 0; i < 3; i++)
			{
				m_axisFactor[i] = 1.0f;
				m_belowAxisLimit[i] = true;
				m_belowPlaneLimit[i] = true;
			}
		}

		CHandlesRenderer::~CHandlesRenderer()
		{
			delete m_data;
		}

		void CHandlesRenderer::beginQuery()
		{

		}

		void CHandlesRenderer::onQuery(CEntityManager* entityManager, CEntity* entity)
		{

		}

		void CHandlesRenderer::init(CEntityManager* entityManager)
		{

		}

		void CHandlesRenderer::update(CEntityManager* entityManager)
		{
			if (m_enable == false)
				return;

			((CLineDrawData*)m_data)->clearBuffer();
			((CPolygonDrawData*)m_data)->clearBuffer();

			// Caculate the screen scale factor
			irr::core::matrix4 invView;
			{
				irr::core::matrix4 view(getVideoDriver()->getTransform(video::ETS_VIEW));
				view.getInversePrimitive(invView);
			}

			core::vector3df cameraRight(invView[0], invView[1], invView[2]);
			core::vector3df cameraLook(invView[8], invView[9], invView[10]);
			core::vector3df cameraUp(invView[4], invView[5], invView[6]);

			cameraRight.normalize();
			cameraLook.normalize();
			cameraUp.normalize();

			m_screenFactor = 0.2f / getSegmentLengthClipSpace(core::vector3df(), cameraRight, entityManager->getCamera());

			// Draw position axis
			CHandles* handles = CHandles::getInstance();
			if (handles->isHandlePosition())
			{
				const core::vector3df& pos = handles->getHandlePosition();

				float quadMin = 0.1f;
				float quadMax = 0.4f;

				core::vector3df quad[4] = {
					core::vector3df(quadMin, quadMin, 0.0f),
					core::vector3df(quadMin, quadMax, 0.0f),
					core::vector3df(quadMax, quadMax, 0.0f),
					core::vector3df(quadMax, quadMin, 0.0f),
				};

				for (int i = 0; i < 3; i++)
				{
					core::vector3df dirAxis, dirPlaneX, dirPlaneY;
					bool belowAxisLimit, belowPlaneLimit;

					computeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, belowAxisLimit, belowPlaneLimit, entityManager->getCamera());

					if (belowAxisLimit)
					{
						// draw axis
						m_data->addLineVertexBatch(pos, pos + dirAxis * m_screenFactor, m_directionColor[i]);

						// draw arrow
						float arrowSize1 = m_screenFactor * 0.1f;
						float arrowSize2 = m_screenFactor * 0.05f;

						core::vector3df side = dirAxis.crossProduct(cameraLook);
						side.normalize();

						core::vector3df a = pos + dirAxis * m_screenFactor;
						core::vector3df m = a - dirAxis * arrowSize1;
						core::vector3df b = m + side * arrowSize2;
						core::vector3df c = m - side * arrowSize2;
						m_data->addTriangleFill(a, b, c, m_directionColor[i]);

						// draw quad
						/*
						float arrowSize1 = m_screenFactor * 0.1f;
						float arrowSize2 = m_screenFactor * 0.05f;

						core::vector3df a = pos + dirAxis * m_screenFactor;

						core::vector3df sideQuad = side * arrowSize2;
						core::vector3df upQuad = cameraUp * arrowSize2;

						core::vector3df v1, v2, v3, v4;
						v1.set(
							a.X - sideQuad.X + upQuad.X,
							a.Y - sideQuad.Y + upQuad.Y,
							a.Z - sideQuad.Z + upQuad.Z
						);
						v2.set(
							a.X + sideQuad.X + upQuad.X,
							a.Y + sideQuad.Y + upQuad.Y,
							a.Z + sideQuad.Z + upQuad.Z
						);
						v3.set(
							a.X + sideQuad.X - upQuad.X,
							a.Y + sideQuad.Y - upQuad.Y,
							a.Z + sideQuad.Z - upQuad.Z
						);
						v4.set(
							a.X - sideQuad.X - upQuad.X,
							a.Y - sideQuad.Y - upQuad.Y,
							a.Z - sideQuad.Z - upQuad.Z
						);
						core::vector3df arrow[] = { v1, v2, v3, v4 };
						m_data->addPolygonFill(arrow, 4, m_directionColor[i]);
						*/
					}

					if (belowPlaneLimit)
					{
						// draw plane
						core::vector3df planeLine[4];
						for (int j = 0; j < 4; j++)
							planeLine[j] = (dirPlaneX * quad[j].X + dirPlaneY * quad[j].Y) * m_screenFactor;

						m_data->addLineVertexBatch(planeLine[0], planeLine[1], m_directionColor[i]);
						m_data->addLineVertexBatch(planeLine[1], planeLine[2], m_directionColor[i]);
						m_data->addLineVertexBatch(planeLine[2], planeLine[3], m_directionColor[i]);
						m_data->addLineVertexBatch(planeLine[3], planeLine[0], m_directionColor[i]);

						SColor fillColor = m_directionColor[i];
						fillColor.setAlpha(50);
						m_data->addPolygonFill(planeLine, 4, fillColor);
					}
				}
			}

			((CLineDrawData*)m_data)->updateBuffer();
			((CPolygonDrawData*)m_data)->updateBuffer();
		}

		// References: https://github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp
		float CHandlesRenderer::getSegmentLengthClipSpace(const core::vector3df& start, const core::vector3df& end, CCamera* camera)
		{
			core::matrix4 trans = getVideoDriver()->getTransform(video::ETS_VIEW_PROJECTION);

			f32 start4[4] = { start.X, start.Y, start.Z, 1.0f };
			f32 end4[4] = { end.X, end.Y, end.Z, 1.0f };

			trans.multiplyWith1x4Matrix(start4);
			core::vector3df startOfSegment(start4[0], start4[1], start4[2]);
			if (fabsf(start4[3]) > FLT_EPSILON) // check for axis aligned with camera direction
			{
				startOfSegment *= 1.f / start4[3];
			}

			trans.multiplyWith1x4Matrix(end4);
			core::vector3df endOfSegment(end4[0], end4[1], end4[2]);
			if (fabsf(end4[3]) > FLT_EPSILON) // check for axis aligned with camera direction
			{
				endOfSegment *= 1.f / end4[3];
			}

			core::vector3df clipSpaceAxis = endOfSegment - startOfSegment;
			clipSpaceAxis.Y /= camera->getAspect();
			float segmentLengthInClipSpace = sqrtf(clipSpaceAxis.X * clipSpaceAxis.X + clipSpaceAxis.Y * clipSpaceAxis.Y);
			return segmentLengthInClipSpace;
		}

		// References: https://github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp
		float CHandlesRenderer::getParallelogram(const core::vector3df& ptO, const core::vector3df& ptA, const core::vector3df& ptB, CCamera* camera)
		{
			core::matrix4 trans = getVideoDriver()->getTransform(video::ETS_VIEW_PROJECTION);
			core::vector3df pts[] = { ptO, ptA, ptB };

			for (unsigned int i = 0; i < 3; i++)
			{
				f32 vec4[4] = { pts[i].X, pts[i].Y, pts[i].Z, 1.0f };
				trans.multiplyWith1x4Matrix(vec4);
				pts[i].set(vec4[0], vec4[1], vec4[2]);
				if (fabsf(vec4[3]) > FLT_EPSILON) // check for axis aligned with camera direction
				{
					pts[i] *= 1.f / vec4[3];
				}
			}

			core::vector3df segA = pts[1] - pts[0];
			core::vector3df segB = pts[2] - pts[0];
			segA.Y /= camera->getAspect();
			segB.Y /= camera->getAspect();
			core::vector3df segAOrtho = core::vector3df(-segA.Y, segA.X, 0.0f);
			segAOrtho.normalize();
			float dt = segAOrtho.dotProduct(segB);
			float surface = sqrtf(segA.X * segA.X + segA.Y * segA.Y) * fabsf(dt);
			return surface;
		}

		// References: https://github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp
		void CHandlesRenderer::computeTripodAxisAndVisibility(int axisIndex, core::vector3df& dirAxis, core::vector3df& dirPlaneX, core::vector3df& dirPlaneY, bool& belowAxisLimit, bool& belowPlaneLimit, CCamera* camera)
		{
			dirAxis = m_directionUnary[axisIndex];
			dirPlaneX = m_directionUnary[(axisIndex + 1) % 3];
			dirPlaneY = m_directionUnary[(axisIndex + 2) % 3];

			float lenDir = getSegmentLengthClipSpace(core::vector3df(0.f, 0.f, 0.f), dirAxis, camera);
			float lenDirMinus = getSegmentLengthClipSpace(core::vector3df(0.f, 0.f, 0.f), -dirAxis, camera);

			float lenDirPlaneX = getSegmentLengthClipSpace(core::vector3df(0.f, 0.f, 0.f), dirPlaneX, camera);
			float lenDirMinusPlaneX = getSegmentLengthClipSpace(core::vector3df(0.f, 0.f, 0.f), -dirPlaneX, camera);

			float lenDirPlaneY = getSegmentLengthClipSpace(core::vector3df(0.f, 0.f, 0.f), dirPlaneY, camera);
			float lenDirMinusPlaneY = getSegmentLengthClipSpace(core::vector3df(0.f, 0.f, 0.f), -dirPlaneY, camera);

			// For readability
			bool& allowFlip = m_allowAxisFlip;
			float mulAxis = (allowFlip && lenDir < lenDirMinus&& fabsf(lenDir - lenDirMinus) > FLT_EPSILON) ? -1.f : 1.f;
			float mulAxisX = (allowFlip && lenDirPlaneX < lenDirMinusPlaneX&& fabsf(lenDirPlaneX - lenDirMinusPlaneX) > FLT_EPSILON) ? -1.f : 1.f;
			float mulAxisY = (allowFlip && lenDirPlaneY < lenDirMinusPlaneY&& fabsf(lenDirPlaneY - lenDirMinusPlaneY) > FLT_EPSILON) ? -1.f : 1.f;
			dirAxis *= mulAxis;
			dirPlaneX *= mulAxisX;
			dirPlaneY *= mulAxisY;

			// for axis
			float axisLengthInClipSpace = getSegmentLengthClipSpace(core::vector3df(0.f, 0.f, 0.f), dirAxis * m_screenFactor, camera);

			float paraSurf = getParallelogram(core::vector3df(0.f, 0.f, 0.f), dirPlaneX * m_screenFactor, dirPlaneY * m_screenFactor, camera);
			belowPlaneLimit = (paraSurf > 0.02f);
			belowAxisLimit = (axisLengthInClipSpace > 0.08f);

			// and store values
			m_axisFactor[axisIndex] = mulAxis;
			m_axisFactor[(axisIndex + 1) % 3] = mulAxisX;
			m_axisFactor[(axisIndex + 2) % 3] = mulAxisY;
			m_belowAxisLimit[axisIndex] = belowAxisLimit;
			m_belowPlaneLimit[axisIndex] = belowPlaneLimit;
		}

		void CHandlesRenderer::render(CEntityManager* entityManager)
		{

		}

		void CHandlesRenderer::postRender(CEntityManager* entityManager)
		{
			if (m_enable == false)
				return;

			IVideoDriver* driver = getVideoDriver();
			driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			IMeshBuffer* buffer = NULL;

			buffer = m_data->PolygonBuffer;
			driver->setMaterial(buffer->getMaterial());
			driver->drawMeshBuffer(buffer);

			buffer = m_data->LineBuffer;
			driver->setMaterial(buffer->getMaterial());
			driver->drawMeshBuffer(buffer);
		}
	}
}