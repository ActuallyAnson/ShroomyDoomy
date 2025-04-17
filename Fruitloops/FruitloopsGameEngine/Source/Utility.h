/******************************************************************************/
/*!
\file    Utility.h
\project FruitLoops
\author  Anson Teng
\date    Dec 03, 2024
\brief   This file defines a shared pointer alias (`Ref<T>`) and a utility
         function (`CreateRef`) for convenient and consistent memory
         management throughout the engine.

         - `Ref<T>` is an alias for `std::shared_ptr<T>`, promoting safe
           ownership semantics for dynamically allocated objects.
         - `CreateRef<T>` wraps `std::make_shared<T>`, forwarding constructor
           arguments to simplify usage and enforce best practices in object
           instantiation.

         These abstractions are part of FruitLoops' core utility layer to
         promote clean and efficient code.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once
#include "pch.h"

namespace FruitLoops 
{
	// Define a shared pointer alias
	template<typename T>
	using Ref = std::shared_ptr<T>;

	// Define a utility function to create a shared pointer
	template<typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
