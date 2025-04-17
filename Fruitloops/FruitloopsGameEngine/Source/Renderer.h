#pragma once
/******************************************************************************/
/*!
\file    Renderer.h
\project FruitLoops
\author  Zhang TongYan
\date    April 5, 2025
\brief   Declares the Renderer and SpriteAnimator classes for 2D rendering
         and animation within the FruitLoops engine.

The Renderer class handles batched 2D rendering of quads, lines, text, and
sprites using OpenGL. It efficiently manages GPU buffers, texture slots,
and draw calls to optimise rendering performance across the entire engine.

The SpriteAnimator class is responsible for controlling entity-based
animations using sprite sheets. It tracks frame states, plays sounds per
frame, and renders animated frames tied to game entities.

This file provides public access to drawing primitives, font rendering,
render statistics, grid utilities, and animation management. It is a core
component of the FruitLoops rendering subsystem.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/



namespace FruitLoops {
    class SpriteAnimator  {
    public:
        /*!
        \brief Constructs a default SpriteAnimator.
        Initializes the SpriteAnimator with default values, such as a zero texture ID and frame dimensions.
        */
        SpriteAnimator()
            :  FrameDuration(0.1f), CurrentFrame(0), TimeAccumulator(0.0f) {}


        /*!
        \brief Updates the current frame based on elapsed time.
        Advances the frame in the animation sequence based on the elapsed time since the last update.
        If enough time has passed, the frame is advanced, looping back to the start if necessary.
        \param deltaTime The time elapsed since the last update, in seconds.
        */
        void Update(float deltaTime);
        void ResetAnimation();
        size_t GetTotalFrames() const;
        /*!
        \brief Renders the current frame of the animation.
        Draws the sprite's current frame at the specified position, with the given size and rotation.
        \param position The position where the current frame should be rendered, in 2D space.
        \param size The size (width and height) of the sprite.
        \param rotation The rotation of the sprite in degrees.
        */
        void RenderCurrentFrame(float rotation);

        // setters
        void AddEntity(Entity entity) { 
            auto it = std::find(entityBucket.begin(), entityBucket.end(), entity);
            if (it == entityBucket.end()) {
                entityBucket.push_back(entity);
            }
            
        }
        bool DelEntity(Entity entity) { 
            auto it = std::find(entityBucket.begin(), entityBucket.end(), entity);
            if ( it != entityBucket.end()) {
                entityBucket.erase(it);
                return true;
            }
            return false;
        }
        void SetSpeed(float speed) { animationSpeed = speed; }
        void SetFramesPerRow(const std::vector<int>& FPR) { framesPerRow = FPR; }
        void SetTextureName(const std::string& name) { textureName = name; }
        void SetFrameSize(Vector2D framesize) { frameSize = framesize; }
        void SetPlaying(bool play) { playing = play; }
        void SetLooping(bool loop) { looping = loop; }
        void SetScale(Vector2D scale) { animationScale = scale; }
        void SetPosition(Vector2D pos) { animationPosition = pos; }
        void SetScaleOffset(Vector2D scale) { scaleOffset = scale; }
        void SetPositionOffset(Vector2D pos) { positionOffset = pos; }
        void SetSoundMap(const std::unordered_map<size_t, std::string>& soundmap) { frameSoundMap = soundmap; }

        // getters
        std::vector<Entity> GetEntity() const { return entityBucket; }
        float GetSpeed() const { return animationSpeed; }
        std::vector<int> GetFramesPerRow() const { return framesPerRow; }
        std::string GetTextureName() const { return textureName; }
        Vector2D GetFrameSize() const { return frameSize; }
        bool GetPlaying() const { return playing; }
        bool GetLooping() const { return looping; }
        Vector2D GetScale() const { return animationScale; }
        Vector2D GetPosition() const { return animationPosition; }
        Vector2D GetScaleOffset() const { return scaleOffset; }
        Vector2D GetPositionOffset() const { return positionOffset; }
        std::unordered_map<size_t, std::string> GetSoundMap() const { return frameSoundMap; }

    private:
        std::vector<Entity> entityBucket; //entity to render
        float animationSpeed;  // Adjust as needed
        std::vector<int> framesPerRow;
        std::string textureName;  // Texture ID for the animation
        Vector2D frameSize;
        bool playing;
        bool looping;
        Vector2D animationScale;
        Vector2D animationPosition;
        Vector2D scaleOffset;
        Vector2D positionOffset;
        std::unordered_map<size_t, std::string> frameSoundMap;
        float FrameDuration; // Time (in seconds) each frame is displayed
        size_t CurrentFrame;    // Current frame in the sequence
        float TimeAccumulator; // Accumulated time since the last frame change

        // Private helper functions
        /*!
        \brief Calculates the maximum number of frames in any row of the texture.
        \return The maximum frame count in a single row of the sprite sheet.
        */
        size_t GetMaxFramesInRow() const;

        /*!
        \brief Advances to the next frame in the animation sequence.
        Moves to the next frame, looping back to the first frame if the end is reached.
        */
        void AdvanceFrame();

        /*!
         \brief Calculates the total number of frames in the spritesheet.
         This considers all frames in each row specified in `FramesPerRow`.
         \return The total number of frames in the animation.
         */
        

        /*!
        \brief Gets the row index for a given frame index.
        Determines which row in the sprite sheet the specified frame belongs to.
        \param frameIndex The index of the frame in the sequence.
        \return The row index corresponding to the specified frame.
        */
        size_t GetRowForFrame(size_t frameIndex) const;

        /*!
        \brief Gets the column index for a given frame index and row.
        Determines which column within a row corresponds to a specific frame.
        \param frameIndex The index of the frame in the sequence.
        \param row The row index that contains the frame.
        \return The column index within the row for the specified frame.
        */
        size_t GetColumnForFrame(size_t frameIndex, size_t row) const;

    };

    class Renderer {
    public:

        /*!
        \brief Initializes the renderer.
        Sets up any necessary resources such as shaders, buffers, and OpenGL state.
        */
        static void Init();

        /*!
        \brief Shuts down the renderer.
        Cleans up any resources that were allocated during initialization.
        */
        static void Shutdown();

        /*!
        \brief Begins a new batch for rendering.
        Prepares the batch system for drawing by clearing previous data and
        setting up internal buffers.
        */
        static void BeginBatch();

        /*!
        \brief Ends the current batch.
        Indicates that all drawing operations for this batch have been completed.
        The batch is ready to be flushed to the GPU.
        */
        static void EndBatch();

        /*!
        \brief Flushes the current batch to the GPU.
        Issues a draw call for all queued quads and primitives in the current batch.
        This is typically called after `EndBatch()`.
        */
        static void Flush();

        /*!
        \brief Begins a new batch for rendering.
        Prepares the batch system for drawing by clearing previous data and
        setting up internal buffers.
        */
        static void BeginLineBatch();

        /*!
        \brief Ends the current batch.
        Indicates that all drawing operations for this batch have been completed.
        The batch is ready to be flushed to the GPU.
        */
        static void EndLineBatch();

        /*!
        \brief Flushes the current batch to the GPU.
        Issues a draw call for all queued quads and primitives in the current batch.
        This is typically called after `EndBatch()`.
        */
        static void LineFlush();

        /*!
        \brief Begins a new batch for rendering.
        Prepares the batch system for drawing by clearing previous data and
        setting up internal buffers.
        */
        static void BeginFontBatch();

        /*!
        \brief Ends the current batch.
        Indicates that all drawing operations for this batch have been completed.
        The batch is ready to be flushed to the GPU.
        */
        static void EndFontBatch();

        /*!
        \brief Flushes the current batch to the GPU.
        Issues a draw call for all queued quads and primitives in the current batch.
        This is typically called after `EndBatch()`.
        */
        static void FontFlush();

        /*!
        \brief Draws a quad (rectangle) with the specified parameters.
        Renders a quad at the given position, with the given size, rotation, and color.
        \param position The position of the quad in 2D space.
        \param size The size (width and height) of the quad.
        \param rot The rotation of the quad, in degrees.
        \param color The color of the quad, specified as a vec4 (R, G, B, A).
        */
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const float& rot, const glm::vec4& color);

        /*!
        \brief Draws a textured quad with the specified parameters.
        Renders a quad at the given position, with the given size, rotation, and a texture.
        \param position The position of the quad in 2D space.
        \param size The size (width and height) of the quad.
        \param rot The rotation of the quad, in degrees.
        \param textureID The OpenGL texture ID to apply to the quad.
        */
        //static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const float& rot, uint32_t textureID);

        /*!
        \brief Renders a string of text.
        Draws each character in the specified text string at the given position, scale, and color.
        \param text The string of text to render.
        \param x The x-coordinate for the starting position of the text.
        \param y The y-coordinate for the starting position of the text.
        \param scale The scale factor for the text size.
        \param color The color of the text, specified as a vec3 (R, G, B).
        */
        static void RenderText(const std::string& text, float x, float y, float scale, const std::string& fontIdentifier,
            const Vector3D& color, const float& opacity);

        /*!
        \brief Draws a textured quad with custom UV coordinates.
        Renders a quad at the given position, with the specified size, rotation, and texture coordinates.
        \param position The position of the quad in 2D space.
        \param size The size (width and height) of the quad.
        \param rot The rotation of the quad, in degrees.
        \param textureID The OpenGL texture ID to apply to the quad.
        \param uvmin The bottom-left corner of the UV coordinates for the quad.
        \param uvmax The top-right corner of the UV coordinates for the quad.
        */
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const float& rot, uint32_t textureID, 
            const glm::vec2& uvmin, const glm::vec2& uvmax, const glm::vec4& color);

        /*!
        \brief Draws a textured quad with custom UV coordinates.
        Renders a quad at the given position, with the specified size, rotation, and texture coordinates.
        \param position The position of the quad in 2D space.
        \param size The size (width and height) of the quad.
        \param rot The rotation of the quad, in degrees.
        \param textureID The OpenGL texture ID to apply to the quad.
        \param uvmin The bottom-left corner of the UV coordinates for the quad.
        \param uvmax The top-right corner of the UV coordinates for the quad.
        */
        static void DrawFontQuad(const glm::vec2& position, const glm::vec2& size, const float& rot,
            const glm::vec2& uvMin, const glm::vec2& uvMax, uint32_t textureID, const glm::vec4& color);

        /*!
        \brief Draws a quad (rectangle) with the specified parameters in debug mode.
        Renders a quad at the given position, with the given size, rotation, and color.
        \param position The position of the quad in 2D space.
        \param size The size (width and height) of the quad.
        \param rot The rotation of the quad, in degrees.
        \param color The color of the quad, specified as a vec4 (R, G, B, A).
        */
        static void DrawDebugQuad(const glm::vec2& position, const glm::vec2& size, const float& rot, const glm::vec4& color);

        /*!
        \brief Draws a point at the specified position.
        Renders a point with the given position and color.
        \param position The position of the point in 2D space.
        \param color The color of the point, specified as a vec4 (R, G, B, A).
        */
        static void DrawPoint(const glm::vec2& position, const glm::vec4& color);

        /*!
        \brief Draws a line between two points.
        Renders a line from the start point to the end point with the specified color.
        \param start The starting point of the line in 2D space.
        \param end The ending point of the line in 2D space.
        \param color The color of the line, specified as a vec4 (R, G, B, A).
        */
        static void DrawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color);

        static void DrawGrid(float gridWidth, float gridHeight, float gridSpacing,
            const glm::vec4& color);

        static void DrawArrow(glm::vec2 start, glm::vec2 end, glm::vec4 color, float arrowHeadSize);

        static void ScaleGizmo(const glm::vec2& position, const glm::vec2& size, float handleSize);

        static void DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color, int segments = 32);

        static Vector2D SnapToGrid(const Vector2D& position, float gridSpacing);
        /*!
        \brief Structure holding rendering statistics.
        Tracks the number of draw calls and quads rendered.
        */
        struct Stats {
            uint32_t DrawCount = 0; ///< The number of draw calls issued.
            uint32_t QuadCount = 0; ///< The number of quads rendered.
            uint32_t lineCount = 0; ///< The number of quads rendered.
            uint32_t FontCount = 0; ///< The number of quads rendered.
        };
        /*!
        \brief Retrieves statistics on rendering performance.
        Returns information such as the number of draw calls and quads rendered.
        \return A reference to the Stats structure containing rendering statistics.
        */
        static const Stats& GetStats();

        /*!
        \brief Resets the rendering statistics.
        Clears all counters such as draw calls and quad count, typically called at the start of a new frame.
        */
        static void ResetStats();


    };
}


