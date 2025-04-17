/**
 * @file LayerStack.h
 * @author Anson Teng
 * @date  Nov 8, 2024
 * @brief Defines the LayerStack class for managing and rendering layers in an application.
 *
 * The `LayerStack` class provides functionality to manage a stack of layers,
 * where each layer is responsible for different tasks such as handling input, updating, and rendering.
 * Layers are stacked in the order they are pushed, with overlays added at the end.
 * The class supports operations to add or remove layers, and to update, render, and handle events in all layers.
 * Layers are processed in the order they are added to the stack.
 * 
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once

//layerstack.h
#include "Layer.h"

namespace FruitLoops {
	/**
   * @class LayerStack
   * @brief Manages a stack of layers.
   *
   * This class provides functionality for managing layers in the application.
   * Layers can be pushed and popped onto the stack. The stack is ordered,
   * with layers added first being at the bottom. Layers are updated, rendered,
   * and handle events in the order they are stacked.
   */
	class LayerStack {
	public:
        /**
        * @brief Default constructor.
        */
        LayerStack() = default;

        /**
         * @brief Destructor to clean up all layers.
         */
        ~LayerStack();

        /**
         * @brief Pushes a layer onto the stack.
         * @param layer The layer to push onto the stack.
         */
        void PushLayer(Layer* layer);

        /**
         * @brief Pushes an overlay onto the stack.
         * @param overlay The overlay to push onto the stack.
         */
        void PushOverlay(Layer* overlay);

        /**
         * @brief Pops a layer from the stack.
         * @param layer The layer to pop from the stack.
         */
        void PopLayer(Layer* layer);

        /**
         * @brief Pops an overlay from the stack.
         * @param overlay The overlay to pop from the stack.
         */
        void PopOverlay(Layer* overlay);

        /**
         * @brief Updates all layers in the stack.
         * @param dt The delta time between frames.
         */
        void updateLayers(float dt);

        /**
         * @brief Renders all layers in the stack.
         */
        void renderLayers();

        /**
         * @brief Handles events for the layers in the stack.
         *
         * Events are handled starting from the top layer down to the bottom layer.
         */
        void handleEvents();

        /**
          * @brief Gets an iterator to the beginning of the layer stack.
          * @return An iterator to the first layer in the stack.
          */

		std::vector<Layer*>::iterator begin() { return layers.begin(); }

        /**
         * @brief Gets an iterator to the end of the layer stack.
         * @return An iterator to one past the last layer in the stack.
         */
		std::vector<Layer*>::iterator end() { return layers.end(); }

        /**
         * @brief Gets a reverse iterator to the beginning of the layer stack.
         * @return A reverse iterator to the last layer in the stack.
         */
		std::vector<Layer*>::reverse_iterator rbegin() { return layers.rbegin(); }

        /**
        * @brief Gets a reverse iterator to the end of the layer stack.
        * @return A reverse iterator to one before the first layer in the stack.
        */
		std::vector<Layer*>::reverse_iterator rend() { return layers.rend(); }

        /**
       * @brief Gets a constant iterator to the beginning of the layer stack.
       * @return A constant iterator to the first layer in the stack.
       */
		std::vector<Layer*>::const_iterator begin() const { return layers.begin(); }

        /**
        * @brief Gets a constant iterator to the end of the layer stack.
        * @return A constant iterator to one past the last layer in the stack.
        */
		std::vector<Layer*>::const_iterator end()	const { return layers.end(); }

        /**
       * @brief Gets a constant reverse iterator to the beginning of the layer stack.
       * @return A constant reverse iterator to the last layer in the stack.
       */
		std::vector<Layer*>::const_reverse_iterator rbegin() const { return layers.rbegin(); }

        /**
        * @brief Gets a constant reverse iterator to the end of the layer stack.
        * @return A constant reverse iterator to one before the first layer in the stack.
        */
		std::vector<Layer*>::const_reverse_iterator rend() const { return layers.rend(); }

        void Reinitialize();

	private:

        /**
        * @brief Finds a specific layer in the stack.
        *
        * Searches for the specified layer either within the range of regular layers
        * or overlays based on the `withinNonOverlays` flag.
        *
        * @param layer A pointer to the layer to find.
        * @param withinNonOverlays If true, searches only within regular layers;
        *                          if false, searches within overlays.
        * @return An iterator pointing to the found layer, or `end()` if not found.
        */
        std::vector<Layer*>::iterator FindLayer(Layer* layer, bool withinNonOverlays);

        /**
         * @brief Detaches and removes the layer at the specified iterator position.
         *
         * Calls `OnDetach` on the layer, erases it from the stack, and adjusts the
         * insertion index if necessary.
         *
         * @param layerIt An iterator pointing to the layer to be detached and removed.
         */
        void DetachLayer(std::vector<Layer*>::iterator layerIt);

		std::vector<Layer*> layers;
		//std::vector<Layer*>::iterator layersInsert;
		unsigned int m_LayerInsertIndex = 0;

	};
}