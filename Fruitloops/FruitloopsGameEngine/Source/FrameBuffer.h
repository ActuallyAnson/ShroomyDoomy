#pragma once
/******************************************************************************/
/*!
\file    FrameBuffer.h
\project FruitLoops
\author  Gareth Ching
\date    April 5, 2025
\brief   Declares the FrameBuffer class used for off-screen rendering
         within the FruitLoops engine.

This header defines an abstraction for an OpenGL framebuffer object (FBO),
which supports rendering to texture for advanced visual effects and UI overlays.
The class handles the creation, binding, resizing, and destruction of
OpenGL framebuffers, along with their colour texture and depth-stencil attachments.

It exposes a clean interface to:
- Retrieve the framebuffer's texture ID.
- Rescale the framebuffer dynamically (e.g., during window resizing).
- Bind and unbind the framebuffer as the current render target.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

namespace FruitLoops {

/*!
* \class FrameBuffer
* \brief Manages an OpenGL framebuffer object (FBO) with texture and renderbuffer attachments.
* Handles rendering to an off-screen buffer, resizing, and accessing the framebuffer texture.
*/
    class FrameBuffer {
    public:
/*!
* \brief Constructs a FrameBuffer object.
* Initializes OpenGL framebuffer, texture, and renderbuffer for off-screen rendering.
* \param w The initial width of the framebuffer.
* \param h The initial height of the framebuffer.
*/
        FrameBuffer(float w, float h);

/*!
* \brief Destructs a FrameBuffer object.
* Cleans up OpenGL resources such as the framebuffer, texture, and renderbuffer.
*/
        ~FrameBuffer();

/*!
* \brief Retrieves the texture ID associated with the framebuffer.
* \return The OpenGL texture ID of the framebuffer's texture attachment.
*/
        unsigned int getFrameTexture();

/*!
* \brief Rescales the framebuffer to the specified width and height.
* Updates the size of the texture and renderbuffer attachments to match the new dimensions.
* \param newWidth The new width of the framebuffer.
* \param newHeight The new height of the framebuffer.
*/
        void RescaleFrameBuffer(float newWidth, float newHeight);

/*!
* \brief Binds the framebuffer as the current render target.
* Redirects rendering operations to this framebuffer instead of the default framebuffer.
*/
        void Bind() const;

/*!
* \brief Unbinds the framebuffer.
* Restores the default framebuffer as the active render target for rendering operations.
*/
        void Unbind() const;

/*!
* \brief Gets the current width of the framebuffer.
* \return The width of the framebuffer.
*/
        float GetWidth() const { return width; }

/*!
* \brief Gets the current height of the framebuffer.
* \return The height of the framebuffer.
*/
        float GetHeight() const { return height; }

    private:
        unsigned int fbo;
        unsigned int texture;
        unsigned int rbo;

        float width;
        float height;
    };
}