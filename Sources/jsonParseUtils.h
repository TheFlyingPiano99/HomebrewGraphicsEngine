#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include "nlohmann/json.hpp"

/*
* Created by Zoltan Simon (2023)
* JSON utilities
*/

namespace Hogra {

	inline glm::vec2 parseVec2(const nlohmann::json& jsonData) {
		glm::vec2 v;
		v.x = jsonData[0];
		v.y = jsonData[1];
		return v;
	}

	inline glm::vec3 parseVec3(const nlohmann::json& jsonData) {
		glm::vec3 v;
		v.x = jsonData[0];
		v.y = jsonData[1];
		v.z = jsonData[2];
		return v;
	}

	inline glm::vec4 parseVec4(const nlohmann::json& jsonData) {
		glm::vec4 v;
		v.x = jsonData[0];
		v.y = jsonData[1];
		v.z = jsonData[2];
		v.w = jsonData[3];
		return v;
	}

	inline glm::mat3 parseMat3(const nlohmann::json& jsonData) {
		glm::mat3 m;
		m[0][0] = jsonData[0][0];
		m[0][1] = jsonData[0][1];
		m[0][2] = jsonData[0][2];
		m[1][0] = jsonData[1][0];
		m[1][1] = jsonData[1][1];
		m[1][2] = jsonData[1][2];
		m[2][0] = jsonData[2][0];
		m[2][1] = jsonData[2][1];
		m[2][2] = jsonData[2][2];
		return m;
	}

	inline glm::mat4 parseMat4(const nlohmann::json& jsonData) {
		glm::mat4 m;
		m[0][0] = jsonData[0][0];
		m[0][1] = jsonData[0][1];
		m[0][2] = jsonData[0][2];
		m[0][3] = jsonData[0][3];
		m[1][0] = jsonData[1][0];
		m[1][1] = jsonData[1][1];
		m[1][2] = jsonData[1][2];
		m[1][3] = jsonData[1][3];
		m[2][0] = jsonData[2][0];
		m[2][1] = jsonData[2][1];
		m[2][2] = jsonData[2][2];
		m[2][3] = jsonData[2][3];
		m[3][0] = jsonData[3][0];
		m[3][1] = jsonData[3][1];
		m[3][2] = jsonData[3][2];
		m[3][3] = jsonData[3][3];
		return m;
	}

}