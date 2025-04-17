/****************************************************************************/
/*!
\file		AssetsManager.h
\Proj name  Fruitloops
\author 	Zhang TongYan
\date   	Oct 02, 2024
\brief		Declaration of the implementation of the AssetsManager class which handles the loading,
            management, and unloading of game assets including textures, shaders,
            soundObjects, and fontObjects.
            This class utilizes external libraries like stb_image for texture loading
            and FreeType for font handling, encapsulating all asset-related operations
            to streamline their use throughout the game engine.
All content (C) 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /****************************************************************************/
#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H 
#include "Graphics.h"
#include "ShaderManager.h"
#include "Audio.h"

namespace FruitLoops {

	struct Character {
		glm::ivec2 Size;        // x is width, y is rows
		glm::ivec2 Bearing;     // x is left, y is top
		FT_Vector Advance;      // advance offset
		glm::vec2 uvMin;        
		glm::vec2 uvMax;        
	};

	struct Font {
		GLuint atlasTexture;
		std::map<char, Character> characters;
	};

	struct FileItem {
		std::string name;
		bool isDirectory;
		GLuint icon;
	};
	/*!
	\class AssetsManager
	\brief Manages all game assets including textures, shaders, soundObjects, and fontObjects.
	\details This class is responsible for loading, managing, and unloading all types of assets used within the game.
			 It uses external libraries such as stb_image for texture loading and FreeType for font management. The class
			 ensures that all assets are accessible through unified interfaces and are cleanly unloaded when no longer needed.
	*/
	class AssetsManager {
	public:

	/*!
	\fn AssetsManager::AssetsManager()
	\brief Constructor that initializes the FreeType library needed for font management.
	*/
		AssetsManager::AssetsManager();

	/*!
	\fn AssetsManager::~AssetsManager()
	\brief Destructor that cleans up all loaded assets and the FreeType library.
	\details Ensures all textures, shaders, soundObjects, and fontObjects are properly unloaded and
			 releases any resources allocated by FreeType.
	*/
		AssetsManager::~AssetsManager();

	/*!
	\fn bool AssetsManager::LoadTexture(const std::string& pathName, const std::string& TexName)
	\param pathName The file path to the texture to load.
	\param TexName The name under which the texture will be registered.
	\return True if the texture was successfully loaded and registered, false otherwise.
	\brief Loads a texture from the specified file and registers it under a given name.
	*/
		bool LoadTexture(std::string const& pathName, std::string TexName);
		
	/*!
	\fn bool AssetsManager::LoadShaders(const std::string& shaderName, const std::string& vertexShader, const std::string& fragmentShader)
	\param shaderName The name under which the shader program will be registered.
	\param vertexShader The path to the vertex shader file.
	\param fragmentShader The path to the fragment shader file.
	\return True if the shaders were successfully compiled, linked, and registered, false otherwise.
	\brief Loads and compiles vertex and fragment shaders, links them into a shader program, and registers it.
	*/
		bool LoadShaders(const std::string& shaderName,
			const std::string& vertexShader,
			const std::string& fragmentShader);
		
	/*!
	\fn bool AssetsManager::LoadSound(const std::string& pathName, const std::string& soundName)
	\param pathName The file path to the sound file.
	\param soundName The name under which the sound will be registered.
	\return True if the sound was successfully loaded and registered, false otherwise.
	\brief Loads a sound from a file and registers it under a specified name using FMOD.
	*/
		bool LoadSound(std::string const& pathName, std::string soundName);
		
	/*!
	\fn bool AssetsManager::LoadFont(const std::string& fontName, const std::string& fontPath, int fontSize)
	\param fontName The name under which the font will be registered.
	\param fontPath The path to the font file.
	\param fontSize The size at which to load the font.
	\return True if the font was successfully loaded and the texture atlas was created, false otherwise.
	\brief Loads a font from a file, creates a texture atlas for its glyphs, and registers the font.
	*/
		bool LoadFont(const std::string& fontName, const std::string& fontPath, int fontSize);

	/*!
	\fn GLuint AssetsManager::GetTextures(const std::string& identifier)
	\param identifier The name of the texture to retrieve.
	\return The OpenGL texture handle associated with the identifier, or 0 if not found.
	\brief Retrieves the texture handle by its registered name.
	*/
		GLuint GetTextures(std::string identifier) const;
		
		
	/*!
	\fn ShaderManager AssetsManager::GetShaders(const std::string& identifier)
	\param identifier The name of the shader program to retrieve.
	\return The ShaderManager instance managing the requested shader program, or a default instance if not found.
	\brief Retrieves the shader program by its registered name.
	*/
		ShaderManager GetShaders(std::string identifier);
		
	/*!
	\fn FMOD::Sound* AssetsManager::GetSounds(const std::string& identifier)
	\param identifier The name of the sound to retrieve.
	\return A pointer to the FMOD::Sound object, or nullptr if the sound is not found.
	\brief Retrieves the sound object by its registered name.
	*/
		FMOD::Sound* GetSounds(std::string identifier);

		void HandleDrop(GLFWwindow* window, int count, const char** paths);
		
	/*!
	\fn Character AssetsManager::GetCharacterMetrics(const std::string& fontName, char character) const
	\param fontName The name of the font from which to retrieve character metrics.
	\param character The character for which metrics are requested.
	\return A Character structure containing the metrics for the specified character.
	\brief Retrieves character metrics for a specific character from a specified font.
	*/
		Character GetCharacterMetrics(const std::string& fontName, char character) const;
		FT_Face GetFace() const { return face; }
		
	/*!
	\fn const Font& AssetsManager::GetFont(const std::string& fontName) const
	\param fontName The name of the font to retrieve.
	\return A constant reference to the Font object, or a default empty Font object if not found.
	\brief Retrieves a Font object by its registered name, providing access to its details and texture atlas.
	*/
		const Font& GetFont(const std::string& fontName) const;

		const std::map<std::string, Font>& GetFontContainer() const { return fontObjects; }
		const std::map<std::string, FMOD::Sound*>& GetSoundContainer() const { return soundObjects; }

		std::vector<FileItem> LoadDirectoryContents(const std::filesystem::path& directoryPath);

	/*!
	\fn void AssetsManager::Init()
	\brief Initializes the AssetsManager by loading all necessary assets from predefined paths.
	\details Scans and loads all textures, shaders, soundObjects, and fontObjects specified in the game's assets directories.
	*/
		void Init();
	private:

		FT_Library ftLibrary;
		FT_Face face;
		static std::map<std::string, GLuint> textureObjects;
		static std::map<std::string, ShaderManager> shaderPrograms;
		static std::map<std::string, FMOD::Sound*> soundObjects;
		static std::map<std::string, Font> fontObjects;
		static std::unordered_map<std::string, GLuint> iconCache;
	};

	extern AssetsManager* assetsManager;
}
