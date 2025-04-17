/******************************************************************************/
/*!
\file    Renderer.cpp
\project FruitLoops
\author  Zhang TongYan
\date    April 5, 2025
\brief   Defines the Renderer and SpriteAnimator classes for 2D rendering
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
#include "pch.h"
#include "Renderer.h"
#include "AssetsManager.h"

namespace FruitLoops {

	// Constants for rendering limits
	static const size_t maxQuadCount = 1000;				// Maximum number of quads to be rendered in one batch
	static const size_t maxVertexCount = maxQuadCount * 4;  // Each quad has 4 vertices
	static const size_t maxindexCount = maxQuadCount * 6;   // Each quad has 6 indices (2 triangles)
	static const size_t maxTextures = 32;					// Maximum number of textures that can be bound at once
	static const size_t maxlineCount = 1000;
	static const size_t maxLineVertexCount = maxlineCount * 2;

	struct Vertex {
		glm::vec2 position;
		glm::vec4 color;
		glm::vec2 texCoords;
		float texIndex;
	};

	// Struct to hold all the renderer data
	struct RendererData {
		GLuint quadVA = 0;	// Vertex Array Object
		GLuint quadVB = 0;	// Vertex Buffer Object
		GLuint quadIB = 0;	// Index Buffer Object

		GLuint fontVA = 0;	// Vertex Array Object
		GLuint fontVB = 0;	// Vertex Buffer Object
		GLuint fontIB = 0;	// Index Buffer Object

		GLuint lineVA = 0;	// Vertex Array Object
		GLuint lineVB = 0;	// Vertex Buffer Object

		// White texture used for untextured quads
		GLuint whiteTexture = 0;
		uint32_t whiteTextureSlot = 0;

		uint32_t indexCount = 0;
		uint32_t lineCount = 0;
		uint32_t fontIndexCount = 0;

		Vertex* quadBuffer = nullptr;		// Pointer to the vertex buffer
		Vertex* quadBufferPtr = nullptr;	// Pointer to the current position in the buffer

		Vertex* fontBuffer = nullptr;		// Pointer to the vertex buffer
		Vertex* fontBufferPtr = nullptr;	// Pointer to the current position in the buffer

		Vertex* lineBuffer = nullptr;
		Vertex* lineBufferPtr = nullptr;

		std::array<uint32_t, maxTextures> textureSlots;
		uint32_t textureSlotIndex = 1;

		Renderer::Stats renderStats;		// Renderer statistics (draw calls, quad count, etc.)
	};

	// Global instance of renderer data
	static RendererData renderData;


	// --------------------------------------------------------------------------- Renderer Functions -----------------------------------------------------------------------------------
	/*!
	\brief Initializes all rendering buffers and prepares necessary resources.
	*/
	void Renderer::Init() {
		// Allocate space for quad vertices in the buffer
		renderData.quadBuffer = new Vertex[maxVertexCount];
		renderData.lineBuffer = new Vertex[maxLineVertexCount];
		renderData.fontBuffer = new Vertex[maxVertexCount];

		// ---- Quad VAO and VBO setup ----
		glCreateVertexArrays(1, &renderData.quadVA);
		glBindVertexArray(renderData.quadVA);

		glCreateBuffers(1, &renderData.quadVB);
		glBindBuffer(GL_ARRAY_BUFFER, renderData.quadVB);
		glBufferData(GL_ARRAY_BUFFER, maxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexArrayAttrib(renderData.quadVA, 0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

		glEnableVertexArrayAttrib(renderData.quadVA, 1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

		glEnableVertexArrayAttrib(renderData.quadVA, 2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

		glEnableVertexArrayAttrib(renderData.quadVA, 3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texIndex));

		// Unbind Quad VAO and VBO
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		// ---- Line VAO and VBO setup ----
		glCreateVertexArrays(1, &renderData.lineVA);
		glBindVertexArray(renderData.lineVA);

		glCreateBuffers(1, &renderData.lineVB);
		glBindBuffer(GL_ARRAY_BUFFER, renderData.lineVB);
		glBufferData(GL_ARRAY_BUFFER, maxLineVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexArrayAttrib(renderData.lineVA, 0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

		glEnableVertexArrayAttrib(renderData.lineVA, 1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

		// Unbind Line VAO and VBO
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// ---- Font VAO and VBO setup ----
		glCreateVertexArrays(1, &renderData.fontVA);
		glBindVertexArray(renderData.fontVA);

		glCreateBuffers(1, &renderData.fontVB);
		glBindBuffer(GL_ARRAY_BUFFER, renderData.fontVB);
		glBufferData(GL_ARRAY_BUFFER, maxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexArrayAttrib(renderData.fontVA, 0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

		glEnableVertexArrayAttrib(renderData.fontVA, 1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

		glEnableVertexArrayAttrib(renderData.fontVA, 2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

		glEnableVertexArrayAttrib(renderData.fontVA, 3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texIndex));


		// Unbind Font VAO and VBO
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Create and initialize the index buffer (6 indices per quad, for two triangles)
		uint32_t indices[maxindexCount];
		uint32_t offset = 0;
		for (int i = 0; i < maxindexCount; i += 6) {
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;

			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

		glCreateBuffers(1, &renderData.quadIB);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData.quadIB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glCreateBuffers(1, &renderData.fontIB);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData.fontIB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Create and initialize a default white texture (1x1 texture)
		glCreateTextures(GL_TEXTURE_2D, 1, &renderData.whiteTexture);
		glBindTexture(GL_TEXTURE_2D, renderData.whiteTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		uint32_t color = 0xffffffff;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

		// Assign the white texture to the first texture slot
		renderData.textureSlots[0] = renderData.whiteTexture;
		for (size_t i = 1; i < maxTextures; i++) {
			renderData.textureSlots[i] = 0;
		}
		glBindVertexArray(0);
	}

	/*!
	\brief Cleans up all rendering resources and buffers.
	*/
	void Renderer::Shutdown() {
		glDeleteVertexArrays(1, &renderData.quadVA);
		glDeleteBuffers(1, &renderData.quadVB);
		glDeleteBuffers(1, &renderData.quadIB);

		glDeleteVertexArrays(1, &renderData.fontVA);
		glDeleteBuffers(1, &renderData.fontVB);
		glDeleteBuffers(1, &renderData.fontIB);

		glDeleteVertexArrays(1, &renderData.lineVA);
		glDeleteBuffers(1, &renderData.lineVB);

		glDeleteTextures(1, &renderData.whiteTexture);
		delete[] renderData.quadBuffer;
		delete[] renderData.lineBuffer;
		delete[] renderData.fontBuffer;
	}

	/*!
	\brief Begins a new rendering batch for quads.
	*/
	void Renderer::BeginBatch() {
		renderData.quadBufferPtr = renderData.quadBuffer;

	}

	/*!
	\brief Begins a new rendering batch for lines.
	*/
	void Renderer::BeginLineBatch() {
		renderData.lineBufferPtr = renderData.lineBuffer;
	}

	/*!
	\brief Begins a new rendering batch for fonts.
	*/
	void Renderer::BeginFontBatch() {
		renderData.fontBufferPtr = renderData.fontBuffer;

	}

	/*!
	\brief Ends the current batch of quads and uploads vertex data to the GPU.
	*/
	void Renderer::EndBatch() {
		GLsizeiptr size = (uint8_t*)renderData.quadBufferPtr - (uint8_t*)renderData.quadBuffer;

		glBindBuffer(GL_ARRAY_BUFFER, renderData.quadVB);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, renderData.quadBuffer);

	}

	/*!
	\brief Ends the current batch of lines and uploads vertex data to the GPU.
	*/
	void Renderer::EndLineBatch() {
		GLsizeiptr size = (uint8_t*)renderData.lineBufferPtr - (uint8_t*)renderData.lineBuffer;

		glBindBuffer(GL_ARRAY_BUFFER, renderData.lineVB);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, renderData.lineBuffer);
	}

	/*!
	\brief Ends the current batch of fonts and uploads vertex data to the GPU.
	*/
	void Renderer::EndFontBatch() {
		GLsizeiptr size = (uint8_t*)renderData.fontBufferPtr - (uint8_t*)renderData.fontBuffer;

		glBindBuffer(GL_ARRAY_BUFFER, renderData.fontVB);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, renderData.fontBuffer);

	}

	/*!
	\brief Renders the batched quads and resets batch-related counters.
	*/
	void Renderer::Flush() {
		for (uint32_t i = 0; i < renderData.textureSlotIndex; i++) {
			glBindTextureUnit(i, renderData.textureSlots[i]);
		}

		glBindVertexArray(renderData.quadVA);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData.quadIB);
		glDrawElements(GL_TRIANGLES, renderData.indexCount, GL_UNSIGNED_INT, nullptr);

		renderData.renderStats.DrawCount++;

		renderData.indexCount = 0;
		renderData.textureSlotIndex = 1;
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/*!
	\brief Renders the batched lines.
	*/
	void Renderer::LineFlush() {

		glBindVertexArray(renderData.lineVA);

		glLineWidth(20.0f);  // Set the line width to make it more visible
		glDrawArrays(GL_LINES, 0, renderData.lineCount);

		renderData.lineCount = 0;

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/*!
	\brief Renders the batched fonts.
	*/
	void Renderer::FontFlush() {

		for (uint32_t i = 0; i < renderData.textureSlotIndex; i++) {
			glBindTextureUnit(i, renderData.textureSlots[i]);
		}

		glBindVertexArray(renderData.fontVA);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData.fontIB);
		glDrawElements(GL_TRIANGLES, renderData.fontIndexCount, GL_UNSIGNED_INT, nullptr);

		renderData.fontIndexCount = 0;
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/*!
	\brief Draws a quad with a specified position, size, rotation, and color.
	\param position The position of the quad.
	\param size The size of the quad.
	\param rot The rotation angle of the quad in radians.
	\param color The color of the quad.
	*/
	void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const float& rot, const glm::vec4& color) {
		if (renderData.indexCount >= maxindexCount) {
			EndBatch();
			Flush();
			BeginBatch();
		}

		float textureIndex = 0.0f;

		// Calculate rotation matrix
		glm::mat2 rotMatrix = glm::mat2(
			cos(rot), -sin(rot),
			sin(rot), cos(rot)
		);

		// Define the four corners of the quad (centered at (0,0) initially)
		glm::vec2 quadVertices[4] = {
			{ -0.5f * size.x, -0.5f * size.y },
			{  0.5f * size.x, -0.5f * size.y },
			{  0.5f * size.x,  0.5f * size.y },
			{ -0.5f * size.x,  0.5f * size.y }
		};

		// Apply rotation and translation to the vertices
		for (int i = 0; i < 4; i++) {
			quadVertices[i] = rotMatrix * quadVertices[i];  // Rotate
			quadVertices[i] += position;  // Translate
		}

		renderData.quadBufferPtr->position = quadVertices[0];
		renderData.quadBufferPtr->color = color;
		renderData.quadBufferPtr->texCoords = { 0.0f, 0.0f };
		renderData.quadBufferPtr->texIndex = textureIndex;
		renderData.quadBufferPtr++;

		renderData.quadBufferPtr->position = quadVertices[1];
		renderData.quadBufferPtr->color = color;
		renderData.quadBufferPtr->texCoords = { 1.0f, 0.0f };
		renderData.quadBufferPtr->texIndex = textureIndex;
		renderData.quadBufferPtr++;

		renderData.quadBufferPtr->position = quadVertices[2];
		renderData.quadBufferPtr->color = color;
		renderData.quadBufferPtr->texCoords = { 1.0f, 1.0f };
		renderData.quadBufferPtr->texIndex = textureIndex;
		renderData.quadBufferPtr++;

		renderData.quadBufferPtr->position = quadVertices[3];
		renderData.quadBufferPtr->color = color;
		renderData.quadBufferPtr->texCoords = { 0.0f, 1.0f };
		renderData.quadBufferPtr->texIndex = textureIndex;
		renderData.quadBufferPtr++;

		renderData.indexCount += 6;
		renderData.renderStats.QuadCount++;

	}

	/*!
	\brief Draws a debug quad with a specified position, size, rotation, and color.
	\param position The position of the quad.
	\param size The size of the quad.
	\param rot The rotation angle of the quad in radians.
	\param color The color of the quad.
	*/
	void Renderer::DrawDebugQuad(const glm::vec2& position, const glm::vec2& size, const float& rot, const glm::vec4& color) {
		// Calculate rotation matrix
		glm::mat2 rotMatrix = glm::mat2(
			cos(rot), -sin(rot),
			sin(rot), cos(rot)
		);

		// Define the four corners of the quad (centered at (0,0) initially)
		glm::vec2 quadVertices[4] = {
			{ -0.5f * size.x, -0.5f * size.y }, // Bottom-left
			{  0.5f * size.x, -0.5f * size.y }, // Bottom-right
			{  0.5f * size.x,  0.5f * size.y }, // Top-right
			{ -0.5f * size.x,  0.5f * size.y }  // Top-left
		};

		// Apply rotation and translation to the vertices
		for (int i = 0; i < 4; i++) {
			quadVertices[i] = rotMatrix * quadVertices[i];  // Rotate
			quadVertices[i] += position;  // Translate
		}

		// Draw lines between each of the corners
		DrawLine(quadVertices[0], quadVertices[1], color); // Bottom edge
		DrawLine(quadVertices[1], quadVertices[2], color); // Right edge
		DrawLine(quadVertices[2], quadVertices[3], color); // Top edge
		DrawLine(quadVertices[3], quadVertices[0], color); // Left edge

	}

	
	/*!
	\brief Renders text at a specified location with a specific scale and font.
	\param text The string of text to render.
	\param x The x-coordinate for the starting position of the text.
	\param y The y-coordinate for the starting position of the text.
	\param scale The scale factor for the text size.
	\param fontIdentifier A unique identifier for the font to use for rendering the text.
	This function iterates through each character in the text, retrieves its metrics from the assets manager,
	calculates its position based on its metrics and the provided scale, and then renders each character
	as a textured quad.
	*/
	void Renderer::RenderText(const std::string& text, float x, float y, float scale, const std::string& fontIdentifier, 
		const Vector3D& color, const float& opacity)
	{

		for (const char& c : text) {
			Character ch = assetsManager->GetCharacterMetrics(fontIdentifier, c);

			float xpos = x + ch.Bearing.x * scale + ch.Size.x / 2.f;
			float ypos = y + (ch.Bearing.y) * scale - ch.Size.y / 2.f;
			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;

			// need to flip y for uv as FreeType origin is top left but in opengl 
			// its bottom left
			glm::vec2 uvMin = glm::vec2(ch.uvMin.x, ch.uvMax.y);  
			glm::vec2 uvMax = glm::vec2(ch.uvMax.x, ch.uvMin.y);

			DrawFontQuad(glm::vec2(xpos, ypos), glm::vec2(w, h), 0.0f,
				uvMin, uvMax, assetsManager->GetFont(fontIdentifier).atlasTexture, {color.x, color.y, color.z, opacity});

			x += (ch.Advance.x / 64.0f) * scale;
		}

	}

	/*!
	\brief Draws a textured quad with custom UV coordinates.
	\param position The position of the quad in 2D space.
	\param size The size (width and height) of the quad.
	\param rot The rotation of the quad in degrees.
	\param assetsManager->GetTextures(anim.textureName) The OpenGL texture ID to apply to the quad.
	\param uvMin The bottom-left corner of the UV coordinates for the quad.
	\param uvMax The top-right corner of the UV coordinates for the quad.
	Handles texture binding based on the current texture slot index and updates the position
	and UVs of vertices to form the quad. Used for rendering textured quads where specific
	UV mapping is required.
	*/
	void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const float& rot, uint32_t TextureID,
		const glm::vec2& uvMin, const glm::vec2& uvMax, const glm::vec4& color)
	{
		// apparently the max render is 15
		if (renderData.indexCount >= maxindexCount || renderData.textureSlotIndex > 15) {

			EndBatch();
			Flush();
			ResetStats();
			BeginBatch();
		}

		glm::mat2 rotMatrix = glm::mat2(
			cos(rot), -sin(rot),
			sin(rot), cos(rot)
		);

		//constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f, };

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < renderData.textureSlotIndex; i++) {
			if (renderData.textureSlots[i] == TextureID) {
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f) {
			textureIndex = (float)renderData.textureSlotIndex;
			renderData.textureSlots[renderData.textureSlotIndex] = TextureID;
			renderData.textureSlotIndex++;
		}

		glm::vec2 quadVertices[4] = {
			{ -0.5f * size.x, -0.5f * size.y },
			{  0.5f * size.x, -0.5f * size.y },
			{  0.5f * size.x,  0.5f * size.y },
			{ -0.5f * size.x,  0.5f * size.y }
		};

		glm::vec2 uvs[4] = {
			uvMin,
			{ uvMax.x, uvMin.y },
			uvMax,
			{ uvMin.x, uvMax.y }
		};

		for (int i = 0; i < 4; i++) {
			quadVertices[i] = rotMatrix * quadVertices[i];
			quadVertices[i] += position;

			renderData.quadBufferPtr->position = quadVertices[i];
			renderData.quadBufferPtr->color = color;
			renderData.quadBufferPtr->texCoords = uvs[i];
			renderData.quadBufferPtr->texIndex = textureIndex;
			renderData.quadBufferPtr++;
		}

		renderData.indexCount += 6;
		renderData.renderStats.QuadCount++;
	}

	/*!
	\brief Draws a textured quad for fonts with specific UV coordinates and texture.
	\param position The 2D position where the font quad should be rendered.
	\param size The size of the quad.
	\param rot The rotation of the quad in radians.
	\param uvMin The lower left corner of the UV coordinates.
	\param uvMax The upper right corner of the UV coordinates.
	\param assetsManager->GetTextures(anim.textureName) The OpenGL texture ID to use.
	\details This function specifically handles rendering of font quads, which requires precise
	control over UV coordinates for correct text rendering. It adjusts the UVs to flip the y-axis
	due to differences in coordinate systems between FreeType and OpenGL. Similar to DrawQuad,
	it updates texture indices and handles batch rendering.
	*/
	void Renderer::DrawFontQuad(const glm::vec2& position, const glm::vec2& size, const float& rot,
		const glm::vec2& uvMin, const glm::vec2& uvMax, uint32_t TextureID, const glm::vec4& color)
	{
		if (renderData.fontIndexCount >= maxindexCount) {
			EndFontBatch();
			FontFlush();
			BeginFontBatch();
		}

		glm::mat2 rotMatrix = glm::mat2(
			cos(rot), -sin(rot),
			sin(rot), cos(rot)
		);

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < renderData.textureSlotIndex; i++) {
			if (renderData.textureSlots[i] == TextureID) {
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f) {
			textureIndex = (float)renderData.textureSlotIndex;
			renderData.textureSlots[renderData.textureSlotIndex] = TextureID;
			renderData.textureSlotIndex++;
		}

		glm::vec2 quadVertices[4] = {
			{ -0.5f * size.x, -0.5f * size.y },
			{  0.5f * size.x, -0.5f * size.y },
			{  0.5f * size.x,  0.5f * size.y },
			{ -0.5f * size.x,  0.5f * size.y }
		};

		glm::vec2 uvs[4] = {
			uvMin,
			{ uvMax.x, uvMin.y },
			uvMax,
			{ uvMin.x, uvMax.y }
		};

		for (int i = 0; i < 4; i++) {
			quadVertices[i] = rotMatrix * quadVertices[i];
			quadVertices[i] += position;

			renderData.fontBufferPtr->position = quadVertices[i];
			renderData.fontBufferPtr->color = color;
			renderData.fontBufferPtr->texCoords = uvs[i];
			renderData.fontBufferPtr->texIndex = textureIndex;
			renderData.fontBufferPtr++;
		}

		renderData.fontIndexCount += 6;
		renderData.renderStats.FontCount++;
	}

	/*!
	\brief Draws a point at a specified position with a given color.
	\param position The 2D position where the point should be rendered.
	\param color The color of the point, specified as a vec4 (RGBA).
	\details This function simulates drawing a point by rendering a very small quad
	at the specified position with the specified color. It leverages the DrawQuad
	method with a predefined small size to mimic a point.
	*/
	void Renderer::DrawPoint(const glm::vec2& position, const glm::vec4& color) {
		// Use a small size to mimic a point
		glm::vec2 pointSize = { 1.0f, 1.0f };

		DrawQuad(position, pointSize, 0.0f, color);
	}

	/*!
	\brief Draws a line between two specified points with a given color.
	\param start The starting point of the line.
	\param end The ending point of the line.
	\param color The color of the line, specified as a vec4 (RGBA).
	\details This function handles line rendering by defining the two endpoints of the line
	and setting the color. If the number of lines exceeds the batch capacity, it flushes
	the current batch and starts a new one. This ensures that all lines are rendered correctly
	within the batching system.
	*/
	void Renderer::DrawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color) {

		// Bind the VAO for line rendering
		if (renderData.lineCount >= maxLineVertexCount) {
			EndLineBatch();
			LineFlush();  // Custom method to handle line batch flushing
			BeginLineBatch();  // Reset the batch
		}

		// Define the two points that make up the line
		renderData.lineBufferPtr->position = start;
		renderData.lineBufferPtr->color = color;
		renderData.lineBufferPtr++;

		renderData.lineBufferPtr->position = end;
		renderData.lineBufferPtr->color = color;
		renderData.lineBufferPtr++;

		renderData.lineCount += 2;

	}

	void Renderer::DrawArrow(glm::vec2 start, glm::vec2 end, glm::vec4 color, float arrowHeadSize) {
		// Draw main arrow line
		Renderer::DrawLine(start, end, color);

		// Calculate arrowhead
		glm::vec2 direction = glm::normalize(end - start);
		glm::vec2 perp = glm::vec2(-direction.y, direction.x) * arrowHeadSize;

		glm::vec2 arrowLeft = end - direction * arrowHeadSize + perp;
		glm::vec2 arrowRight = end - direction * arrowHeadSize - perp;

		// Draw arrowhead as two lines
		Renderer::DrawLine(end, arrowLeft, color);
		Renderer::DrawLine(end, arrowRight, color);
	}

	void Renderer::ScaleGizmo(const glm::vec2& position, const glm::vec2& size, float handleSize) {
		// Calculate positions for the handles
		glm::vec2 xHandleEnd = glm::vec2(position.x + size.x * 0.5f + handleSize, position.y);
		glm::vec2 yHandleEnd = glm::vec2(position.x, position.y + size.y * 0.5f + handleSize);

		// Size of the square handles
		glm::vec2 handleDimensions = { handleSize, handleSize };

		// Draw X-axis scale gizmo (Red)
		Renderer::DrawLine(position, xHandleEnd - glm::vec2(handleSize * 0.5f, 0), glm::vec4(1, 0, 0, 1)); // Line in red
		Renderer::DrawQuad(xHandleEnd - handleDimensions * 0.5f, handleDimensions, 0.0f, glm::vec4(1, 0, 0, 1)); // Square handle at the end

		// Draw Y-axis scale gizmo (Green)
		Renderer::DrawLine(position, yHandleEnd - glm::vec2(0, handleSize * 0.5f), glm::vec4(0, 1, 0, 1)); // Line in green
		Renderer::DrawQuad(yHandleEnd - handleDimensions * 0.5f, handleDimensions, 0.0f, glm::vec4(0, 1, 0, 1)); // Square handle at the end
	}

	//void Renderer::DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color, int segments = 32) {
	//	if (segments < 3) segments = 3; // Ensure there are at least 3 segments for a valid circle

	//	float angleStep = 2.0f * glm::pi<float>() / static_cast<float>(segments);
	//	glm::vec2 previousPoint = center + glm::vec2(radius, 0.0f); // Start point at (radius, 0)

	//	for (int i = 1; i <= segments; ++i) {
	//		float angle = i * angleStep;
	//		glm::vec2 currentPoint = center + glm::vec2(radius * cos(angle), radius * sin(angle));

	//		// Draw a line between the previous point and the current point
	//		DrawLine(previousPoint, currentPoint, color);

	//		previousPoint = currentPoint; // Update the previous point for the next segment
	//	}
	//}

	/*!
	\brief Retrieves the current rendering statistics.
	\return The current statistics of the renderer, including the number of draw calls and rendered quads.
	\details This function provides access to the internal statistics of the renderer, allowing for performance
	monitoring and debugging by reporting the number of draw calls and other relevant metrics.
	*/
	const Renderer::Stats& Renderer::GetStats() {
		return renderData.renderStats;
	}

	/*!
	\brief Resets the rendering statistics.
	\details This function clears the rendering statistics, setting all counts such as draw calls and
	rendered elements back to zero. It is typically called at the beginning of a new frame or rendering cycle.
	*/
	void Renderer::ResetStats() {
		memset(&renderData.renderStats, 0, sizeof(Stats));
	}

	void Renderer::DrawGrid(float gridWidth, float gridHeight, float gridSpacing,
		const glm::vec4& color) {
		float halfWidth = gridWidth / 2.0f;
		float halfHeight = gridHeight / 2.0f;

		// Draw vertical lines
		for (float x = -halfWidth; x <= halfWidth; x += gridSpacing) {
			DrawLine({ x, -halfHeight }, { x, halfHeight }, color);
		}

		// Draw horizontal lines
		for (float y = -halfHeight; y <= halfHeight; y += gridSpacing) {
			DrawLine({ -halfWidth, y }, { halfWidth, y }, color);
		}
	}

	Vector2D Renderer::SnapToGrid(const Vector2D& position, float gridSpacing) {
		float x = std::round(position.x / gridSpacing) * gridSpacing;
		float y = std::round(position.y / gridSpacing) * gridSpacing;
		return { x, y };
	}

	// --------------------------------------------------------------------------- SpriteAnimator Functions -----------------------------------------------------------------------------------
	
	/*!
	\brief Updates the sprite animation based on the elapsed time.
	\param deltaTime The time elapsed since the last update, in seconds.
	\details This function advances the sprite animation frame if the accumulated time exceeds
	the frame duration. It handles the timing and frame advancement for sprite animations.
	*/
	void SpriteAnimator::Update(float deltaTime)
	{
		if (!playing) return;
		TimeAccumulator += deltaTime;

		if (TimeAccumulator >= animationSpeed) {
			TimeAccumulator -= animationSpeed;
			AdvanceFrame();
		}
		
	}

	/*!
	\brief Resets the animation to the first frame.
	\details This function resets the animation to its initial state, setting the current frame
	to the first frame and clearing any accumulated time.
	*/
	void SpriteAnimator::ResetAnimation()
	{
		TimeAccumulator = 0.0f;
		CurrentFrame = 1; // Reset to the first frame
	}

	/*!
	\brief Renders the current frame of the sprite animation at a specified position and rotation.
	\param position The position to render the sprite.
	\param size The size of the sprite.
	\param rotation The rotation angle of the sprite, in radians.
	\details This function calculates the correct texture coordinates for the current frame and renders
	the sprite using these coordinates. It ensures the sprite is rendered with the correct orientation
	and alignment based on its animation state.
	*/
	void SpriteAnimator::RenderCurrentFrame(float rotation)
	{
		for (Entity entity : entityBucket) {
			size_t row = GetRowForFrame(CurrentFrame);
			size_t col = GetColumnForFrame(CurrentFrame, row);
			float FrameWidth = frameSize.x / GetMaxFramesInRow();
			float FrameHeight = frameSize.y / static_cast<float>(framesPerRow.size());
			glm::vec2 framePos(col * FrameWidth, row * FrameHeight);
			glm::vec2 uvMin(
				framePos.x / frameSize.x,
				1 - ((framePos.y + FrameHeight) / frameSize.y)

			);
			glm::vec2 uvMax(
				(framePos.x + FrameWidth) / frameSize.x,
				1 - (framePos.y / frameSize.y)
			);
			// Render the quad with the calculated UV coordinates
			auto it = frameSoundMap.find(CurrentFrame);
			if (it != frameSoundMap.end()) {
				std::string soundKey = it->second;
				audioSystem->Play_Sound(soundKey, 0.3f);
#ifdef DEBUG
				spdlog::info("Playing sound {} for frame {}", soundKey, CurrentFrame);
#endif // DEBUG
			}
			if (entity == 0) {
				Renderer::DrawQuad({ animationPosition.x, animationPosition.y },
					{ animationScale.x + scaleOffset.x, animationScale.y + scaleOffset.y }
				, rotation, assetsManager->GetTextures(textureName), uvMin, uvMax, { 1.0, 1.0, 1.0, 1.0 });
			}
			else {
				auto& transformComponent = ecsHub->GetComponent<Transform>(entity);
				animationPosition.x = transformComponent.position.x + positionOffset.x;
				animationPosition.y = transformComponent.position.y + positionOffset.y;
				Renderer::DrawQuad({ animationPosition.x, animationPosition.y },
					{ animationScale.x + scaleOffset.x, animationScale.y + scaleOffset.y }
				, rotation, assetsManager->GetTextures(textureName), uvMin, uvMax, { 1.0, 1.0, 1.0, 1.0 });
			}
			
		}
		
	}

	/*!
	\brief Calculates the maximum number of frames in any row of the sprite sheet.
	\return The maximum number of frames in a single row.
	\details This function iterates over the frames per row to find the maximum count, which is used
	to calculate frame dimensions and positioning.
	*/
	size_t SpriteAnimator::GetMaxFramesInRow() const
	{
		size_t maxFrames = 0;
		for (size_t frames : framesPerRow) {
			if (frames > maxFrames) {
				maxFrames = frames;
			}
		}
		return maxFrames;
		
	}

	/*!
	\brief Advances to the next frame in the animation sequence.
	\details This function increments the current frame index, looping back to the first frame if the end
	of the animation sequence is reached. It manages the cyclic behavior of sprite animations.
	*/
	void SpriteAnimator::AdvanceFrame()
	{
		size_t totalFrames = GetTotalFrames();
		if (CurrentFrame >= totalFrames - 1) { // If the last frame is reached
			if (looping) {
				CurrentFrame = 1; // Restart animation
			}
			else {
				CurrentFrame = totalFrames - 1; // Stop at last frame
				playing = false; // Stop playing if not looping
				entityBucket.clear();
				ResetAnimation();
			}
		}
		else {
			CurrentFrame++; // Move to the next frame
		}
	}

	/*!
	\brief Calculates the total number of frames in the sprite animation.
	\return The total number of frames.
	\details This function sums up all frames across all rows of the sprite sheet to determine the total number
	of frames available in the animation.
	*/
	size_t SpriteAnimator::GetTotalFrames() const
	{
		size_t totalFrames = 0;
		for (size_t frames : framesPerRow) {
			totalFrames += frames;
		}
		return totalFrames;
	}

	/*!
	\brief Determines the row index for a specified frame index.
	\param frameIndex The index of the frame in the sequence.
	\return The row index where the frame is located.
	\details This function calculates which row of the sprite sheet contains the specified frame, based on
	the frame index and the distribution of frames per row.
	*/
	size_t SpriteAnimator::GetRowForFrame(size_t frameIndex) const
	{
		if (framesPerRow.empty()) {
			return 0;
		}
		size_t cumulativeFrames = 0;
		for (size_t i = 0; i < framesPerRow.size(); ++i) {
			cumulativeFrames += framesPerRow[i];
			if (frameIndex <= cumulativeFrames) {
				return i;
			}
		}
		return framesPerRow.size() - 1;
	}

	/*!
	\brief Determines the column index for a specified frame index and row.
	\param frameIndex The index of the frame in the sequence.
	\param row The row index containing the frame.
	\return The column index of the frame within the specified row.
	\details This function calculates the column index of the frame within its row, using the frame index and
	the cumulative frames counted up to the specified row.
	*/
	size_t SpriteAnimator::GetColumnForFrame(size_t frameIndex, size_t row) const
	{
		if (framesPerRow.empty()) {
			return 0;
		}
		size_t framesBeforeRow = 0;
		for (size_t i = 0; i < row; ++i) {
			framesBeforeRow += framesPerRow[i];
		}
		size_t columnIndex = frameIndex - framesBeforeRow - 1; // Adjust for 1-based indexing
		return columnIndex;
	}

}
