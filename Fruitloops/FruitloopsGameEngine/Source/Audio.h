/******************************************************************************/
/*!
\file  Audio.h
\Proj name  Shroomy Doomy
\author  Reagan Tang
\date    Oct 03, 2024
\brief   This file contains the declarations for the AudioSystem class
         and related functions, responsible for managing all sound and music
         playback in the game. It includes initialization of the FMOD system,
         playing, stopping, and releasing sounds and music tracks, along with
         handling dynamic sound channel management.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

#pragma once
#include "System.h"
#include "AssetsManager.h"
#include "Input.h"
#include "Log.h"

// Forward declarations for FMOD classes.
namespace FMOD {
    class System;
    class Channel;
    class Sound;
}

namespace FruitLoops {

    class AudioSystem : public EngineSystems {
    public:

        
        /*!
        * \brief Constructs the AudioSystem instance.
        * Initializes member variables and sets up the FMOD system for the game.
        */
        AudioSystem();

        /*!
        * \brief Destructs the AudioSystem instance.
        * Cleans up allocated resources and shuts down the FMOD system.
        */
        ~AudioSystem();

        /*!
        * \brief Plays a sound effect associated with the specified key.
        * \param soundKey The unique key identifying the sound effect.
        */
        void Play_Sound(const std::string& soundKey, float volume);

        /*!
        * \brief Plays background music associated with the specified key.
        * \param musicKey The unique key identifying the music track.
        */
        void Play_Music(const std::string& musicKey, float fadeDuration, float volume);

        /*!
        * \brief Releases the memory and resources associated with a sound effect.
        * \param soundKey The unique key identifying the sound effect to release.
        */
        void Release_Sound(const std::string& soundKey);

        /*!
        * \brief Adjusts the volume of the specified music channel.
        * \param pChannel Pointer to the FMOD channel for the music.
        * \param volume The desired volume level (range: 0.0 to 1.0).
        */
        void AdjustMusicVolume(FMOD::Channel* pChannel, float volume);

        /*!
        * \brief Stops playback of the current music smoothly over the given fade duration.
        * \param fadeDuration The time in seconds over which the music should fade out.
        */
        void Stop_Music(float fadeDuration);

        /*!
        * \brief Stops playback of a sound effect associated with the specified key.
        * \param soundKey The unique key identifying the sound effect to stop.
        */
        void Stop_Sound(const std::string& soundKey);

        /*!
        * \brief Stops playback of the specified music track and releases its resources.
        * \param musicKey The unique key identifying the music track to stop and release.
        */
        void StopAndReleaseMusic(const std::string& musicKey);

        /*!
        * \brief Retrieves the map of sound channels currently managed by the audio system.
        * \return A reference to the map of sound channels keyed by their unique identifiers.
        */
        std::map<std::string, FMOD::Channel*>& GetChannelMap() { return soundChannels; }

        /*!
        * \brief Retrieves the FMOD system instance.
        * \return Pointer to the FMOD system used for audio management.
        */
        FMOD::System* GetSystem() { return pSystem; }

        /*!
        * \brief Initializes the audio system.
        * Sets up the FMOD system, prepares audio resources, and initializes sound channels.
        */
        void Initialize() override;

        /*!
        * \brief Updates the audio system every frame.
        * Performs tasks such as updating sound playback states and managing dynamic resources.
        * \param deltaTime The time elapsed since the last update, in seconds.
        */
        void Update(float deltaTime) override;

        /*!
        * \brief Pauses all currently active audio playback.
        * Useful for suspending game audio during certain game states (e.g., pause menu).
        */
        void PauseAll();

        /*!
        * \brief Resumes playback of all paused audio.
        * Restores game audio to its active state after being paused.
        */
        void ResumeAll();

        /*!
        * \brief Transitions smoothly from the current music to new music over the given fade duration.
        * \param newMusicKey The unique key identifying the new music track.
        * \param fadeDuration The time in seconds over which the crossfade occurs.
        */
        void TransitionMusic(const std::string& newMusicKey, float fadeDuration);

    public:
        static float BGMVolume;
        static float BGMFadeDuration;

        static void SetBGMVolume(float volume) { BGMVolume = volume; }
        static void SetBGMFadeDuration(float duration) { BGMFadeDuration = duration; }

    private:
        FMOD::System* pSystem;
        static std::map<std::string, FMOD::Channel*> soundChannels;
        std::string currentMusicKey;

        // Variables for crossfade transition.
        bool isTransitioning = false;
        float transitionTimer = 0.0f;
        float transitionDuration = 0.0f;
        std::string newMusicKeyTransition;
        FMOD::Channel* newMusicChannel = nullptr;

        // Variables for smooth stopping of music.
        bool isStoppingMusic = false;
        float stoppingTimer = 0.0f;
        float stoppingDuration = 0.0f;

        bool isFadingInMusic = false;
    };

    extern AudioSystem* audioSystem;
}