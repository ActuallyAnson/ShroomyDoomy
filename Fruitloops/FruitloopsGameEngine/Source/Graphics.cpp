
/****************************************************************************/
/*!
\file		Graphics.cpp
\Proj name  Fruitloops
\author 	Zhang TongYan
\date   	Oct 02, 2024
\brief		Main file for graphics system
All content (C) 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /****************************************************************************/

#include "pch.h"
#include "Graphics.h"
#include "Factory.h"
#include "Input.h"
#include "Log.h"

namespace FruitLoops {

    // Static objects
    Graphics::Camera2D Graphics::camera2d;

    // External pointer for use in other systems
    Graphics* GRAPHICS = NULL;

    // --------------------------------------------------Main Functions-----------------------------------------------------------------

/*!
* \brief Constructor for the Graphics system.
* Initializes the GRAPHICS pointer and registers the Graphics system as an observer.
*/
	Graphics::Graphics()
	{
		GRAPHICS = this;
	}

/*!
* \brief Updates the Graphics system.
* Handles per-frame updates such as maintaining transform data and drawing operations.
* \param dt Delta time since the last frame.
*/
    void Graphics::Update(float dt) {
        update(dt);
        draw();
    }

/*!
* \brief Destructor for the Graphics system.
* Cleans up resources and shuts down the Renderer.
*/
    Graphics::~Graphics() {
       
        GRAPHICS = nullptr;
        Renderer::Shutdown();
    }

/*!
* \brief Initializes the Graphics system.
* Sets up shaders, batch rendering, 2D camera, and sprite animators for entities with AnimationComponent.
*/
    void Graphics::Initialize() {
        logGraphics("Initializing Graphics system...");
        
        // Set the clear color for the graphics system
        logGraphics("Setting clear color to white (1.0, 1.0, 1.0, 1.0)");
        glClearColor(1.f, 1.f, 1.f, 1.f);

        BatchShader = assetsManager->GetShaders("Batch");
        DebugShader = assetsManager->GetShaders("Debug");
        FontShader = assetsManager->GetShaders("Font");

        // Setup Batch Shader
        BatchShader.Use();
        auto loc = glGetUniformLocation(BatchShader.GetHandle(), "u_Textures");

        int samplers[32];
        for (int i = 0; i < 32; i++) {
            samplers[i] = i;
        }

        glUniform1iv(loc, 16, samplers);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        BatchShader.UnUse();

        // Init Font shader stuff
        FontShader.Use();
        auto fontloc = glGetUniformLocation(FontShader.GetHandle(), "fontAtlas");

        int fontsamplers[16];
        for (int i = 0; i < 16; i++) {
            fontsamplers[i] = i;
        }

        glUniform1iv(fontloc, 16, fontsamplers);
        Vector3D color = ConfigData::fontColor;
        glUniform3f(glGetUniformLocation(FontShader.GetHandle(), "textColor"), color.x, color.y, color.z); // Black
        FontShader.UnUse();

        // Init the Renderer and Camera
        Renderer::Init();
        logGraphics("Initializing 2D camera");
        camera2d.init(windowSystem);

        std::vector<std::pair<Entity, std::string>> allObjects = factory->GetAllGameObjects();
        animatorContainer.clear();
        factory->LoadAnimation("../GameData/AnimationContainer.json", animatorContainer);

        logGraphics("Graphics system initialized.");
    }

/*!
* \brief Reinitializes the Graphics system.
* Recreates the sprite animators for all game objects with an AnimationComponent.
*/
    void Graphics::ReInitialize()
    {
        animatorContainer.clear();
        factory->LoadAnimation("../GameData/AnimationContainer.json", animatorContainer);
        
    }

    // Maintain basic transform data here, calculations settled in DrawQuads
/*!
* \brief Updates internal data for graphics-related calculations.
* Logs graphics updates periodically for debugging purposes.
* \param dt Delta time since the last frame.
*/
    void Graphics::update(float dt) {
        static auto lastLoggedTime = std::chrono::high_resolution_clock::now();

        camera2d.update(static_cast<int>(InputSystem::framebufferWindowSize.x), static_cast<int>(InputSystem::framebufferWindowSize.y));
        //std::cout << "Updating camer with x: " << InputSystem::framebufferWindowSize.x << ", y: " << InputSystem::framebufferWindowSize.y << std::endl;

        // Get the current time
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastLoggedTime);

        UNUSED_PARAM(dt);
        
        glViewport(0, 0, static_cast<GLsizei>(InputSystem::framebufferWindowSize.x), static_cast<GLsizei>(InputSystem::framebufferWindowSize.y));
   
    }

/*!
* \brief Handles drawing operations using batch rendering.
* Updates the window title with the current FPS in debug mode.
*/
    void Graphics::draw() {
        #ifdef _DEBUG
        static auto lastDrawLogTime = std::chrono::high_resolution_clock::now();
        InputSystem::update_time(1.0);
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << InputSystem::fps;
        std::string fps = stream.str();
        std::string title = "FruitLoops Engine | FPS: " + fps;
        glfwSetWindowTitle(windowSystem->GetWindow(), title.c_str());
        #endif
    }

    // ----------------------------------------------------------------Utility Functions-----------------------------------------------------------------
/*!
* \brief Sets a 3x3 matrix uniform for the given shader program.
* \param handle Shader program handle.
* \param item Name of the uniform variable in the shader.
* \param mat 3x3 matrix to set as the uniform value.
*/
    void Graphics::SetUniformMat3(GLuint handle, const GLchar* item, glm::mat3& mat) {
        GLint uniformVarLoc = glGetUniformLocation(handle, item);
        if (uniformVarLoc >= 0) {
            glUniformMatrix3fv(uniformVarLoc, 1, GL_FALSE, glm::value_ptr(mat));
        }
        else {
            std::cerr << "Uniform 'u_ViewProj' doesn't exist!!!" << std::endl;
            return;
        }
    }
    // ----------------------------------------------Camera Functions-----------------------------------------------------------------
