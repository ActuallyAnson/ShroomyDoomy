/******************************************************************************/
/*!
\file  Physics.cpp
\Proj name  Shroomy Doomy
\author  Reagan Tang
\date    Oct 03, 2024
\brief  This file contains the implementations for the PhysicsObject class
        and related functions, responsible for handling physics calculations
        in the game, such as velocity, acceleration, and object movement.

All content ? 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /******************************************************************************/
#include "pch.h"
#include "Physics.h"

namespace FruitLoops {
    /*!
    * \brief Applies the force to the PhysicsObject, modifying its acceleration.
    *
    * Checks if the force is active before applying it to the object's acceleration.
    * The applied acceleration is scaled by the object's mass.
    *
    * \param obj The physics object that will have its acceleration updated.
    */
    void Force::Apply(PhysicsObject& obj) {
        if (!isActive) return;
        obj.acceleration += direction * (magnitude / obj.mass);
    }

    /*!
     * \brief Applies a linear force to the PhysicsObject, updating its acceleration.
     *
     * The force is applied by scaling the force's magnitude with respect to the object's mass.
     *
     * \param obj The physics object to which the linear force is applied.
     */
    void LinearForce::Apply(PhysicsObject& obj) {
        obj.acceleration += this->direction * (this->magnitude / obj.mass);
    }

    /*!
     * \brief Applies a rotational force (torque) to the PhysicsObject, updating its angular velocity.
     *
     * The torque is computed based on the force's magnitude and then divided by the object's inertia.
     *
     * \param obj The physics object to which the rotational force is applied.
     */
    void RotationalForce::Apply(PhysicsObject& obj) {
        float torque = this->magnitude; // Define how torque is calculated
        obj.angularVelocity += torque / obj.inertia;
    }

    /*!
    * \brief Adds a force to the list of active forces acting on an object.
    * \param force The force vector to be added.
    */
    void ForcesManager::AddForce(std::shared_ptr<Force> force) {
        m_forces.push_back(std::move(force));
    }

    /*!
     * \brief Removes a force from the list of active forces by its index.
     * \param index The index of the force to remove.
     */
    void ForcesManager::UpdateForces(float deltaTime) {
        for (auto& force : m_forces) {
            if (force->isActive) {
                force->lifetime -= deltaTime;
                if (force->lifetime <= 0) {
                    force->isActive = false;
                }
            }
        }
        m_forces.erase(std::remove_if(m_forces.begin(), m_forces.end(),
            [](const std::shared_ptr<Force>& f) { return !f->isActive; }), m_forces.end());
    }

    /*!
     * \brief Calculates and returns the sum of all active forces.
     * \return A Vector2D representing the resultant force.
     */
    Vector2D ForcesManager::SumForces() const {
        Vector2D totalForce(0, 0);
        for (const auto& force : m_forces) {
            if (force->isActive) {
                totalForce += force->direction * force->magnitude;
            }
        }
        return totalForce;
    }

    /*!
     * \brief Clears all forces from the list of active forces.
     */
    void ForcesManager::Clear() {
        m_forces.clear();
    }

    /*!
     * \brief Default constructor for PhysicsObject, initializes drag and mass.
     */
    PhysicsObject::PhysicsObject() : drag(0.1f), mass(1.0f) , inertia(0.f) {}

    /*!
     * \brief Initializes the physics object by setting velocity and acceleration to zero.
     */
    void PhysicsObject::Initialize() {
        velocity = { 0.0f, 0.0f };
        acceleration = { 0.0f, 0.0f };
    }

    /*!
     * \brief Constructor for PhysicsObject, initializing position and mass.
     * \param startPosition The starting position of the physics object.
     * \param massValue The mass of the physics object.
     */
    PhysicsObject::PhysicsObject(Vector2D startPosition, float massValue)
        : position(startPosition), velocity(0, 0), acceleration(0, 0),
        angularVelocity(0, 0), angularAcceleration(0, 0),
        mass(massValue), drag(0.1f) {
        
        float radius = 1.0f;
        inertia = (2.0f / 5.0f) * mass * radius * radius;
    }

    /*!
     * \brief Applies a force to the physics object by adding it to the forces manager.
     * \param force The force vector to apply.
     */
    void PhysicsObject::ApplyForce(const Vector2D& forces) {
        auto newForce = std::make_unique<Force>(forces.normalized(), forces.magnitude());
        forcesManager.AddForce(std::move(newForce));
    }

    /*!
     * \brief Applies a drag force proportional to the object's velocity and drag coefficient.
     */
    void PhysicsObject::ApplyDrag() {
        Vector2D dragDirection = -velocity.normalized();
        float dragMagnitude = velocity.magnitude() * drag;  // Change this formula if needed for different drag calculations
        auto dragForce = std::make_unique<Force>(dragDirection, dragMagnitude);
        forcesManager.AddForce(std::move(dragForce));
    }

    /*!
     * \brief Updates the position of the physics object based on its velocity and elapsed time.
     * \param pos Reference to the position vector to update.
     * \param deltaTime The time step for the update.
     */
    void PhysicsObject::UpdatePosition(Vector2D& pos, float deltaTime) {
        pos += velocity * deltaTime;
    }

    /*!
     * \brief Updates the velocity of the physics object based on forces, mass, and elapsed time.
     * \param vel Reference to the velocity vector to update.
     * \param deltaTime The time step for the update.
     */
    void PhysicsObject::UpdateVelocity(Vector2D& vel, float deltaTime) {
        acceleration = forcesManager.SumForces() / mass;
        vel += acceleration * deltaTime;
        forcesManager.Clear();
    }

    /*!
     * \brief Updates the physics object's state, including velocity, position, and drag,
     *        for all entities with RigidBody and Transform components.
     * \param deltaTime The time step for the update.
     */
    void PhysicsObject::Update(float deltaTime) {
        ApplyDrag();

        auto physicsEntities = ecsHub->GetEntitiesWithComponents<RigidBody, Transform>();
        for (auto entity : physicsEntities) {
            auto& rigidbody = ecsHub->GetComponent<RigidBody>(entity);

            UpdateVelocity(rigidbody.velocity, deltaTime);
            UpdatePosition(rigidbody.position, deltaTime);

            /*---------------------------UNCOMMENT THIS IF PLANNING TO USE PHYSICS AND RIGIDBODY---------------------------*/
            //transform.position = rigidbody.position;
        }
    }
} // namespace FruitLoops
