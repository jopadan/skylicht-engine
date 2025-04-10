#include "pch.h"
#include "CDemoNavMesh.h"

#include "imgui.h"

#include "Primitive/CCapsule.h"
#include "IndirectLighting/CIndirectLighting.h"
#include "MeshManager/CMeshManager.h"
#include "Debug/CSceneDebug.h"

#include "CMoveAgent.h"

CDemoNavMesh::CDemoNavMesh(CZone* zone) :
	CDemo(zone),
	m_state(0),
	m_agent(NULL),
	m_map(NULL),
	m_recastMesh(NULL),
	m_tileWidth(2.0f),
	m_tileHeight(2.0f),
	m_fromTile(NULL),
	m_toTile(NULL),
	m_drawDebugRecastMesh(false),
	m_drawDebugNavMesh(true),
	m_drawDebugObstacle(true),
	m_drawDebugQueryBox(false),
	m_drawDebugTileMap(true)
{
	m_builder = new Graph::CRecastBuilder();
	m_obstacle = new Graph::CObstacleAvoidance();
	m_walkingTileMap = new Graph::CWalkingTileMap();
	m_query = new Graph::CGraphQuery();
	m_navMesh = new CMesh();
}

CDemoNavMesh::~CDemoNavMesh()
{
	if (m_recastMesh)
		delete m_recastMesh;
	delete m_builder;
	delete m_obstacle;
	delete m_navMesh;
	delete m_walkingTileMap;
	delete m_query;
}

void CDemoNavMesh::init()
{
	if (m_map == NULL)
		m_map = m_zone->searchObjectInChild(L"Map");

	if (m_agent == NULL)
	{
		m_agent = m_zone->createEmptyObject();

		CCapsule* capsule = m_agent->addComponent<CCapsule>();
		capsule->setRadius(0.5f);
		capsule->setHeight(0.5f);
		capsule->init();

		m_agent->addComponent<CIndirectLighting>();
		m_agent->addComponent<CMoveAgent>();
	}

	if (m_recastMesh == NULL)
	{
		m_recastMesh = new Graph::CRecastMesh();

		CEntityPrefab* mapPrefab = CMeshManager::getInstance()->loadModel(COLLISION_MODEL, "");
		if (mapPrefab)
			m_recastMesh->addMeshPrefab(mapPrefab, core::IdentityMatrix);
	}

	m_fromTile = NULL;
	m_toTile = NULL;
	m_path.clear();

	m_walkingTileMap->release();
	m_navMesh->removeAllMeshBuffer();
	m_obstacle->clear();

	m_agent->setVisible(true);
	m_agent->getTransformEuler()->setPosition(core::vector3df(0.0f, 1.0f, 0.0f));

	if (m_map)
		m_map->setVisible(true);

	CMoveAgent* moveAgent = m_agent->getComponent<CMoveAgent>();
	moveAgent->setGraphQuery(m_query);
	moveAgent->clearPath();

	m_clickPosition.set(0.0f, 0.0f, 0.0f);
}

void CDemoNavMesh::close()
{
	m_agent->setVisible(false);

	if (m_map)
		m_map->setVisible(false);
}

inline int bit(int a, int b)
{
	return (a & (1 << b)) >> b;
}

void intToCol(int i, int a, SColor& c)
{
	int	r = bit(i, 1) + bit(i, 3) * 2 + 1;
	int	g = bit(i, 2) + bit(i, 4) * 2 + 1;
	int	b = bit(i, 0) + bit(i, 5) * 2 + 1;
	c.set(a, r * 120, g * 120, b * 120);
}