/*!
* \brief Initializes the 2D camera.
* Sets up the camera entity, computes the initial aspect ratio, and calculates view and projection matrices.
* \param pWindow Pointer to the WindowsWindow object.
*/
    void Graphics::Camera2D::init(WindowsWindow* pWindow) {

        auto cameraEntity = ecsHub->GetEntitiesWithComponents<CameraComponent>();
        for (auto cam : cameraEntity) {
            auto& transform = ecsHub->GetComponent<Transform>(cam);
            auto& cameraComponent = ecsHub->GetComponent<CameraComponent>(cam);

            static GLsizei fb_width, fb_height;
            glfwGetFramebufferSize(pWindow->GetWindow(), &fb_width, &fb_height);
            cameraComponent.ar = static_cast<GLfloat>(fb_width) / fb_height;

            // compute camera's up and right vectors ...
            glm::mat3 rot(
                cos(transform.orientation.x * (M_PI / 180.f)), sin(transform.orientation.x * (M_PI / 180.f)), 0.f,
                -sin(transform.orientation.x * (M_PI / 180.f)), cos(transform.orientation.x * (M_PI / 180.f)), 0.f,
                0.f, 0.f, 1.0f
            );

            up = glm::vec2(-sin(transform.orientation.x * (M_PI / 180.f)), cos(transform.orientation.x * (M_PI / 180.f)));
            right = glm::vec2(cos(transform.orientation.x * (M_PI / 180.f)), sin(transform.orientation.x * (M_PI / 180.f)));

            // at startup, the camera must be initialized to free camera
            viewTransformMat = glm::mat3(
                1, 0, 0,
                0, 1, 0,
                -transform.position.x, -transform.position.x, 1
            );

            cameraWindowToNDC = {

                2.0f / (cameraComponent.ar * cameraComponent.height), 0 ,0,
                0, 2.0f / cameraComponent.height, 0,
                0, 0, 1
            };
            worldToNDCMatrix = cameraWindowToNDC * viewTransformMat;
        }
    }

/*!
* \brief Updates the 2D camera's transformation and projection matrices.
* Handles rotation, zoom, and view matrix updates based on user input and camera component flags.
* \param framebufferWidth Width of the framebuffer.
* \param framebufferHeight Height of the framebuffer.
*/
    void Graphics::Camera2D::update(int framebufferWidth, int framebufferHeight) {
        // Access camera entities
        auto cameraEntity = ecsHub->GetEntitiesWithComponents<CameraComponent>();
        for (auto cam : cameraEntity) {
            auto& transform = ecsHub->GetComponent<Transform>(cam);
            auto& cameraComponent = ecsHub->GetComponent<CameraComponent>(cam);

            // Handle camera rotation
            if (cameraComponent.leftTurnFlag) {
                transform.orientation.x += transform.orientation.y;
            }
            else if (cameraComponent.rightTurnFlag) {
                transform.orientation.x -= transform.orientation.y;
            }

            // Update 'up' and 'right' vectors based on orientation
            float orientationRadians = glm::radians(transform.orientation.x);
            up = glm::vec2(-sin(orientationRadians), cos(orientationRadians));
            right = glm::vec2(cos(orientationRadians), sin(orientationRadians));
          

            // Handle camera zoom
            if (cameraComponent.zoomFlag) {
                cameraComponent.height += cameraComponent.heightChangeDir * cameraComponent.heightChangeVal;
                if (cameraComponent.height >= cameraComponent.maxHeight || cameraComponent.height <= cameraComponent.minHeight) {
                    cameraComponent.heightChangeDir *= -1;
                }
            }

            // Zoom using mouse scroll
            static GLint prevMousescroll = 0;
            if (prevMousescroll != InputSystem::mouseScrollY) {
                cameraComponent.height -= cameraComponent.heightChangeVal * (InputSystem::mouseScrollY - prevMousescroll);
                if (cameraComponent.height > cameraComponent.maxHeight) {
                    cameraComponent.height = cameraComponent.maxHeight;
                }
                else if (cameraComponent.height < cameraComponent.minHeight) {
                    cameraComponent.height = cameraComponent.minHeight;
                }
                prevMousescroll = InputSystem::mouseScrollY;
            }

            // Compute the view transformation matrix
            if (cameraComponent.camTypeFlag) {
                GLfloat x = -right.x * transform.position.x + -right.y * transform.position.y;
                GLfloat y = -up.x * transform.position.x + -up.y * transform.position.y;
                viewTransformMat = glm::mat3(
                    right.x, up.x, 0.0f,
                    right.y, up.y, 0.0f,
                    x, y, 1.0f
                );
            }
            else {
                viewTransformMat = glm::mat3(
                    1.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    -transform.position.x, -transform.position.y, 1.0f
                );
            }

            // Use the framebuffer dimensions to compute the aspect ratio
            cameraComponent.ar = static_cast<GLfloat>(framebufferWidth) / framebufferHeight;

            // Compute the camera's projection matrix (camera window to NDC)
            cameraWindowToNDC = glm::mat3(
                2.0f / (cameraComponent.ar * cameraComponent.height), 0.0f, 0.0f,
                0.0f, 2.0f / cameraComponent.height, 0.0f,
                0.0f, 0.0f, 1.0f
            );

            // Compute the world to NDC transformation matrix
            worldToNDCMatrix = cameraWindowToNDC * viewTransformMat;
        }

    }
  
}