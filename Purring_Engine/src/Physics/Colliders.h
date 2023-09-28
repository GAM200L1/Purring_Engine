/*!*****************************************************************************
	@file       Colliders.h
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#pragma once

#include "Math/MathCustom.h"
#include "Math/Transform.h"
#include "RigidBody.h"
#include <variant>
#include "Data/json.hpp"
#include <set>

namespace PE
{
	// may change this to box collider to combine obb and aabb in the future
	struct AABBCollider
	{
		// ----- Public Variables ----- //
		vec2 positionOffset{ 0.f, 0.f };
		vec2 scaleOffset{ 1.f, 1.f };
		vec2 center{};
		vec2 min{};
		vec2 max{};
		vec2 scale{};


		// ----- Public Methods ----- //
		// For Future -hans
		// Serialization
		nlohmann::json ToJson() const
		{
			nlohmann::json j;
			j["min"]["x"] = min.x;
			j["min"]["y"] = min.y;
			j["max"]["x"] = max.x;
			j["max"]["y"] = max.y;
			return j;
		}

		// Deserialization
		static AABBCollider FromJson(const nlohmann::json& j)
		{
			AABBCollider aabb;
			aabb.min.x = j["min"]["x"];
			aabb.min.y = j["min"]["y"];
			aabb.max.x = j["max"]["x"];
			aabb.max.y = j["max"]["y"];
			return aabb;
		}
	};

	void Update(AABBCollider& r_AABB, vec2 const& r_position, vec2 const& r_scale);

	struct CircleCollider
	{
		// ----- Public Variables ----- //
		vec2 positionOffset{ 0.f, 0.f };
		float scaleOffset{ 1.f };
		vec2 center{};
		float radius{};


		// ----- Public Methods ----- //
		// For Future -hans
		// Serialization
		nlohmann::json ToJson() const
		{
			nlohmann::json j;
			j["center"]["x"] = center.x;
			j["center"]["y"] = center.y;
			j["radius"] = radius;
			return j;
		}

		// Deserialization
		static CircleCollider FromJson(const nlohmann::json& j)
		{
			CircleCollider circle;
			circle.center.x = j["center"]["x"];
			circle.center.y = j["center"]["y"];
			circle.radius = j["radius"];
			return circle;
		}

	};

	void Update(CircleCollider& r_circle, vec2 const& r_position, vec2 const& r_scale);

	struct Collider
	{
		std::variant<AABBCollider, CircleCollider> colliderVariant;
		std::set<size_t> objectsCollided;


		// ----- Public Methods ----- //
		// Serialization
		nlohmann::json ToJson() const
		{
			nlohmann::json j;
			if (std::holds_alternative<AABBCollider>(colliderVariant))
			{
				j["type"] = "AABBCollider";
				//j["data"] = std::get<AABBCollider>(colliderVariant).ToJson();			// for future -hans
			}
			else if (std::holds_alternative<CircleCollider>(colliderVariant))
			{
				j["type"] = "CircleCollider";
				//j["data"] = std::get<CircleCollider>(colliderVariant).ToJson();		// for future -hans
			}
			return j;
		}

		// Deserialization
		static Collider FromJson(const nlohmann::json& j)
		{
			Collider collider;
			std::string type = j["type"];
			if (type == "AABBCollider")
			{
				collider.colliderVariant = AABBCollider::FromJson(j["data"]);
			}
			else if (type == "CircleCollider")
			{
				collider.colliderVariant = CircleCollider::FromJson(j["data"]);
			}
			return collider;
		}
		bool isTrigger{ false };
	};

	struct LineSegment
	{
		LineSegment() = default;
		LineSegment(vec2 const& r_startPt, vec2 const& r_endPt);
		vec2 point0;
		vec2 point1;
		vec2 lineVec;
		vec2 normal;
	};


	struct Contact
	{
		vec2 intersectionPoint{ 0.f, 0.f };
		vec2 normal{ 0.f, 0.f };
		float penetrationDepth{ 0.f };
	};

	struct Manifold
	{
		Manifold() = delete;
		Manifold(Contact const& r_contData,
			Transform& r_transA, Transform& r_transB,
			RigidBody* r_rbA, RigidBody* r_rbB);

		Transform& r_transformA;
		Transform& r_transformB;

		RigidBody* r_rigidBodyA = nullptr;
		RigidBody* r_rigidBodyB = nullptr;

		Contact contactData;

		void ResolveCollision();
		void ResolveVelocity();
		void ResolvePosition();
	};
}
