/******************************************************************************/
/*!
\file  Physics.h
\Proj name  Shroomy Doomy
\author  Reagan Tang
\date    Oct 03, 2024
\brief  This file contains the declarations for the PhysicsObject class 
        and related functions, responsible for handling physics calculations 
        in the game, such as velocity, acceleration, and object movement.

All content ? 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /******************************************************************************/
#pragma once

#include "Input.h"

namespace FruitLoops
{
    class PhysicsObject;
    // General Force class
    class Force {
    public:
        Vector2D direction;
        float magnitude;
        float lifetime;
        bool isActive;

        Force(const Vector2D& dir, float mag, float life = std::numeric_limits<float>::infinity(), bool active = true)
        : direction(dir), magnitude(mag), lifetime(life), isActive(active) {}

        virtual void Apply(PhysicsObject& obj);
        virtual ~Force() {}
    };

    // For 2D forces
    class LinearForce : public Force {
    public:
        using Force::Force; // Inherit constructor

        void Apply(PhysicsObject& obj) override;
    };

    // For Rotational force
    class RotationalForce : public Force {
    public:
        using Force::Force;

        void Apply(PhysicsObject& obj) override;
    };

    class ForcesManager {
    public:
        void AddForce(std::shared_ptr<Force> m_forces);
        void UpdateForces(float deltaTime);
        Vector2D SumForces() const;
        void Clear();

    private:
        std::vector<std::shared_ptr<Force>> m_forces;
    };

    class PhysicsObject : public EngineSystems
    {
    public:
        // Properties
        Vector2D position;
        Vector2D velocity;
        Vector2D acceleration;
        Vector2D angularVelocity;
        Vector2D angularAcceleration;
        float mass;
        float drag; // Drag coefficient for linear resistance
        float inertia;
        ForcesManager forcesManager;

        //PhysicsObject();
        //PhysicsObject(Vector2D startPosition, float massValue);
        //void Initialize();
        //void ApplyForce(const Vector2D& force);
        //void Update(float deltaTime);



/**************************************************************************
* @brief
*    Initializes the PhysicsObject's velocity and acceleration to their
*    default values.
*************************************************************************/
        void Initialize() override;

/**************************************************************************
* @brief
*    Default constructor for the PhysicsObject class. Initializes a new
*    PhysicsObject with default values.
*************************************************************************/
        PhysicsObject();

/**************************************************************************
* @brief
*    Constructor for the PhysicsObject class that initializes its position
*    and mass, while setting velocity and acceleration to zero.
*
* @param startPosition
*    The starting position of the physics object (Vector2D).
* @param massValue
*    The mass of the physics object (float).
*************************************************************************/
        PhysicsObject(Vector2D startPosition, float massValue);

/**************************************************************************
* @brief
*    Applies a force to the physics object, adjusting its acceleration
*    based on the force and its mass.
*
* @param force
*    The force vector to be applied (Vector2D).
*************************************************************************/
        void ApplyForce(const Vector2D& force);

/**************************************************************************
* @brief
*    Updates the position of the physics object based on the current velocity
*    and input system, factoring in the deltaTime for frame-rate independence.
*
* @param pos
*    The position of the physics object (Vector2D).
* @param deltaTime
*    The time step between frames (float).
*************************************************************************/
        void UpdatePosition(Vector2D& position, float deltaTime);

/**************************************************************************
* @brief
*    Updates the velocity of the physics object based on the current
*    acceleration and deltaTime, then resets the acceleration for the next
*    frame.
*
* @param vel
*    The velocity of the physics object (Vector2D).
* @param deltaTime
*    The time step between frames (float).
*************************************************************************/
        void UpdateVelocity(Vector2D& velocity, float deltaTime);

/**************************************************************************
* @brief
*    Updates the physics simulation by iterating through all entities with
*    RigidBody and Transform components, updating their velocities and positions.
*
* @param deltaTime
*    The time step between frames (float).
*************************************************************************/
        void Update(float deltaTime) override;

    private:
        void ApplyDrag();
    };
}
