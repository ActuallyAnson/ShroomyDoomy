/******************************************************************************
/*!
\file  GameLogicSystem.cs
\Proj name  Shroomy Doomy
\author  Benjamin
\date    Feb 03, 2024
\brief  This file implements the GameLogicSystem class, which manages the lifecycle
       and update cycle of game logic behaviours. It is responsible for initializing,
       updating, ending, and resetting the system's behaviours.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;
using System.Collections.Generic;

namespace FruitLoops
{
    /**
     * @brief Manages the lifecycle and update cycle of game logic behaviours.
     *
     * The GameLogicSystem is implemented as a singleton that maintains lists of behaviour
     * function delegates and their corresponding behaviour components. It is responsible for
     * initializing, updating, ending, and resetting game logic behaviours.
     */
    public class GameLogicSystem
    {
        private static GameLogicSystem _instance;

        private static bool gameActive = false;

        /**
         * @brief Gets the singleton instance of the GameLogicSystem.
         *
         * If the instance does not exist, a new instance is created.
         *
         * @return The singleton instance of GameLogicSystem.
         */
        public static GameLogicSystem Instance
        {
            get
            {
                if (_instance == null)
                    _instance = new GameLogicSystem();
                return _instance;
            }
        }

        private readonly List<BehaviourFCT> _behaviours = new List<BehaviourFCT>();
        private readonly List<BehaviourComponent> _behaviourComponents = new List<BehaviourComponent>();
        private readonly List<BehaviourFCT> _initialBehaviours = new List<BehaviourFCT>();

        public IReadOnlyList<BehaviourFCT> Behaviours => _behaviours;

        private GameLogicSystem() { }

        /**
         * @brief Initializes the GameLogicSystem.
         *
         * Marks the game as active and calls the Init method on all registered behaviours.
         */
        public void Init()
        {
            gameActive = true;

            foreach (var behaviour in _behaviours)
            {
                behaviour.Init();
            }
            Debug.LogInfo("GameLogicSystem Initialized");
        }

        /**
         * @brief Updates all registered behaviour components.
         *
         * Iterates through each behaviour component and calls its associated behaviour's
         * Update method using the provided delta time.
         *
         * @param deltaTime The time elapsed since the last update (in seconds).
         */
        public void Update(float deltaTime)
        {
            if (gameActive == true)
            {
                foreach (var component in _behaviourComponents)
                {
                    try
                    {
                        var behaviour = _behaviours[component.GetBehaviourIndex()];
                        behaviour.Update(deltaTime);
                    }
                    catch (Exception ex)
                    {
                        Debug.LogError($"Error during update of behaviour component: {ex.Message}");
                    }
                }
            }
        }

        /**
         * @brief Ends the game logic system.
         *
         * Calls the End method on all registered behaviours, clears all behaviour lists,
         * marks the game as inactive, and logs the termination.
         */
        public void End()
        {
            for (int i = 0; i < _behaviours.Count; i++)
            {
                var behaviour = _behaviours[i];
                Debug.LogInfo($"Next behaviour to be cleared: {behaviour.GetType().Name} for entity ID: {behaviour.Owner.ID}");
                behaviour.End();
            }
            _behaviours.Clear();
            _behaviourComponents.Clear();

            gameActive = false;
            Debug.LogError("End Function finished running");
        }

        /**
         * @brief Adds a behaviour function delegate to the system.
         *
         * Also stores the initial behaviour if it has not already been added.
         *
         * @param behaviour The behaviour function delegate to add.
         */
        public void AddBehaviour(BehaviourFCT behaviour)
        {
            _behaviours.Add(behaviour);

            // Store initial behaviors if not already added
            if (!_initialBehaviours.Contains(behaviour))
            {
                _initialBehaviours.Add(behaviour);
            }
        }

        /**
        * @brief Registers a behaviour component with the system.
        *
        * @param component The behaviour component to register.
        */
        public void RegisterLogicComponent(BehaviourComponent component)
        {
            _behaviourComponents.Add(component);
        }

        /**
         * @brief Retrieves a behaviour component of the specified type for a given entity.
         *
         * Iterates through the registered behaviour components and returns the one that matches
         * the specified type and whose owner has the provided entity ID.
         *
         * @tparam T The type of the behaviour component.
         * @param entity The entity whose behaviour component is requested.
         * @return The behaviour component of type T if found; otherwise, null.
         */
        public T GetBehaviour<T>(Entity entity) where T : BehaviourComponent
        {
            foreach (var component in _behaviourComponents)
            {
                if (component is T behaviour && behaviour.GetOwner().ID == entity.ID)
                {
                    return behaviour;
                }
            }
            return null;
        }

        /**
         * @brief Resets the GameLogicSystem to its initial state.
         *
         * Ends the current game logic, restores the initial behaviours, and re-initializes the system.
         */
        public void ResetToInitial()
        {
            End();

            foreach (var behaviour in _initialBehaviours)
            {
                _behaviours.Add(behaviour);
            }

            Init();
        }
    }
}