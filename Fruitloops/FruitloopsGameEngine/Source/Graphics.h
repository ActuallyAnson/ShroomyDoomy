#pragma once
/****************************************************************************/
/*!
\file       Graphics.h
\Proj name  Fruitloops
\author     Zhang TongYan
\date       Oct 02, 2024
\brief      Header file for graphics system.
            This file declares the Graphics system for the FruitLoops project.
            It includes functionality for managing shaders, camera operations,
            texture objects, and rendering.
All content (C) 2024 DigiPen Institute of Technology Singapore. All rights reserved.
 */
 /****************************************************************************/

#include "System.h"
#include "WindowsWindow.h"
#include "Collision.h"
#include "ShaderManager.h"
#include "AssetsManager.h"
#include "Renderer.h"

namespace FruitLoops {

    class Graphics : public EngineSystems {
    public:

        /*!
        \brief Constructor for the Graphics system.
        Initializes any required member variables and prepares the system for use.
        */
        Graphics();

        /*!
        \brief Destructor for the Graphics system.
        Releases any allocated resources and cleans up.
        */
        ~Graphics();

        /*!
        \brief Initializes the graphics system.
        Sets up necessary components, including OpenGL initialization, shaders, and textures.
        */
        void Initialize() override;

        void ReInitialize();
        /*!
        \brief Updates the graphics system.
        Handles per-frame updates such as camera transformations and rendering logic.
        \param dt The delta time (time since the last update) in seconds.
        */
        void Update(float dt) override;

        /*!
        \brief Returns the name of this system for identification purposes.
        \return The string "Graphics".
        */
        virtual std::string GetSystemName() { return "Graphics"; }

        /*!
        \brief Structure representing a vertex in 2D space.
        Contains position and texture coordinates for rendering purposes.
        */
        struct Vertex {
            glm::vec2 position;  ///< The 2D position of the vertex.
            glm::vec2 texCoords; ///< The texture coordinates of the vertex.
        };

        /*!
        \brief Structure representing a 2D camera.
        Handles camera transformations and updates for 2D rendering.
        */
        struct Camera2D {
            glm::vec2 right, up;                  ///< Camera direction vectors.
            glm::mat3 viewTransformMat;                 ///< The camera's view matrix.
            glm::mat3 cameraWindowToNDC;        ///< Transformation from camera window to normalized device coordinates (NDC).
            glm::mat3 worldToNDCMatrix;         ///< Transformation from world space to NDC.

            //Vector2D position;  // Add this line to represent the camera's position in world space
            //float height;       // Height of the camera view, for scaling

            /*!
            \brief Initializes the 2D camera.
            Sets up initial camera parameters based on the given window dimensions.
            \param window A pointer to the window for retrieving size information.
            */
            void init(WindowsWindow* window);

            /*!
            \brief Updates the 2D camera.
            Recalculates the camera's transformations based on any changes to the window or camera properties.
            \param window A pointer to the window for retrieving size information.
            */
            void update(int framebufferWidth, int framebufferHeight);
        };

        /*!
        \brief Sets a uniform matrix value in a shader.
        Sends a 3x3 matrix to a shader's uniform variable.
        \param program The OpenGL shader program ID.
        \param name The name of the uniform variable in the shader.
        \param matrix The 3x3 matrix to send to the shader.
        */
        void SetUniformMat3(GLuint program, const GLchar* name, glm::mat3& matrix);

    private:

        /*!
        \brief Internal update function for the graphics system.
        Handles updating logic such as camera transformations and buffer updates.
        \param dt The delta time (time since the last update) in seconds.
        */
        void update(float dt);

        /*!
        \brief Draw function for rendering objects to the screen.
        Responsible for issuing OpenGL draw calls and handling shaders.
        */
        void draw();

        

    public:
        static Camera2D camera2d;                          ///< The 2D camera used for rendering.
        ShaderManager BatchShader;
        ShaderManager DebugShader;
        ShaderManager FontShader;
        std::vector<SpriteAnimator> animatorContainer;
    };

    /*!
    \brief A global pointer to the Graphics system.
    Used to access the graphics system from other parts of the engine.
    */
    extern Graphics* GRAPHICS;
}