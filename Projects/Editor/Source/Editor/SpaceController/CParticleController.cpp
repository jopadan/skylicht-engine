/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CParticleController.h"
#include "CPropertyController.h"
#include "CSceneController.h"

#include "Editor/Space/Particle/CSpaceParticle.h"

#include "ParticleSystem/CParticleComponent.h"


namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CParticleController);

		CParticleController::CParticleController() :
			m_particle(NULL),
			m_hierachyNode(NULL),
			m_contextNode(NULL),
			m_canvas(NULL),
			m_contextMenuParticle(NULL),
			m_spaceParticle(NULL)
		{

		}

		CParticleController::~CParticleController()
		{

		}

		void CParticleController::setSpaceParticle(CSpaceParticle* space)
		{
			m_spaceParticle = space;
		}

		void CParticleController::closeSpaceParticle(CSpaceParticle* space)
		{
			if (m_spaceParticle == space)
			{
				m_spaceParticle = NULL;
				m_hierachyNode = NULL;
				m_contextNode = NULL;
				m_particle = NULL;
			}
		}

		void CParticleController::initContextMenu(GUI::CCanvas* canvas)
		{
			m_canvas = canvas;
			m_contextMenuParticle = new CContextMenuParticle(canvas);
		}

		void CParticleController::setGameObject(CGameObject* obj)
		{
			if (m_spaceParticle == NULL)
				return;

			if (obj == NULL)
			{
				m_spaceParticle->setNull();
				return;
			}

			Particle::CParticleComponent* particle = obj->getComponent<Particle::CParticleComponent>();
			if (particle)
				m_spaceParticle->setParticle(particle);
			else
				m_spaceParticle->setNull();
		}

		CParticleHierachyNode* CParticleController::buildNode(Particle::CParticleComponent* particle)
		{
			if (m_hierachyNode)
				delete m_hierachyNode;

			m_hierachyNode = new CParticleHierachyNode(NULL);
			m_hierachyNode->setName(L"Particle System");
			m_hierachyNode->setIcon(GUI::ESystemIcon::ParticleSystem);
			m_hierachyNode->setTagData(particle, CParticleHierachyNode::Particle);

			m_particle = particle;

			setNodeEvent(m_hierachyNode);

			buildHierarchyData(particle);

			return m_hierachyNode;
		}

		void CParticleController::buildHierarchyData(Particle::CParticleComponent* particle)
		{
			for (u32 i = 0, n = particle->getNumOfGroup(); i < n; i++)
			{
				Particle::CGroup* group = particle->getGroup(i);
				buildGroupHierachy(group);
			}
		}

		CParticleHierachyNode* CParticleController::buildGroupHierachy(Particle::CGroup* group)
		{
			if (!m_hierachyNode)
				return NULL;

			CParticleHierachyNode* node = m_hierachyNode->addChild();
			node->setTagData(group, CParticleHierachyNode::Group);
			node->setParentData(group);
			node->setIcon(GUI::ESystemIcon::Folder);
			node->setName(group->Name.c_str());
			setNodeEvent(node);

			updateGroupHierachy(node, group);

			return node;
		}

		CParticleHierachyNode* CParticleController::getNodeByTagData(void* data)
		{
			if (!m_hierachyNode)
				return NULL;

			return m_hierachyNode->getNodeByTag(data);
		}

		void CParticleController::removeGroup(Particle::CGroup* group)
		{
			if (!m_hierachyNode)
				return;

			std::vector<CParticleHierachyNode*>& childs = m_hierachyNode->getChilds();
			for (CParticleHierachyNode* child : childs)
			{
				if (child->getTagData() == group)
				{
					child->remove();
					return;
				}
			}
		}

		void CParticleController::updateGroupHierachy(Particle::CGroup* group)
		{
			if (!m_hierachyNode)
				return;

			CParticleHierachyNode* node = NULL;
			std::vector<CParticleHierachyNode*> childs = m_hierachyNode->getChilds();
			for (CParticleHierachyNode* child : childs)
			{
				if (child->getTagData() == group)
				{
					node = child;
					break;
				}
			}

			if (node == NULL)
			{
				// try to update from parent group
				Particle::CSubGroup* subGroup = dynamic_cast<Particle::CSubGroup*>(group);
				if (subGroup)
					updateGroupHierachy(subGroup->getParentGroup());
				return;
			}

			childs = node->getChilds();
			for (CParticleHierachyNode* child : childs)
			{
				child->remove();
			}

			updateGroupHierachy(node, group);

			if (m_spaceParticle)
				m_spaceParticle->updateTreeNode(node);
		}

		void CParticleController::updateGroupName()
		{
			if (!m_hierachyNode)
				return;

			updateGroupName(m_hierachyNode->getChilds());
		}

		void CParticleController::updateGroupName(std::vector<CParticleHierachyNode*>& nodes)
		{
			for (CParticleHierachyNode* child : nodes)
			{
				if (child->getTagDataType() == CParticleHierachyNode::Group)
				{
					Particle::CGroup* group = (Particle::CGroup*)child->getTagData();
					child->setName(group->Name.c_str());
				}

				std::vector<CParticleHierachyNode*>& childs = child->getChilds();
				if (childs.size() > 0)
					updateGroupName(childs);
			}
		}

		void CParticleController::updateGroupHierachy(CParticleHierachyNode* node, Particle::CGroup* group)
		{
			CParticleHierachyNode* nodeEmitters = node->addChild();
			nodeEmitters->setTagData(NULL, CParticleHierachyNode::Emitters);
			nodeEmitters->setParentData(group);
			nodeEmitters->setIcon(GUI::ESystemIcon::Folder);
			nodeEmitters->setName(L"Emitters");
			setNodeEvent(nodeEmitters);

			std::vector<Particle::CEmitter*>& emitters = group->getEmitters();
			for (Particle::CEmitter* emitter : emitters)
			{
				CParticleHierachyNode* nodeEmitter = nodeEmitters->addChild();
				nodeEmitter->setTagData(emitter, CParticleHierachyNode::Emitter);
				nodeEmitter->setParentData(group);
				nodeEmitter->setIcon(GUI::ESystemIcon::ParticleEmitter);
				nodeEmitter->setName(emitter->getName());
				setNodeEvent(nodeEmitter);

				Particle::CZone* zone = emitter->getZone();
				CParticleHierachyNode* nodeZone = nodeEmitter->addChild();
				nodeZone->setTagData(zone, CParticleHierachyNode::Zone);
				nodeZone->setParentData(group);

				Particle::EZone zoneType = zone->getType();
				switch (zoneType)
				{
				case Particle::EZone::Point:
					nodeZone->setIcon(GUI::ESystemIcon::ObjectPoint);
					break;
				case Particle::EZone::Sphere:
					nodeZone->setIcon(GUI::ESystemIcon::ObjectSphere);
					break;
				case Particle::EZone::AABox:
					nodeZone->setIcon(GUI::ESystemIcon::ObjectBox);
					break;
				case Particle::EZone::Cylinder:
					nodeZone->setIcon(GUI::ESystemIcon::ObjectCylinder);
					break;
				case Particle::EZone::Line:
					nodeZone->setIcon(GUI::ESystemIcon::ObjectLine);
					break;
				case Particle::EZone::PolyLine:
					nodeZone->setIcon(GUI::ESystemIcon::ObjectPolyline);
					break;
				case Particle::EZone::Ring:
					nodeZone->setIcon(GUI::ESystemIcon::ObjectCircle);
					break;
				default:
					break;
				}

				nodeZone->setName(zone->getName());
				setNodeEvent(nodeZone);
			}

			CParticleHierachyNode* nodeModels = node->addChild();
			nodeModels->setTagData(NULL, CParticleHierachyNode::Models);
			nodeModels->setParentData(group);
			nodeModels->setIcon(GUI::ESystemIcon::Folder);
			nodeModels->setName(L"Models");
			setNodeEvent(nodeModels);

			std::vector<Particle::CModel*> models = group->getModels();
			for (Particle::CModel* model : models)
			{
				CParticleHierachyNode* nodeModel = nodeModels->addChild();
				nodeModel->setTagData(model, CParticleHierachyNode::Model);
				nodeModel->setParentData(group);
				nodeModel->setIcon(GUI::ESystemIcon::ParticleModel);
				nodeModel->setName(model->getName());
				setNodeEvent(nodeModel);
			}

			Particle::IRenderer* renderer = group->getRenderer();
			if (renderer)
			{
				CParticleHierachyNode* nodeRenderer = node->addChild();
				nodeRenderer->setTagData(renderer, CParticleHierachyNode::Renderer);
				nodeRenderer->setParentData(group);
				nodeRenderer->setIcon(GUI::ESystemIcon::ParticleRenderer);
				nodeRenderer->setName(renderer->getName());
				setNodeEvent(nodeRenderer);
			}
			else
			{
				CParticleHierachyNode* nodeRenderer = node->addChild();
				nodeRenderer->setTagData(NULL, CParticleHierachyNode::Renderer);
				nodeRenderer->setParentData(group);
				nodeRenderer->setIcon(GUI::ESystemIcon::ParticleRenderer);
				nodeRenderer->setName(L"No Renderer");
				setNodeEvent(nodeRenderer);
			}

			CParticleHierachyNode* gravity = node->addChild();
			gravity->setTagData(group, CParticleHierachyNode::Gravity);
			gravity->setParentData(group);
			gravity->setIcon(GUI::ESystemIcon::Axis);
			gravity->setName(L"Gravity Rotation");
			setNodeEvent(gravity);

			CParticleHierachyNode* orientation = node->addChild();
			orientation->setTagData(group, CParticleHierachyNode::Orientation);
			orientation->setParentData(group);
			orientation->setIcon(GUI::ESystemIcon::Axis);
			orientation->setName(L"Particle Rotation");
			setNodeEvent(orientation);

			std::vector<Particle::CSubGroup*> subGroups = m_particle->getData()->getSubGroup(group);
			if (subGroups.size() > 0)
			{
				CParticleHierachyNode* nodeSubGroups = node->addChild();
				nodeSubGroups->setTagData(NULL, CParticleHierachyNode::SubGroups);
				nodeSubGroups->setParentData(group);
				nodeSubGroups->setIcon(GUI::ESystemIcon::Folder);
				nodeSubGroups->setName(L"SubGroups");
				setNodeEvent(nodeSubGroups);

				for (Particle::CSubGroup* g : subGroups)
				{
					CParticleHierachyNode* node = nodeSubGroups->addChild();
					node->setTagData(g, CParticleHierachyNode::Group);
					node->setParentData(g);
					node->setIcon(GUI::ESystemIcon::Folder);
					node->setName(g->Name.c_str());
					setNodeEvent(node);

					updateGroupHierachy(node, g);
				}
			}
		}

		void CParticleController::setNodeEvent(CParticleHierachyNode* node)
		{
			node->OnSelected = std::bind(&CParticleController::onSelectNode, this, std::placeholders::_1, std::placeholders::_2);
		}

		void CParticleController::onSelectNode(CParticleHierachyNode* node, bool selected)
		{
			CParticleHierachyNode::EDataType type = node->getTagDataType();

			CSceneController* sceneController = CSceneController::getInstance();
			CParticleGizmos* particleGizmos = sceneController->getParticleGizmos();

			if (type == CParticleHierachyNode::Emitter)
			{
				Particle::CEmitter* emitter = (Particle::CEmitter*)node->getTagData();
				if (emitter->haveDirection())
				{
					particleGizmos->setEmitter((Particle::CDirectionEmitter*)emitter, m_particle->getGameObject()->calcWorldTransform());
					sceneController->setGizmos(particleGizmos);
				}
				else
				{
					sceneController->setGizmos(sceneController->getTransformGizmos());
				}
			}
			else if (type == CParticleHierachyNode::Zone)
			{
				Particle::CZone* zone = (Particle::CZone*)node->getTagData();

				particleGizmos->setZone(zone, m_particle->getGameObject()->calcWorldTransform());
				sceneController->setGizmos(particleGizmos);
			}
			else if (type == CParticleHierachyNode::Gravity)
			{
				Particle::CGroup* group = (Particle::CGroup*)node->getTagData();

				particleGizmos->setGravityRotation(group, m_particle->getGameObject()->calcWorldTransform());
				sceneController->setGizmos(particleGizmos);
			}
			else if (type == CParticleHierachyNode::Orientation)
			{
				Particle::CGroup* group = (Particle::CGroup*)node->getTagData();

				particleGizmos->setParticleRotation(group, m_particle->getGameObject()->calcWorldTransform());
				sceneController->setGizmos(particleGizmos);
			}
			else
			{
				sceneController->setGizmos(sceneController->getTransformGizmos());
			}

			CSpaceProperty* spaceProperty = (CSpaceProperty*)CEditor::getInstance()->getWorkspaceByName(L"Property");
			if (spaceProperty)
			{
				switch (type)
				{
				case CParticleHierachyNode::Group:
				case CParticleHierachyNode::Renderer:
				case CParticleHierachyNode::Emitter:
				case CParticleHierachyNode::Zone:
				case CParticleHierachyNode::Model:
				case CParticleHierachyNode::Gravity:
				case CParticleHierachyNode::Orientation:
				{
					Particle::CParticleSerializable* ps = (Particle::CParticleSerializable*)node->getTagData();
					if (ps)
						CPropertyController::getInstance()->setParticleProperty(spaceProperty, ps, m_particle);
					else
						CPropertyController::getInstance()->setProperty(NULL);
				}
				break;
				default:
					CPropertyController::getInstance()->setProperty(NULL);
					break;
				}
			}
		}

		void CParticleController::onContextMenu(CParticleHierachyNode* node)
		{
			if (m_canvas == NULL)
				return;

			if (m_contextMenuParticle->onContextMenu(node, m_particle))
				m_contextNode = node;
		}

		void CParticleController::createGroup(Particle::CParticleComponent* particle)
		{
			Particle::CFactory* factory = particle->getParticleFactory();

			// Group
			Particle::CGroup* group = particle->createParticleGroup();

			// Emitter
			Particle::CStraightEmitter* emitter = factory->createStraightEmitter(Transform::Oy);
			emitter->setFlow(10.0f);
			group->addEmitter(emitter);

			// Zone
			Particle::CAABox* zone = factory->createAABoxZone(core::vector3df(), core::vector3df(1.0f, 1.0f, 1.0f));
			emitter->setZone(zone);

			// Models
			Particle::CModel* model = group->createModel(Particle::ColorA);
			model->setStart(1.0f)->setEnd(0.0f);

			// Renderer
			Particle::CQuadRenderer* renderer = factory->createQuadRenderer();
			group->setRenderer(renderer);

			m_particle->Play();

			onCreateGroup(group);
		}

		void CParticleController::onCreateGroup(Particle::CGroup* group)
		{
			// Build hierachy node
			CParticleHierachyNode* node = buildGroupHierachy(group);

			if (m_spaceParticle)
				m_spaceParticle->addToTreeNode(node);
		}

		void CParticleController::createSubGroup(Particle::CParticleComponent* particle, Particle::CGroup* group)
		{
			Particle::CSubGroup* newSubGroup = particle->createParticleSubGroup(group);
			newSubGroup->Name = L"SubGroup";

			// Emitter
			Particle::CFactory* factory = particle->getParticleFactory();
			Particle::CRandomEmitter* emitter = factory->createRandomEmitter();
			emitter->setFlow(10.0f);
			newSubGroup->addEmitter(emitter);

			// Zone
			Particle::CSphere* zone = factory->createSphereZone(core::vector3df(), 0.5f);
			emitter->setZone(zone);

			// Models
			Particle::CModel* model = newSubGroup->createModel(Particle::ColorA);
			model->setStart(1.0f)->setEnd(0.0f);

			// Renderer
			Particle::CQuadRenderer* renderer = factory->createQuadRenderer();
			renderer->SizeX = 0.3f;
			renderer->SizeY = 0.3f;
			renderer->SizeZ = 0.3f;
			newSubGroup->setRenderer(renderer);

			// First init
			newSubGroup->initParticles();

			updateGroupHierachy(group);
		}
	}
}