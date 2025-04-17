/**
 * @file LayerStack.cpp
 * @author Anson Teng
 * @date  Nov 8, 2024
 * @brief Implements the functionality of the `LayerStack` class.
 *
 * This file contains the implementation of the `LayerStack` class, which manages a stack of layers.
 * Layers in the stack can be pushed or popped, and they are updated and rendered in sequence.
 * The stack is also responsible for handling events, where events are passed from the topmost layer to the bottommost layer.
 *
 * Each layer is processed during the application's update and render loops, and events are handled from top to bottom layers
 * to ensure proper input handling.
 * 
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#include "pch.h"
#include "LayerStack.h"
#include "Core.h"
namespace FruitLoops {

    /**
     * @brief Destructor for LayerStack.
     *
     * Cleans up all layers in the stack by calling `OnDetach` on each layer
     * and then deleting it.
     */
    LayerStack::~LayerStack() {
        for (Layer* layer : layers) {
            layer->OnDetach();
            delete layer;
        }
        m_LayerInsertIndex = 0;
        //layers.clear();
    }

    /**
     * @brief Adds a new layer to the stack.
     *
     * Inserts the layer at the current insertion index, which separates layers from overlays.
     * The index is incremented after insertion.
     *
     * @param layer A pointer to the layer to be added to the stack.
     */
    void LayerStack::PushLayer(Layer* layer) {
        if (layers.empty()) {
            layers.push_back(layer); // Direct push when empty
        }
        else {
            layers.emplace(layers.begin() + m_LayerInsertIndex, layer);
        }
        m_LayerInsertIndex++;
    }

    /**
     * @brief Adds an overlay to the stack.
     *
     * Overlays are inserted at the end of the stack, allowing them to be rendered and updated
     * after regular layers. They remain on top of regular layers for event handling.
     *
     * @param overlay A pointer to the overlay to be added to the stack.
     */
    void LayerStack::PushOverlay(Layer* overlay) {
        layers.push_back(overlay);
    }

    /**
     * @brief Removes a specified layer from the stack.
     *
     * Searches within the range of non-overlay layers for the specified layer.
     * If found, the layer is detached and removed, and the insertion index is decremented.
     *
     * @param layer A pointer to the layer to be removed from the stack.
     */
    void LayerStack::PopLayer(Layer* layer) {
        auto it = FindLayer(layer, true);
        if (it != layers.begin() + m_LayerInsertIndex) {
            DetachLayer(it);
        }
    }

    /**
     * @brief Removes a specified overlay from the stack.
     *
     * Searches for the overlay within the overlay section of the stack.
     * If found, the overlay is detached and removed.
     *
     * @param overlay A pointer to the overlay to be removed from the stack.
     */
    void LayerStack::PopOverlay(Layer* overlay) {
        auto it = FindLayer(overlay, false);
        if (it != layers.end()) {
            DetachLayer(it);
        }
    }

    /**
     * @brief Updates all layers in the stack.
     *
     * Calls the `OnUpdate` function for each layer in the stack, passing in the
     * provided delta time. This allows each layer to perform per-frame updates.
     *
     * @param dt The delta time used for updating layers.
     */
    void LayerStack::updateLayers(float dt) {
        for (Layer* layer : layers) {
            layer->OnUpdate(dt);
        }
    }

    /**
     * @brief Renders all layers in the stack.
     *
     * Calls the `OnRender` function for each layer, allowing them to render
     * their contents sequentially.
     */
    void LayerStack::renderLayers() {
        for (Layer* layer : layers) {
            if (layer->IsVisible() == true) {
                layer->OnRender();
            }
            
        }
    }

    /**
     * @brief Handles input events by passing them to layers from top to bottom.
     *
     * Calls `HandleInput` on each layer, starting from the topmost layer. If a layer
     * successfully handles an event, propagation is stopped.
     */
    void LayerStack::handleEvents() {
        for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
            if ((*it)->HandleInput()) {
                break;  // Stop event propagation if handled
            }
        }
    }

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
    std::vector<Layer*>::iterator LayerStack::FindLayer(Layer* layer, bool withinNonOverlays) {
        if (withinNonOverlays) {
            return std::find(layers.begin(), layers.begin() + m_LayerInsertIndex, layer);
        }
        else {
            return std::find(layers.begin() + m_LayerInsertIndex, layers.end(), layer);
        }
    }

    /**
     * @brief Detaches and removes the layer at the specified iterator position.
     *
     * Calls `OnDetach` on the layer, erases it from the stack, and adjusts the
     * insertion index if necessary.
     *
     * @param layerIt An iterator pointing to the layer to be detached and removed.
     */
    void LayerStack::DetachLayer(std::vector<Layer*>::iterator layerIt) {
        (*layerIt)->OnDetach();
        layers.erase(layerIt);
        m_LayerInsertIndex--;
    }

    void LayerStack::Reinitialize() {
        // Step 1: Clean up existing layers
        for (Layer* layer : layers) {
            if (layer) {
                layer->OnDetach(); // Ensure proper detachment
                delete layer;      // Deallocate memory
            }
        }

        layers.clear();          // Clear the vector
        m_LayerInsertIndex = 0;  // Reset the insertion index

#ifdef _DEBUG
        spdlog::info("LayerStack cleared and ready for reinitialization.");
#endif

        // Step 2: Reinitialize layers (this step may vary based on how layers are created)
        // Example: Adding default layers
        coreEngine->InitializeLayers();

#ifdef _DEBUG
        spdlog::info("LayerStack reinitialized with {} layers.", layers.size());
#endif
    }
} // namespace FruitLoops