void CDemoNavMesh::update()
{
	CSceneDebug* debug = CSceneDebug::getInstance();

	if (m_clickPosition.getLength() > 0.0f)
	{
		debug->getNoZDebug()->addPosition(m_clickPosition, 0.25f, SColor(255, 0, 200, 0));
	}

	SColor red(255, 100, 0, 0);
	SColor blue(255, 0, 0, 100);
	SColor grey(255, 20, 20, 20);
	SColor white(255, 100, 100, 100);
	SColor green(255, 0, 10, 0);
	SColor greenL(255, 0, 100, 0);

	// draw debug recastmesh
	if (m_drawDebugRecastMesh)
	{
		int* tris = m_recastMesh->getTris();
		float* verts = m_recastMesh->getVerts();

		for (u32 i = 0, n = m_recastMesh->getTriCount(); i < n; i++)
		{
			int a = tris[i * 3];
			int b = tris[i * 3 + 1];
			int c = tris[i * 3 + 2];

			core::vector3df va(verts[a * 3], verts[a * 3 + 1], verts[a * 3 + 2]);
			core::vector3df vb(verts[b * 3], verts[b * 3 + 1], verts[b * 3 + 2]);
			core::vector3df vc(verts[c * 3], verts[c * 3 + 1], verts[c * 3 + 2]);

			debug->addLine(va, vb, white);
			debug->addLine(vb, vc, white);
			debug->addLine(vc, va, white);
		}
	}

	// draw nav polymesh
	if (m_drawDebugNavMesh)
	{
		for (u32 i = 0, n = m_navMesh->getMeshBufferCount(); i < n; i++)
		{
			IMeshBuffer* mb = m_navMesh->getMeshBuffer(i);
			IVertexBuffer* vb = mb->getVertexBuffer();
			IIndexBuffer* ib = mb->getIndexBuffer();

			for (u32 i = 0, n = ib->getIndexCount(); i < n; i += 3)
			{
				u32 a = ib->getIndex(i);
				u32 b = ib->getIndex(i + 1);
				u32 c = ib->getIndex(i + 2);

				S3DVertex* p1 = (S3DVertex*)vb->getVertex(a);
				S3DVertex* p2 = (S3DVertex*)vb->getVertex(b);
				S3DVertex* p3 = (S3DVertex*)vb->getVertex(c);

				debug->addLine(p1->Pos, p2->Pos, white);
				debug->addLine(p2->Pos, p3->Pos, white);
				debug->addLine(p3->Pos, p1->Pos, white);
			}
		}
	}

	// draw bound obstacle
	if (m_drawDebugObstacle)
	{
		core::array<core::line3df>& segments = m_obstacle->getSegments();
		for (u32 i = 0, n = segments.size(); i < n; i++)
		{
			debug->addLine(segments[i], red);
		}
	}

	// draw query box
	if (m_drawDebugQueryBox)
	{
		if (m_query->getRootNode())
		{
			std::queue<Graph::COctreeNode*> nodes;
			nodes.push(m_query->getRootNode());

			while (nodes.size() > 0)
			{
				Graph::COctreeNode* node = nodes.front();
				nodes.pop();

				debug->addBoudingBox(node->Box, grey);

				for (int i = 0; i < 8; i++)
				{
					if (node->Childs[i])
						nodes.push(node->Childs[i]);
				}
			}
		}
	}

	// draw tilemap
	if (m_drawDebugTileMap)
	{
		core::vector3df halfHeight(0.0f, 0.2f, 0.0f);
		m_walkingTileMap->resetVisit();
		core::array<Graph::STile*>& tiles = m_walkingTileMap->getTiles();

		SColor c;

		for (u32 i = 0, n = tiles.size(); i < n; i++)
		{
			Graph::STile* tile = tiles[i];

			intToCol(tile->AreaId, 255, c);

			debug->addLine(tile->Position, tile->Position + halfHeight, c);

			/*
			if (tile == m_fromTile)
			{
				for (u32 j = 0, n = tile->Neighbours.size(); j < n; j++)
				{
					Graph::STile* nei = tile->Neighbours[j];
					debug->addLine(tile->Position, nei->Position, green);
				}
			}
			*/

			tile->Visit = true;
		}
		m_walkingTileMap->resetVisit();
	}

	// draw path
	if (m_fromTile)
		debug->addBoudingBox(m_fromTile->BBox, greenL);
	if (m_toTile)
		debug->addBoudingBox(m_toTile->BBox, greenL);

	for (int i = 0, n = (int)m_path.size() - 1; i < n; i++)
	{
		Graph::STile* tile1 = m_path[i];
		Graph::STile* tile2 = m_path[i + 1];
		debug->addLine(tile1->Position, tile2->Position, blue);
	}
}

