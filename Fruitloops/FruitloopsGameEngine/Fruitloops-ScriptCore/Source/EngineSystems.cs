/******************************************************************************
/*!
\file  EngineSystems.cs
\Proj name  Shroomy Doomy
\author  Tong Yan
\date    Feb 03, 2024
\brief  This file defines the EngineSystems abstract class, which serves as a base
       for engine systems that require initialization and per-frame updates.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;
using System.Runtime.InteropServices;

namespace FruitLoops
{
    /**
     * @brief Abstract base class for engine systems.
     *
     * This class defines the interface for engine systems, requiring them to implement
     * initialization and per-frame update logic.
     */
    public abstract class EngineSystems
	{
        /**
         * @brief Initializes the engine system.
         *
         * This method is called once to set up the system before any updates occur.
         */
        public abstract void Initialize();

        /**
         * @brief Updates the engine system.
         *
         * This method is called every frame to update the system's state.
         *
         * @param deltaTime The time elapsed since the last update, in seconds.
         */
        public abstract void Update(float deltaTime);
	}
}
