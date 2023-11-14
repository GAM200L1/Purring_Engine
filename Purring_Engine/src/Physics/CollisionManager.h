/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CollisionManager.h
 \date     10-09-2023
 
 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu
 
 \brief   Contains declaration of CollisionManager class and its member functions
		  as well as declarations for functions that check if two objects with colliders have collided
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include "System.h"
#include "SpatialGrid.h"

namespace PE
{

	class CollisionManager : public System
	{
	public:

		// ----- Public Variable ----- //
		static vec2 gridSize;
		static bool gridActive;

		// ----- Constructors/Destructors ----- //
		/*!***********************************************************************************
		 \brief Construct a new Collision Manager object
		 
		*************************************************************************************/
		CollisionManager();

		/*!***********************************************************************************
		 \brief Destroy a Collision Manager object

		*************************************************************************************/
		~CollisionManager();

		// ----- Public Getters ----- //
		/*!***********************************************************************************
		 \brief Get the pointer to the first element in the m_manifolds vector
		 
		 \return Manifold* - Pointer to the first object in the m_manifolds vector
		*************************************************************************************/
		Manifold* GetManifoldVector();
		
		/*!***********************************************************************************
		 \brief Get the name of the system which is Collision
		 
		 \return std::string - m_systemName variable
		*************************************************************************************/
		std::string GetName();

		// ----- System Methods ----- //
		/*!***********************************************************************************
		 \brief Prints to console log that CollisionManager has been successfully initialized
		 		and added to list of systems
		 
		*************************************************************************************/
		void InitializeSystem();

		/*!***********************************************************************************
		 \brief Calls Collision Methods which update the colliders to their new position and scales,
		 		tests for collisions, and resolves collisions
		 
		 \param[in] deltaTime - difference in time between previous frame and current frame
		*************************************************************************************/
		void UpdateSystem(float deltaTime);

		/*!***********************************************************************************
		 \brief Calls any destructors and frees any memory allocated
		 
		*************************************************************************************/
		void DestroySystem();

		// ----- Collision Methods ----- //
		/*!***********************************************************************************
		 \brief Input's each collider into their respective update function to update their 
		 		position and scale
		 
		*************************************************************************************/
		void UpdateColliders();

		/*!***********************************************************************************
		 \brief Tests for collision between two objects by inputting them as parameters into a
		 		helper function that tests collision for various types of collisions.
		 
		*************************************************************************************/
		void TestColliders();

		/*!***********************************************************************************
		 \brief Resolves the velocity and positions of objects with colliders if they have collided
		 
		*************************************************************************************/
		void ResolveCollision();

	private:

		Grid m_grid;
		std::vector<Manifold> m_manifolds;
		std::set <std::pair<size_t, size_t>> m_collisionPairs;
		std::string m_systemName{ "CollisionManager" };
	};

	// ----- Collision Helper Functions ----- //
	
	/*!***********************************************************************************
	 \brief Tests for collision between two AABB Colliders
	 
	 \param[in,out] r_AABB1 - collider of first object with AABB
	 \param[in,out] r_AABB2 - collider of second object with AABB
	 \param[in,out] r_contactPt - reference to contact point to save collision information into
	 \return true - both objects have collided
	 \return false - objects did not collide 
	*************************************************************************************/
	bool CollisionIntersection(AABBCollider const& r_AABB1, AABBCollider const& r_AABB2, Contact& r_contactPt);

	/*!***********************************************************************************
	 \brief Tests for collision between two Circle Colliders
	 
	 \param[in,out] r_circle1 - collider of first object with circle collider
	 \param[in,out] r_circle2 - collider of second object with circle collider
	 \param[in,out] r_contactPt - reference to contact point to save collision information into
	 \return true - both objects have collided
	 \return false - objects did not collide 
	*************************************************************************************/
	bool CollisionIntersection(CircleCollider const& r_circle1, CircleCollider const& r_circle2, Contact& r_contactPt);
	
	/*!***********************************************************************************
	 \brief Tests for collision between an AABB and Circle collider in that order
	 
	 \param[in,out] r_AABB - collider of first object with AABB
	 \param[in,out] r_circle - collider of second object with circle collider
	 \param[in,out] r_contactPt - reference to contact point to save collision information into
	 \return true - both objects have collided
	 \return false - objects did not collide 
	*************************************************************************************/
	bool CollisionIntersection(AABBCollider const& r_AABB, CircleCollider const& r_circle, Contact& r_contactPt);

	/*!***********************************************************************************
	 \brief Tests for collision between an Circle and AABB collider in that order
	 
	 \param[in,out] r_circle - collider of first object with circle collider
	 \param[in,out] r_circle - collider of second object with AABB collider
	 \param[in,out] r_contactPt - reference to contact point to save collision information into
	 \return true - both objects have collided
	 \return false - objects did not collide 
	*************************************************************************************/
	bool CollisionIntersection(CircleCollider const& r_circle, AABBCollider const& r_AABB, Contact& r_contactPt);
	
	/*!***********************************************************************************
	 \brief Helper function for Circle-AABB Collision. Essentially checks for Circle-Line Intersection.
	 
	 \param[in,out] r_circle - collider of object with circle collider
	 \param[in,out] r_lineSeg - Line Segment to check circle against
	 \return true - Circle collided with line
	 \return false - Circle did not collide with line
	*************************************************************************************/
	int CircleAABBEdgeIntersection(CircleCollider const& r_circle, LineSegment const& r_lineSeg); //, float& r_interTime, Contact& r_contactPt
	
}