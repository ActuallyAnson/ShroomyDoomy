/**
 * @file Layer.cpp
 * @author Anson Teng
 * @date  Nov 8, 2024
 * @brief A simple place holder for the layer class header
 *
 * Layers provide a modular way to manage application content, such as UI, game objects, or other
 * elements requiring independent updates and render cycles. Each layer can be updated, rendered, and
 * can handle input events. Layers also have an optional debug name for identification purposes.
 *
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "pch.h"
#include "Layer.h"
//layer.cpp

namespace FruitLoops {
	Layer::Layer(const std::string& name)
		: debugName(name), isVisible()
	{
	}
	Layer::~Layer()
	{
	}
}
