#include "HeightMap.h"

HeightMap::HeightMap(std::string name)
{
	std::ifstream file(name.c_str(), std::ios::binary);
	if (!file)
		return;

	numVertices = RAW_WIDTH * RAW_HEIGHT;
	numIndices = (RAW_WIDTH - 1) * (RAW_HEIGHT - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];
	unsigned char *data = new unsigned char[numVertices];
	file.read((char *)data, numVertices * sizeof(unsigned char));
	file.close();

	for (int x = 0; x < RAW_WIDTH; ++x) {
		for (int z = 0; z < RAW_HEIGHT; ++z) {
			int offset = (x * RAW_WIDTH) + z;

			vertices[offset] = Vector3(x * HEIGHTMAP_X, data[offset]
				* HEIGHTMAP_Y, z * HEIGHTMAP_Z);

			textureCoords[offset] = Vector2(x * HEIGHTMAP_TEX_X, z
				* HEIGHTMAP_TEX_Z);
		}
	}

	delete data;
	numIndices = 0;

	for (int x = 0; x < RAW_WIDTH - 1; ++x) {
		for (int z = 0; z < RAW_HEIGHT - 1; ++z) {
			int a = (x * (RAW_WIDTH)) + z;
			int b = ((x + 1) * (RAW_WIDTH)) + z;
			int c = ((x + 1) * (RAW_WIDTH)) + (z + 1);
			int d = (x * (RAW_WIDTH)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;
		}
	}

	GenerateNormals();

	// calculate slopes
	slopes = new float[numVertices];
	for (int i = 0; i < numVertices; i++) {
		float slope = 1.0f - normals[i].y;
		slopes[i] = slope;
	}

	// create array of modifiable vertices
	snowVertex = new Vector3[numVertices];
	snowVertexPull = new bool[numVertices] { false };
	for (int i = 0; i < numVertices; i++) {
		if (slopes[i] < 0.22) {
			snowVertex[i] = vertices[i];
		}
		else {
			snowVertex[i] = Vector3(0, 0, 0);
		}
	}
	// 66048 vertices
	// 45868 modifiable vertices
	// Vertex 0-256 is x value of 0 (jumps in sets of 256)

	BufferData();
}

HeightMap::~HeightMap()
{
	delete slopes;
	delete snowVertex;
	delete snowVertexPull;
}

bool HeightMap::getPosition(float x, float z)
{
	float worldX = x;
	float worldZ = z;

	// work out which grid square on the terrain is the player in
	int gridX = (int)floor((worldX / HEIGHTMAP_X));
	int gridZ = (int)floor((worldZ / HEIGHTMAP_Z));

	// test if this x,z position is on the terrain
	if (gridX >= 256 || gridZ >= 256 || gridX < 0 || gridZ < 0) {
		return false;
	}

	// Working out which triangle the player is standing on
	float xCoord = fmod(worldX, HEIGHTMAP_X) / HEIGHTMAP_X;
	float zCoord = fmod(worldZ, HEIGHTMAP_Z) / HEIGHTMAP_Z;
	float answer;
	if (xCoord <= (1 - zCoord)) {
		// in one half of a triangle
		// Triangle vertices
		int v1 = (gridX * 257) + gridZ;
		int v2 = ((gridX + 1) * 257) + gridZ;
		int v3 = (gridX * 257) + (gridZ + 1);

		// Test if triangle vertices are pullable
		if (snowVertex[v1] != Vector3(0, 0, 0)
			&& snowVertex[v2] != Vector3(0, 0, 0)
			&& snowVertex[v3] != Vector3(0, 0, 0)) {
			snowVertexPull[v1] = true;
			snowVertexPull[v2] = true;
			snowVertexPull[v3] = true;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		// in the other half of a triangle
		// Triangle vertices
		int v1 = ((gridX + 1) * 257) + gridZ;
		int v2 = ((gridX + 1) * 257) + (gridZ + 1);
		int v3 = (gridX * 257) + (gridZ + 1);

		// Test if triangle vertices are pullable
		if (snowVertex[v1] != Vector3(0, 0, 0)
			&& snowVertex[v2] != Vector3(0, 0, 0)
			&& snowVertex[v3] != Vector3(0, 0, 0)) {
			snowVertexPull[v1] = true;
			snowVertexPull[v2] = true;
			snowVertexPull[v3] = true;
			return true;
		}
		else {
			return false;
		}
	}
}

void HeightMap::Draw()
{
	RebufferData();
	Mesh::Draw();
}

void HeightMap::update(float msec)
{
	for (int i = 0; i < numVertices; i++) {
		if (snowVertexPull[i]) {
			vertices[i].y += 3.0f;
			snowVertexPull[i] = false;
		}
	}
}

void HeightMap::reset()
{
	for (int i = 0; i < numVertices; i++) {
		if (snowVertex[i] != Vector3(0, 0, 0)) {
			vertices[i].y = snowVertex[i].y;
		}
		snowVertexPull[i] = false;
	}
}
