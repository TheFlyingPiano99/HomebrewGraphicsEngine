#pragma once
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/postprocess.h>      
#include <assimp/scene.h>      
#include "Geometry.h"

namespace hograengine {
	class GeometryLoader
	{
	public:

		class GeometryLoadingException : public std::exception {
		};

		Geometry* load(const std::string& path) {
			Geometry* geometry = nullptr;
			Assimp::Importer importer;
			auto* scene = importer.ReadFile(
				path, 
				aiProcess_Triangulate 
				| aiProcess_GenUVCoords
				| aiProcess_GenNormals
			);
			if (nullptr == scene) {
				throw GeometryLoadingException();
			}


			// Process meshes:
			for (int m = 0; m < scene->mNumMeshes; m++) {
				auto* mesh = scene->mMeshes[m];
				std::vector<Vertex> vertices;
				std::vector<GLint> indices;
				for (int v = 0; v < mesh->mNumVertices; v++) {
					auto aiVertex = mesh->mVertices[v];
					auto aiNormal = mesh->mNormals[v];
					auto aiUV = mesh->mTextureCoords[0][v];

					Vertex vertex;
					vertex.position = glm::vec4(aiVertex.x, aiVertex.y, aiVertex.z, 1.0f);
					vertex.normal = glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z);
					if (mesh->HasTextureCoords(0)) {
						vertex.texUV = glm::vec2(aiUV.x, aiUV.y);
					}
					vertex.color = glm::vec3(0.18f, 0.18f, 0.18f);
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
				geometry = new Geometry(vertices, indices);
			}
			return geometry;
		}


	};
}

