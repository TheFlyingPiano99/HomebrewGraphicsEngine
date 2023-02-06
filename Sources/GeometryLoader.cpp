#include "GeometryLoader.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/postprocess.h>      
#include <assimp/scene.h>      

namespace Hogra {
	Geometry* GeometryLoader::Load(const std::string& path) {
		Geometry* geometry = nullptr;
		Assimp::Importer importer;
		auto* scene = importer.ReadFile(
			path,
			aiProcess_Triangulate
			| aiProcess_GenUVCoords
			| aiProcess_GenNormals
			| aiProcess_CalcTangentSpace
		);
		if (nullptr == scene) {
			throw GeometryLoadingException();
		}
		
		// Process meshes:
		for (int m = 0; m < scene->mNumMeshes; m++) {	// Obsolate loop (only one mesh)
			auto* mesh = scene->mMeshes[m];
			std::vector<Vertex_pos_norm_tang_bitang_uv> vertices;
			std::vector<GLuint> indices;
			for (int v = 0; v < mesh->mNumVertices; v++) {
				auto aiVertex = mesh->mVertices[v];
				auto aiNormal = mesh->mNormals[v];
				auto aiTangent = mesh->mTangents[v];
				auto aiBitangent = mesh->mBitangents[v];
				auto aiUV = mesh->mTextureCoords[0][v];
				
				Vertex_pos_norm_tang_bitang_uv vertex;
				vertex.position = glm::vec3(aiVertex.x, aiVertex.y, aiVertex.z);
				vertex.normal = glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z);
				vertex.tangent = glm::vec3(aiTangent.x, aiTangent.y, aiTangent.z);
				vertex.bitangent = glm::vec3(aiBitangent.x, aiBitangent.y, aiBitangent.z);
				vertex.uv = glm::vec2(aiUV.x, aiUV.y);
				vertices.push_back(vertex);
			}
			if (mesh->HasFaces()) {
				for (int f = 0; f < mesh->mNumFaces; f++) {
					auto face = mesh->mFaces[f];
					for (int i = 0; i < face.mNumIndices; i++) {
						indices.push_back(face.mIndices[i]);
					}
				}
			}
			geometry = Allocator::New<Geometry>();
			geometry->Init(vertices, indices);
			break;
		}
		return geometry;
	}
}
