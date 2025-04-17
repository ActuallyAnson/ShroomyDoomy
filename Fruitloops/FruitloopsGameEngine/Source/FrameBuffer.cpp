/******************************************************************************/
/*!
\file    FrameBuffer.cpp
\project FruitLoops
\author  Gareth Ching
\date    April 5, 2025
\brief   Defines the FrameBuffer class responsible for off-screen rendering
         in the FruitLoops engine using OpenGL.

This file implements functionality for creating, managing, and resizing
OpenGL framebuffer objects (FBOs), including the setup of associated
color textures and depth-stencil renderbuffers. These are used to
support post-processing, render-to-texture effects, and off-screen
scene composition in the rendering pipeline.

Key features include:
- Creation and cleanup of OpenGL FBO resources.
- Dynamic rescaling of framebuffers to support window resizing.
- Access to the texture for sampling rendered content elsewhere.
- Bind and unbind utilities for rendering to/from the framebuffer.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "FrameBuffer.h"

namespace FruitLoops {
/*!
* \brief Constructs a FrameBuffer object.
* Initializes OpenGL framebuffer, texture, and renderbuffer for rendering.
* \param w The width of the framebuffer.
* \param h The height of the framebuffer.
*/
    FrameBuffer::FrameBuffer(float w, float h)
        : width(w), height(h)
    {
        
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

/*!
* \brief Destructs a FrameBuffer object.
* Cleans up OpenGL framebuffer, texture, and renderbuffer resources.
*/
    FrameBuffer::~FrameBuffer()
    {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &texture);
        glDeleteRenderbuffers(1, &rbo);
    }

/*!
* \brief Retrieves the texture ID of the framebuffer.
* \return The texture ID associated with the framebuffer.
*/
    unsigned int FrameBuffer::getFrameTexture()
    {
        return texture;
    }

/*!
* \brief Rescales the framebuffer to a new width and height.
* Resizes the texture and renderbuffer attachments, updates OpenGL viewport.
* \param newWidth The new width for the framebuffer.
* \param newHeight The new height for the framebuffer.
*/
    void FrameBuffer::RescaleFrameBuffer(float newWidth, float newHeight)
    {
        // Update stored width and height
        width = newWidth;
        height = newHeight;

        // Bind the framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // Resize the texture attachment
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Resize the renderbuffer attachment
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Reattach the texture and renderbuffer to the framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        // Unbind the framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

/*!
* \brief Binds the framebuffer for rendering.
* Makes the framebuffer the active render target in OpenGL.
*/
    void FrameBuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

/*!
* \brief Unbinds the framebuffer.
* Restores the default framebuffer as the active render target in OpenGL.
*/
    void FrameBuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


}