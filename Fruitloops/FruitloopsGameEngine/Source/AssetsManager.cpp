/****************************************************************************/
/*!
\file		AssetsManager.cpp
\Proj name  Fruitloops
\author 	Zhang TongYan
\date   	Oct 02, 2024
\brief		Implementation of the AssetsManager class which handles the loading,
            management, and unloading of game assets including textures, shaders,
            soundObjects, and fontObjects.
            This class utilizes external libraries like stb_image for texture loading
            and FreeType for font handling, encapsulating all asset-related operations
            to streamline their use throughout the game engine.
All content (C) 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /****************************************************************************/
#include "pch.h"
#include "AssetsManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace FruitLoops {

    // Containers
    std::map<std::string, GLuint> AssetsManager::textureObjects;
    std::map<std::string, ShaderManager> AssetsManager::shaderPrograms;
    std::map<std::string, FMOD::Sound*> AssetsManager::soundObjects;
    std::map<std::string, Font> AssetsManager::fontObjects;
    std::unordered_map<std::string, GLuint> AssetsManager::iconCache;

    AssetsManager* assetsManager = NULL;

    /*!
    \brief Constructor for the AssetsManager class.
    \details Initializes the FreeType library for font rendering and sets the singleton instance.
    */
    AssetsManager::AssetsManager() {

        // init font
        if (FT_Init_FreeType(&ftLibrary)) {
#ifdef _Debug
            std::cerr << "Failed to initialize FreeType Library" << std::endl;
#endif
        }  

        assetsManager = this;
    }

    /*!
    \brief Destructor for the AssetsManager class.
    \details Cleans up all loaded resources, including textures, shaders, soundObjects, and fontObjects, and
    shuts down the FreeType library.
    */
    AssetsManager::~AssetsManager() {

        // release font 
        for (auto& fontPair : fontObjects) {
            glDeleteTextures(1, &fontPair.second.atlasTexture);  // Delete the atlas texture
        }
        fontObjects.clear();

        // Clean up FreeType resources
        if (face) {
            FT_Done_Face(face);
            face = nullptr;  // Avoid dangling pointers
        }

        if (ftLibrary) {
            FT_Done_FreeType(ftLibrary);
            ftLibrary = nullptr;
        }

        assetsManager = nullptr;
    }

    /*!
    \brief Initializes the AssetsManager by loading all resources from the designated paths.
    \details Scans directories for resources such as textures, shaders, soundObjects, and fontObjects,
    loading them into the appropriate storage containers for use throughout the game.
    */
    void AssetsManager::Init()
    {
#ifdef _DEBUG
        spdlog::info("Loading textures");
#endif
        for (const auto& file : std::filesystem::directory_iterator("../Assets/Textures/")) {
            if (file.is_regular_file() && file.path().extension() == ".png") {
                std::string filename = file.path().stem().string();
                std::string filenameWithExtension = file.path().filename().string();
                LoadTexture(file.path().string(), file.path().stem().string());
                iconCache[filenameWithExtension] = GetTextures(filename);
            }
        }

        // Load shaders
#ifdef _DEBUG
        spdlog::info("Loading shaders");
#endif
        for (const auto& file : std::filesystem::directory_iterator("../Assets/Shaders/")) {
            if (file.is_regular_file() && file.path().extension() == ".vert") {
                std::string name = file.path().stem().string();  // Extract base name like "Batch"
                std::string vertPath = file.path().string();

                // Manually construct the fragment shader path by replacing the extension
                std::string fragPath = file.path().parent_path().string() + "/" + name + ".frag";

                // Ensure the matching .frag file exists before loading shaders
                if (std::filesystem::exists(fragPath)) {
                    LoadShaders(name, vertPath, fragPath);
                }
            }
        }

        // Load soundObjects
#ifdef _DEBUG
        spdlog::info("Loading soundObjects");
#endif
        for (const auto& file : std::filesystem::directory_iterator("../Assets/Audio/")) {
            if (file.is_regular_file() && (file.path().extension() == ".wav" || file.path().extension() == ".mp3")) {
                LoadSound(file.path().string(), file.path().stem().string());
            }
        }
#ifdef _DEBUG
        spdlog::info("Loading fontObjects");
#endif
        int fontSize = ConfigData::fontDetail;  // Default font size
        for (const auto& file : std::filesystem::directory_iterator("../Assets/Fonts/")) {
            if (file.is_regular_file() && file.path().extension() == ".ttf") {
                LoadFont(file.path().stem().string(), file.path().string(), fontSize);
            }
        }
    }

    /*!
    \brief Loads a texture from a file.
    \param pathName The file path to the texture image.
    \param TexName The name to associate with the loaded texture.
    \return True if the texture was loaded successfully, false otherwise.
    \details Loads a texture using stb_image, creates an OpenGL texture object, and stores
    it in the texture map with the specified name.
    */
	bool AssetsManager::LoadTexture(std::string const& pathName, std::string TexName) {
        int width, height, channels;
        
        stbi_set_flip_vertically_on_load(true);
        // Allocate memory for the texture data
        unsigned char* ptr_texels = stbi_load(pathName.c_str(), &width, &height, &channels, STBI_rgb_alpha);  // Force 4 channels (RGBA)

        if (!ptr_texels) {
#ifdef _Debug
            std::cerr << "Failed to load texture : " << pathName << std::endl;
#endif
            return false;  // Return 0 to indicate failure
        }

        GLuint texobj_hdl;
        // define and initialize a handle to texture object that will
        // encapsulate two-dimensional textures
        glCreateTextures(GL_TEXTURE_2D, 1, &texobj_hdl);
        // allocate GPU storage for texture image data loaded from file
        glTextureStorage2D(texobj_hdl, 1, GL_RGBA8, width, height);
        // copy image data from client memory to GPU texture buffer memory
        glTextureSubImage2D(texobj_hdl, 0, 0, 0, width, height,
            GL_RGBA, GL_UNSIGNED_BYTE, ptr_texels);

        // client memory not required since image is buffered in GPU memory
        stbi_image_free(ptr_texels);
        // store obj in the container
        textureObjects[TexName] = texobj_hdl;
        return true;
	}

    /*!
    \brief Loads and compiles shaders from files.
    \param shaderName The name to associate with the shader program.
    \param vertexShader Path to the vertex shader file.
    \param fragmentShader Path to the fragment shader file.
    \return True if the shaders were compiled and linked successfully, false otherwise.
    \details Compiles the shaders, links them into a shader program, and stores the program
    in the shader map with the specified name. If compilation or linking fails, logs the error.
    */
    bool AssetsManager::LoadShaders(const std::string& shaderName,
        const std::string& vertexShader,
        const std::string& fragmentShader) {
        std::map<std::string, ShaderManager>::iterator it =
            shaderPrograms.find(shaderName);
        if (it != shaderPrograms.end()) return false;
        std::vector<std::pair<GLenum, std::string>> shaderFiles{
        std::make_pair(GL_VERTEX_SHADER, vertexShader),
        std::make_pair(GL_FRAGMENT_SHADER, fragmentShader)
        };
        ShaderManager shaders;
        //AUTOMATION_HOOK_SHADER(shaders, shaderFiles);
        shaders.CompileLinkValidate(shaderFiles);
        if (GL_FALSE == shaders.IsLinked()) {
#ifdef _DEBUG
            spdlog::error("Unable to compile/link/validate shader programs");
            spdlog::error(shaders.GetLog());
#endif
            std::exit(EXIT_FAILURE);
        }

        shaderPrograms[shaderName] = shaders;
        return true;
    }

    /*!
    \brief Loads a sound file into an FMOD sound object.
    \param pathName The file path to the sound file.
    \param soundName The name to associate with the loaded sound.
    \return True if the sound was loaded successfully, false otherwise.
    \details Loads the sound file using FMOD, creating a sound object that is stored
    in the sound map with the specified name.
    */
    bool AssetsManager::LoadSound(std::string const& pathName, std::string soundName) {
        FMOD::Sound* pSound = nullptr;
        audioSystem->GetSystem()->createSound(pathName.c_str(), FMOD_DEFAULT, nullptr, &pSound);
        soundObjects[soundName] = pSound;
        return true;
    }

    /*!
    \brief Retrieves the OpenGL texture object associated with a given identifier.
    \param identifier The name of the texture to retrieve.
    \return The OpenGL texture handle, or 0 if the texture is not found.
    \details Looks up the texture in the texture map and returns the associated OpenGL texture handle.
    */
    GLuint AssetsManager::GetTextures(std::string identifier) const
    {
        auto it = textureObjects.find(identifier);
        if (it != textureObjects.end()) {
            return it->second;
        }
        return 0;
    }


    /*!
    \brief Retrieves the ShaderManager associated with a given identifier.
    \param identifier The name of the shader program to retrieve.
    \return A ShaderManager instance containing the shader program, or an empty ShaderManager if not found.
    \details Looks up the shader program in the shader map and returns the associated ShaderManager instance.
    */
    ShaderManager AssetsManager::GetShaders(std::string identifier)
    {
        if (!shaderPrograms.count(identifier)) {
#ifdef _Debug
            std::cerr << "Shader not found" << std::endl;
#endif
            return ShaderManager();
        }
        return shaderPrograms[identifier]; 
    }

    /*!
    \brief Retrieves an FMOD::Sound object associated with a given identifier.
    \param identifier The name of the sound to retrieve.
    \return A pointer to the FMOD::Sound object, or nullptr if the sound is not found.
    \details Looks up the sound in the sound map and returns the associated FMOD::Sound pointer.
    */
    FMOD::Sound* AssetsManager::GetSounds(std::string identifier)
    {
        if (!soundObjects.count(identifier)) {
#ifdef _Debug
            std::cerr << "Sound not found" << std::endl;
#endif
            return nullptr;
        }
        return soundObjects[identifier];
    }

    void AssetsManager::HandleDrop(GLFWwindow* window, int count, const char** paths)
    {

        UNUSED_PARAM(window);
        for (int i = 0; i < count; i++) {
            std::string filePath = paths[i];
            std::filesystem::path path(filePath);

            // Convert extension to lowercase for consistent comparison
            std::string ext = path.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if (ext == ".png") {
                std::string filename = path.stem().string();
                LoadTexture(path.string(), filename);
                // Define the destination folder for textures
                std::filesystem::path destFolder = "../Assets/Textures/";
                // Create the directory if it doesn't exist
                if (!std::filesystem::exists(destFolder)) {
                    std::filesystem::create_directories(destFolder);
                }
                // Construct the full destination path (using the original filename)
                std::filesystem::path destPath = destFolder / path.filename();
                // Copy the file to the assets folder, overwriting if it exists
                try {
                    std::filesystem::copy_file(path, destPath, std::filesystem::copy_options::overwrite_existing);
                    std::cout << "Texture copied to assets folder: " << destPath << std::endl;
                }
                catch (std::filesystem::filesystem_error& e) {
                    std::cout << "Error copying texture file: " << e.what() << std::endl;
                }
            }
            else if (ext == ".vert") {
                std::string name = path.stem().string();
                std::string vertPath = path.string();
                std::string fragPath = path.parent_path().string() + "/" + name + ".frag";

                if (std::filesystem::exists(fragPath)) {
                    LoadShaders(name, vertPath, fragPath);
                    // Define the destination folder for textures
                    std::filesystem::path destFolder = "../Assets/Shaders/";
                    // Create the directory if it doesn't exist
                    if (!std::filesystem::exists(destFolder)) {
                        std::filesystem::create_directories(destFolder);
                    }
                    // Construct the full destination path (using the original filename)
                    std::filesystem::path destPath = destFolder / path.filename();
                    // Copy the file to the assets folder, overwriting if it exists
                    try {
                        std::filesystem::copy_file(path, destPath, std::filesystem::copy_options::overwrite_existing);
                        std::cout << "Texture copied to assets folder: " << destPath << std::endl;
                    }
                    catch (std::filesystem::filesystem_error& e) {
                        std::cout << "Error copying texture file: " << e.what() << std::endl;
                    }
                }
                else {
                    std::cout << "Matching fragment shader file not found for " << vertPath << std::endl;
                }
            }
            else if (ext == ".wav" || ext == ".mp3") {
                LoadSound(path.string(), path.stem().string());
                // Define the destination folder for textures
                std::filesystem::path destFolder = "../Assets/Audio/";
                // Create the directory if it doesn't exist
                if (!std::filesystem::exists(destFolder)) {
                    std::filesystem::create_directories(destFolder);
                }
                // Construct the full destination path (using the original filename)
                std::filesystem::path destPath = destFolder / path.filename();
                // Copy the file to the assets folder, overwriting if it exists
                try {
                    std::filesystem::copy_file(path, destPath, std::filesystem::copy_options::overwrite_existing);
                    std::cout << "Texture copied to assets folder: " << destPath << std::endl;
                }
                catch (std::filesystem::filesystem_error& e) {
                    std::cout << "Error copying texture file: " << e.what() << std::endl;
                }
            }
            else if (ext == ".ttf") {
                int fontSize = ConfigData::fontDetail;
                LoadFont(path.stem().string(), path.string(), fontSize);
                // Define the destination folder for textures
                std::filesystem::path destFolder = "../Assets/Fonts/";
                // Create the directory if it doesn't exist
                if (!std::filesystem::exists(destFolder)) {
                    std::filesystem::create_directories(destFolder);
                }
                // Construct the full destination path (using the original filename)
                std::filesystem::path destPath = destFolder / path.filename();
                // Copy the file to the assets folder, overwriting if it exists
                try {
                    std::filesystem::copy_file(path, destPath, std::filesystem::copy_options::overwrite_existing);
                    std::cout << "Texture copied to assets folder: " << destPath << std::endl;
                }
                catch (std::filesystem::filesystem_error& e) {
                    std::cout << "Error copying texture file: " << e.what() << std::endl;
                }
            }
            else {
                std::cout << "File type not supported: " << ext << std::endl;
            }
        }
    }

    /*!
    \brief Retrieves character metrics for a specific character from a font.
    \param fontName The name of the font.
    \param character The character to retrieve metrics for.
    \return A Character structure containing the metrics for the specified character.
    \details Looks up the font and then the character in the font's character map, returning
    the character's metrics such as size, bearing, advance, and texture coordinates.
    */
    Character AssetsManager::GetCharacterMetrics(const std::string& fontName, char character) const {
        // Check if the font exists in the fontObjects map
        auto fontIt = fontObjects.find(fontName);
        if (fontIt == fontObjects.end()) {
#ifdef _Debug
            std::cerr << "Font not found: " << fontName << std::endl;
#endif
            return Character();  // Return a default Character if the font is not found
        }

        // Check if the character exists in the font's character map
        const auto& characterMap = fontIt->second.characters;
        auto charIt = characterMap.find(character);
        if (charIt == characterMap.end()) {
#ifdef _Debug
            std::cerr << "Character not found in font " << fontName << ": " << character << std::endl;
#endif
            return Character();  // Return a default Character if the character is not found
        }

        return charIt->second;  // Return the Character metrics
    }

    /*!
    \brief Retrieves a Font object by name.
    \param fontName The name of the font to retrieve.
    \return A constant reference to the Font object, or an empty Font object if not found.
    \details Looks up the font in the font map and returns a reference to the Font object.
    */
    const Font& AssetsManager::GetFont(const std::string& fontName) const {
        auto it = fontObjects.find(fontName);
        if (it == fontObjects.end()) {
#ifdef _Debug
            std::cerr << "Font not found: " << fontName << std::endl;
#endif
            static Font emptyFont;  // A static empty font to return in case of failure
            return emptyFont;
        }
        return it->second;  // Return by reference to avoid copying
    }

    /*!
    \brief Loads a font from a file and generates a texture atlas for its glyphs.
    \param fontName The name to associate with the loaded font.
    \param fontPath The file path to the font file.
    \param fontSize The size of the font to load.
    \return True if the font was loaded and the texture atlas was created successfully, false otherwise.
    \details Loads the font using FreeType, creates a texture atlas containing all glyphs for the font,
    and stores the Font object in the font map with the specified name.
    */
    bool AssetsManager::LoadFont(const std::string& fontName, const std::string& fontPath, int fontSize) {

        if (FT_New_Face(ftLibrary, fontPath.c_str(), 0, &face)) {
#ifdef _Debug
            std::cerr << "Failed to load font: " << fontPath << std::endl;
#endif
            return false;
        }

        FT_Set_Pixel_Sizes(face, 0, fontSize);

        unsigned int atlasWidth = 1024;
        unsigned int atlasHeight = 512;

        GLuint textureAtlas;
        glGenTextures(1, &textureAtlas);
        glBindTexture(GL_TEXTURE_2D, textureAtlas);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

        int x = 0;  // x position in atlas
        int y = 0;  // y position in atlas
        int rowHeight = 0;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        std::map<char, Character> characters;

        for (unsigned char c = 0; c < 128; c++) {

            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
#ifdef _Debug
                std::cerr << "Failed to load Glyph: " << c << std::endl;
#endif
                continue;
            }

            if (x + face->glyph->bitmap.width > atlasWidth) {
                x = 0;
                y += rowHeight;
                rowHeight = 0;
            }

            glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

            Character character;
            character.Size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            character.Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
            character.Advance = face->glyph->advance;
            character.uvMin = glm::vec2((float)x / atlasWidth, (float)y / atlasHeight);
            character.uvMax = glm::vec2((float)(x + face->glyph->bitmap.width) / atlasWidth,
                (float)(y + face->glyph->bitmap.rows) / atlasHeight);

            characters[c] = character;

            x += face->glyph->bitmap.width;
            rowHeight = std::max(static_cast<unsigned int>(rowHeight), face->glyph->bitmap.rows);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Font font;
        font.atlasTexture = textureAtlas;
        font.characters = characters;

        fontObjects[fontName] = font;

        return true;
    }

 /**
 * @brief Loads and caches image files from a directory, returning a list of FileItem structures.
 *
 * This function scans the specified directory and creates a list of FileItem objects for each file and directory.
 * It loads textures for files with image extensions (.png, .jpg, .jpeg, .bmp) and caches them to optimize performance.
 * Non-image files are skipped with a warning in debug mode if the directory is invalid or not a directory.
 *
 * @param directoryPath The filesystem path to the directory to scan.
 * @return std::vector<FileItem> List of FileItems representing the contents of the directory.
 */
    std::vector<FileItem> AssetsManager::LoadDirectoryContents(const std::filesystem::path& directoryPath) {
        std::vector<FileItem> items;

        if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)) {
            for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
                FileItem item;
                item.name = entry.path().filename().string();
                item.isDirectory = std::filesystem::is_directory(entry.path());

                // Only attempt to load textures for valid image file extensions
                if (!item.isDirectory) { // Exclude directories
                    const auto extension = entry.path().extension().string();
                    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp") {
                        // Check if the icon is already cached
                        if (iconCache.find(item.name) == iconCache.end()) {
                            // Only load the texture if it's not in the cache
                            if (!GetTextures(item.name)) {
                                
                                LoadTexture(entry.path().string(), item.name);
                            }
                            // Store the loaded texture in both the iconCache and the item
                            iconCache[item.name] = GetTextures(item.name);
                        }
                        item.icon = iconCache[item.name];  // Use the cached icon
                    }
                    else {
#ifdef _DEBUG
                        spdlog::warn("Skipped non-image file: {}", entry.path().string());
#endif
                    }
                }

                items.push_back(item);
            }
        }
        else {
#ifdef _DEBUG
            spdlog::error("Directory does not exist or is not a directory: {}", directoryPath.string());
#endif
        }
        return items;
    }
}