void CDemoNavMesh::onGUI()
{
	ImGui::Text("Demo NavMesh");
	ImGui::Text("After build NavMesh");
	ImGui::Text("- Left mouse to set Agent position");
	ImGui::Text("- Right mouse to move Agent");

	if (ImGui::CollapsingHeader("Draw Debug"))
	{
		ImGui::Checkbox("Recast mesh", &m_drawDebugRecastMesh);
		ImGui::Checkbox("Navmesh", &m_drawDebugNavMesh);
		ImGui::Checkbox("Obstacle", &m_drawDebugObstacle);
		ImGui::Checkbox("Query box", &m_drawDebugQueryBox);
		ImGui::Checkbox("Walk tilemap", &m_drawDebugTileMap);
	}

	ImGui::Text("Build step 1");

	if (ImGui::CollapsingHeader("Config NavMesh"))
	{
		Graph::SBuilderConfig config = m_builder->getConfig();
		ImGui::SliderFloat("CellSize", &config.CellSize, 0.2f, 1.0f);
		ImGui::SliderFloat("CellHeight", &config.CellHeight, 0.2f, 1.0f);
		ImGui::SliderFloat("AgentHeight", &config.AgentHeight, 0.5f, 2.0f);
		ImGui::SliderFloat("AgentRadius", &config.AgentRadius, 0.1f, 1.0f);
		ImGui::SliderFloat("AgentMaxClimb", &config.AgentMaxClimb, 0.1f, 1.0f);
		ImGui::SliderFloat("AgentMaxSlope", &config.AgentMaxClimb, 5.0f, 60.0f);
		m_builder->setConfig(config);
	}

	if (ImGui::Button("Build NavMesh"))
	{
		buildNavMesh();
	}

	ImGui::Spacing();

	ImGui::Text("Build step 2");

	if (ImGui::CollapsingHeader("Config Tile"))
	{
		ImGui::SliderFloat("TileWidth", &m_tileWidth, 1.0f, 4.0f);
		ImGui::SliderFloat("TileHeight", &m_tileHeight, 1.0f, 4.0f);
	}

	if (ImGui::Button("Build Walking TileMap"))
	{
		buildWalkingMap();
	}

	ImGui::Spacing();
	ImGui::Spacing();
}

void CDemoNavMesh::onViewRayClick(const core::line3df& ray, int button, bool holdShift)
{
	core::vector3df outIntersection;
	core::triangle3df outTriangle;
	float rayDistance = 10000.0f;

	if (m_query->getCollisionPoint(ray, rayDistance, outIntersection, outTriangle) == true)
	{
		m_clickPosition = outIntersection;

		CMoveAgent* moveAgent = m_agent->getComponent<CMoveAgent>();

		if (button == 0)
		{
			m_fromTile = m_walkingTileMap->getTileByPosition(m_clickPosition);

			// left click
			moveAgent->setPosition(m_clickPosition);
			moveAgent->setTargetPosition(m_clickPosition);
		}
		else
		{
			m_fromTile = m_walkingTileMap->getTileByPosition(moveAgent->getPosition());
			m_toTile = m_walkingTileMap->getTileByPosition(m_clickPosition);

			// right click
			moveAgent->setTargetPosition(m_clickPosition);
		}

		if (m_fromTile && m_toTile)
		{
			if (m_query->findPath(m_walkingTileMap, m_fromTile, m_toTile, m_path))
			{
				moveAgent->setPath(m_path, m_clickPosition);
			}
		}
	}
}

void CDemoNavMesh::buildNavMesh()
{
	m_builder->build(m_recastMesh, m_navMesh, m_obstacle);
	m_query->buildIndexNavMesh(m_navMesh, m_obstacle);
}

void CDemoNavMesh::buildWalkingMap()
{
	m_fromTile = NULL;
	m_toTile = NULL;
	m_path.clear();

	CMoveAgent* moveAgent = m_agent->getComponent<CMoveAgent>();
	moveAgent->clearPath();

	m_walkingTileMap->generate(m_tileWidth, m_tileHeight, m_navMesh, m_obstacle);
}
