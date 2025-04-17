/**
 * @file Layer.h
 * @author Anson Teng
 * @date  Nov 8, 2024
 * @brief Defines the `Layer` class, representing a renderable and updatable layer in the application.
 *
 * Layers provide a modular way to manage application content, such as UI, game objects, or other
 * elements requiring independent updates and render cycles. Each layer can be updated, rendered, and
 * can handle input events. Layers also have an optional debug name for identification purposes.
 * 
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once

namespace FruitLoops {

    /**
     * @class Layer
     * @brief Represents a single layer within the application, which can handle updates, rendering, and input events.
     */
    class Layer {
    public:
        /**
         * @brief Constructs a Layer with an optional debug name.
         *
         * @param name The name of the layer, primarily used for debugging purposes.
         */
        Layer(const std::string& name = "Layer");

        /**
         * @brief Virtual destructor to allow for inheritance.
         */
        virtual ~Layer();

        /**
         * @brief Initializes the layer. Called when the layer is first attached.
         */
        virtual void OnAttach() {}

        /**
         * @brief Shuts down the layer. Called when the layer is detached.
         */
        virtual void OnDetach() {}

        /**
         * @brief Updates the layer logic every frame.
         *
         * @param dt Delta time since the last frame, used for time-based calculations.
         */
        virtual void OnUpdate(float dt) { dt = 1; }

        /**
         * @brief Renders the layer's content every frame.
         */
        virtual void OnRender() {}

        /**
         * @brief Hides all objects in layer by setting scale to 0.
         */
        virtual void HideObjects() {}

        /**
         * @brief Show all objects in layer by setting scale to back to original scale.
         */
        virtual void ShowObjects() {}

        virtual void UpdateOriginalScale() {}

        /**
         * @brief Function to start storyboard, explicitly for StoryLayer.
         */
        virtual void StartStoryboard() {}


        /**
         * @brief Handles input events for the layer.
         *
         * @return True if the event was handled, false otherwise.
         */
        virtual bool HandleInput() { return false; }

        virtual std::vector<Entity>* GetObjectContainer() = 0;

        /**
         * @brief Retrieves the debug name of the layer.
         *
         * @return A constant reference to the layer's debug name.
         */
        inline const std::string& GetName() const { return debugName; }

        /**
         * @brief Checks if the layer is currently visible.
         *
         * @return True if the layer is visible, false otherwise.
         */
        inline bool IsVisible() const { return isVisible; }

        /**
         * @brief Sets the visibility of the layer.
         *
         * @param visible True to make the layer visible, false to hide it.
         */
        inline bool* SetVisibility() { return &isVisible; }

        /**
         * @brief Sets the opacity of the layer.
         *
         * @param opacity to be set
         */
        inline void SetOpacity(float opacity) { layerOpacity = opacity; }

    protected:
        std::string debugName;  ///< The debug name of the layer, used for identification.
        bool isVisible = true;  ///< Visibility status of the layer.
        float layerOpacity = 1.0f;
    };

}
