/******************************************************************************/
/*!
\file  Audio.cpp
\Proj name  Shroomy Doomy
\author  Reagan Tang
\date    Oct 03, 2024
\brief   This file contains the definitions for the AudioSystem class
         and related functions, responsible for managing all sound and music
         playback in the game. It includes initialization of the FMOD system,
         playing, stopping, and releasing sounds and music tracks, along with
         handling dynamic sound channel management.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
#include "Audio.h"
#include "pch.h"


namespace FruitLoops {

    // Define the static map for tracking sound channels.
    std::map<std::string, FMOD::Channel*> AudioSystem::soundChannels;
    AudioSystem* audioSystem = nullptr;

    float AudioSystem::BGMVolume = 0.1f;       // Default BGM volume
    float AudioSystem::BGMFadeDuration = 2.0f;  // Default fade time

    //=============================================================================
    // Constructor
    //=============================================================================
    AudioSystem::AudioSystem() {
        pSystem = nullptr;
        FMOD::System_Create(&pSystem);
        pSystem->init(32, FMOD_INIT_NORMAL, nullptr);
        audioSystem = this;
    }

    //=============================================================================
    // Destructor
    //=============================================================================
    AudioSystem::~AudioSystem() {
        // Stop and release various sound effects.
      
        // Cooking SFX
        Stop_Sound("SFX_Bubbling");
        Release_Sound("SFX_Bubbling");

        Stop_Sound("SFX_FoodReady");
        Release_Sound("SFX_FoodReady");

        Stop_Sound("SFX_Cooking");
        Release_Sound("SFX_Cooking");

        // Button SFX
        Stop_Sound("SFX_ButtonClick");
        Release_Sound("SFX_ButtonClick");

        // Mouse SFX
        Stop_Sound("SFX_Mouse_v1");
        Release_Sound("SFX_Mouse_v1");

        // Tile SFX
        Stop_Sound("SFX_TileHover");
        Release_Sound("SFX_TileHover ");

        // Chest SFX
        Stop_Sound("SFX_ChestOpen");
        Release_Sound("SFX_ChestOpen");

        // Enenmy SFX (Attack)
        Stop_Sound("SFX_Enemy_Get_Attacked_1");
        Release_Sound("SFX_Enemy_Get_Attacked_1");
        Stop_Sound("SFX_Enemy_Get_Attacked_2");
        Release_Sound("SFX_Enemy_Get_Attacked_2");
        Stop_Sound("SFX_Enemy_Get_Attacked_3");
        Release_Sound("SFX_Enemy_Get_Attacked_3");
        Stop_Sound("SFX_Enemy_Get_Attacked_4");
        Release_Sound("SFX_Enemy_Get_Attacked_4");

		// Enemy SFX (Move)
        Stop_Sound("SFX_EnemyMove_1");
        Release_Sound("SFX_EnemyMove_1");
        Stop_Sound("SFX_EnemyMove_2");
        Release_Sound("SFX_EnemyMove_2");
        Stop_Sound("SFX_EnemyMove_3");
        Release_Sound("SFX_EnemyMove_3");

		// Enemy SFX (Attack)
        Stop_Sound("SFX_Enemy1_Attack_01");
        Release_Sound("SFX_Enemy1_Attack_01");
        Stop_Sound("SFX_Enemy1_Attack_02");
        Release_Sound("SFX_Enemy1_Attack_02");
        Stop_Sound("SFX_Enemy1_Attack_03");
        Release_Sound("SFX_Enemy1_Attack_03");
        Stop_Sound("SFX_Enemy1_Attack_04");
        Release_Sound("SFX_Enemy1_Attack_04");
        Stop_Sound("SFX_Enemy1_Attack_05");
        Release_Sound("SFX_Enemy1_Attack_05");
        Stop_Sound("SFX_Enemy1_Attack_06");
        Release_Sound("SFX_Enemy1_Attack_06");
        Stop_Sound("SFX_Enemy1_Attack_07");
        Release_Sound("SFX_Enemy1_Attack_07");
        Stop_Sound("SFX_Enemy1_Attack_08");
        Release_Sound("SFX_Enemy1_Attack_08");
        Stop_Sound("SFX_Enemy1_Attack_09");
        Release_Sound("SFX_Enemy1_Attack_09");
        Stop_Sound("SFX_Enemy1_Attack_10");
        Release_Sound("SFX_Enemy1_Attack_10");

        Stop_Sound("SFX_Enemy2_Attack_01");
        Release_Sound("SFX_Enemy2_Attack_01");
        Stop_Sound("SFX_Enemy2_Attack_02");
        Release_Sound("SFX_Enemy2_Attack_02");
        Stop_Sound("SFX_Enemy2_Attack_03");
        Release_Sound("SFX_Enemy2_Attack_03");
        Stop_Sound("SFX_Enemy2_Attack_04");
        Release_Sound("SFX_Enemy2_Attack_04");
        Stop_Sound("SFX_Enemy2_Attack_05");
        Release_Sound("SFX_Enemy2_Attack_05");
        Stop_Sound("SFX_Enemy2_Attack_06");
        Release_Sound("SFX_Enemy2_Attack_06");
        Stop_Sound("SFX_Enemy2_Attack_07");
        Release_Sound("SFX_Enemy2_Attack_07");
        Stop_Sound("SFX_Enemy2_Attack_08");
        Release_Sound("SFX_Enemy2_Attack_08");
        Stop_Sound("SFX_Enemy2_Attack_09");
        Release_Sound("SFX_Enemy2_Attack_09");
        Stop_Sound("SFX_Enemy2_Attack_10");
        Release_Sound("SFX_Enemy2_Attack_10");

		// Enemy SFX (Death)
        Stop_Sound("SFX_Enemy1_Death_01");
        Release_Sound("SFX_Enemy1_Death_01");
        Stop_Sound("SFX_Enemy1_Death_02");
        Release_Sound("SFX_Enemy1_Death_02");
        Stop_Sound("SFX_Enemy1_Death_03");
        Release_Sound("SFX_Enemy1_Death_03");
        Stop_Sound("SFX_Enemy1_Death_04");
        Release_Sound("SFX_Enemy1_Death_04");

        Stop_Sound("SFX_Enemy2_Death_01");
        Release_Sound("SFX_Enemy2_Death_01");
        Stop_Sound("SFX_Enemy2_Death_02");
        Release_Sound("SFX_Enemy2_Death_02");
        Stop_Sound("SFX_Enemy2_Death_03");
        Release_Sound("SFX_Enemy2_Death_03");
        Stop_Sound("SFX_Enemy2_Death_04");
        Release_Sound("SFX_Enemy2_Death_04");

        // Game Progress SFX
        Stop_Sound("SFX_GameOver");
        Release_Sound("SFX_GameOver");

        Stop_Sound("SFX_WinGame");
        Release_Sound("SFX_WinGame");

        // Player SFX (Move)
        Stop_Sound("SFX_PlayerMove_Jump_1");
        Release_Sound("SFX_PlayerMove_Jump_1");
        Stop_Sound("SFX_PlayerMove_Jump_2");
        Release_Sound("SFX_PlayerMove_Jump_2");
        Stop_Sound("SFX_PlayerMove_Jump_3");
        Release_Sound("SFX_PlayerMove_Jump_3");
        Stop_Sound("SFX_PlayerMove_Jump_4");
        Release_Sound("SFX_PlayerMove_Jump_4");

        Stop_Sound("SFX_PlayerMove_Land");
        Release_Sound("SFX_PlayerMove_Land");

        // Stop and release music tracks.
        StopAndReleaseMusic("BGM_ShroomyDoomy_Level1_v1");
        StopAndReleaseMusic("BGM_ShroomyDoomy_Level2_v1");
        StopAndReleaseMusic("BGM_ShroomyDoomy_Tutorial_v1");
        StopAndReleaseMusic("BGM_ShroomyDoomy_MainMenu_v1");

        audioSystem = nullptr;
    }

    //=============================================================================
    // Play_Sound: Plays a sound effect.
    //=============================================================================
    void AudioSystem::Play_Sound(const std::string& soundKey, float volume) {
        FMOD::Sound* pSound = assetsManager->GetSounds(soundKey);
        if (!pSound) {
#ifdef DEBUG
            std::cerr << "Sound not found for key: " << soundKey << std::endl;
#endif
            return;
        }
        FMOD::Channel* pChannel = nullptr;
        // Check if the sound is already playing.
        auto it = soundChannels.find(soundKey);
        if (it != soundChannels.end() && it->second) {
            bool isPlaying = false;
            it->second->isPlaying(&isPlaying);
            if (isPlaying) {
                it->second->stop();
                it->second->setVolume(volume);
            }
        }
        pSystem->playSound(pSound, nullptr, false, &pChannel);
        soundChannels[soundKey] = pChannel;
        if (pChannel) {
            pChannel->setVolume(volume);
        }
    }

    //=============================================================================
    // Play_Music: Plays background music.
    //=============================================================================
    void AudioSystem::Play_Music(const std::string& musicKey, float fadeDuration, float volume) {
        if (!currentMusicKey.empty()) {
            // If there's already music, perform a crossfade transition.
            TransitionMusic(musicKey, fadeDuration > 0 ? fadeDuration : BGMFadeDuration);
            return;
        }
        FMOD::Sound* pSound = assetsManager->GetSounds(musicKey);
        if (!pSound) {
#ifdef _DEBUG
            spdlog::error("SmoothlyPlayMusic: Failed to load sound: {}", musicKey);
#endif
            return;
        }
        pSound->setMode(FMOD_LOOP_NORMAL);
        FMOD::Channel* pChannel = nullptr;
        // Play the new sound in a paused state so we can set its volume.
        pSystem->playSound(pSound, nullptr, true, &pChannel);
        if (!pChannel)
            return;
        pChannel->setVolume(volume > 0 ? volume : BGMVolume);
        pChannel->setPaused(false);
        soundChannels[musicKey] = pChannel;
        currentMusicKey = musicKey;
        // Set up fade-in parameters.
        isFadingInMusic = true;
        transitionTimer = 0.0f;
        transitionDuration = fadeDuration;
        newMusicChannel = pChannel;
        newMusicKeyTransition = musicKey;
    }

    //=============================================================================
    // Release_Sound: Releases a sound from memory.
    //=============================================================================
    void AudioSystem::Release_Sound(const std::string& soundKey) {
        FMOD::Sound* pSound = assetsManager->GetSounds(soundKey);
        if (pSound) {
            pSound->release();
            pSound = nullptr;
        }
    }

    //=============================================================================
    // AdjustMusicVolume: Adjusts the volume of a given channel.
    //=============================================================================
    void AudioSystem::AdjustMusicVolume(FMOD::Channel* pChannel, float volume) {
        UNUSED_PARAM(volume);
        if (pChannel) {
            pChannel->setVolume(BGMVolume);
        }
    }

    //=============================================================================
    // Stop_Music: Smoothly stops the current music over the specified fade duration.
    // If fadeDuration is zero or less, it stops immediately.
    //=============================================================================
    void AudioSystem::Stop_Music(float fadeDuration) {
        if (currentMusicKey.empty())
            return;
        float fadeTime = (fadeDuration >= 0) ? fadeDuration : BGMFadeDuration;
        if (fadeTime <= 0.0f) {
            auto it = soundChannels.find(currentMusicKey);
            if (it != soundChannels.end() && it->second) {
                it->second->stop();
                soundChannels.erase(it);
            }
            currentMusicKey = "";
            return;
        }
        isStoppingMusic = true;
        stoppingTimer = 0.0f;
        stoppingDuration = fadeTime;
    }

    //=============================================================================
    // Stop_Sound: Stops a sound effect.
    //=============================================================================
    void AudioSystem::Stop_Sound(const std::string& soundKey) {
        auto it = soundChannels.find(soundKey);
        if (it != soundChannels.end() && it->second) {
            it->second->stop();
            it->second = nullptr;
        }
    }

    //=============================================================================
    // StopAndReleaseMusic: Stops and releases a music track.
    //=============================================================================
    void AudioSystem::StopAndReleaseMusic(const std::string& musicKey) {
        // Here we can call Stop_Music with a zero fade (immediate stop)
        Stop_Music(0.0f);
        Release_Sound(musicKey);
    }

    //=============================================================================
    // Initialize: Sets up the audio system (e.g., starts background music).
    //=============================================================================
    void AudioSystem::Initialize() {
        Play_Music(ConfigData::BGM, BGMFadeDuration, BGMVolume);  // Ensure ConfigData::BGM is defined in your project.
    }

    //=============================================================================
    // Update: Called every frame to update audio transitions.
    //=============================================================================
    void AudioSystem::Update(float deltaTime) {
        // Handle crossfade transition if active.
        if (isTransitioning) {
            transitionTimer += deltaTime;
            float t = transitionTimer / transitionDuration;
            /*if (t > 0.15f)
                t = 0.15f;*/
			t = std::min(t, BGMVolume);
            // Fade out current music.
            auto it = soundChannels.find(currentMusicKey);
            if (it != soundChannels.end() && it->second) {
                it->second->setVolume(BGMVolume - t);
            }
            // Fade in new music.
            if (newMusicChannel) {
                newMusicChannel->setVolume(t);
            }
            if (transitionTimer >= transitionDuration) {
                if (it != soundChannels.end() && it->second) {
                    it->second->stop();
                    soundChannels.erase(it);
                }
                soundChannels[newMusicKeyTransition] = newMusicChannel;
                currentMusicKey = newMusicKeyTransition;
                isTransitioning = false;
                transitionTimer = 0.0f;
                transitionDuration = 0.0f;
                newMusicKeyTransition = "";
                newMusicChannel = nullptr;
            }
        }

        // Handle smooth music stop if requested.
        if (isStoppingMusic) {
            stoppingTimer += deltaTime;
            float t = stoppingTimer / stoppingDuration;
            /*if (t > 0.15f)
                t = 0.15f;*/
            t = std::min(t, BGMVolume);
            auto it = soundChannels.find(currentMusicKey);
            if (it != soundChannels.end() && it->second) {
                it->second->setVolume(BGMVolume - t);
            }
            if (stoppingTimer >= stoppingDuration) {
                if (it != soundChannels.end() && it->second) {
                    it->second->stop();
                    soundChannels.erase(it);
                }
                currentMusicKey = "";
                isStoppingMusic = false;
            }
        }

        // Handle simple fade-in (when no previous music exists).
        if (isFadingInMusic) {
            transitionTimer += deltaTime;
            float t = transitionTimer / transitionDuration;
            /*if (t > 0.15f)
                t = 0.15f;*/
            t = std::min(t, BGMVolume);
            if (newMusicChannel) {
                newMusicChannel->setVolume(t);
            }
            if (transitionTimer >= transitionDuration) {
                isFadingInMusic = false;
                transitionTimer = 0.0f;
                transitionDuration = 0.0f;
                newMusicKeyTransition = "";
                newMusicChannel = nullptr;
            }
        }

        // Update the FMOD system.
        if (pSystem) {
            pSystem->update();
        }
    }

    //=============================================================================
    // PauseAll: Pauses all active audio channels.
    //=============================================================================
    void AudioSystem::PauseAll() {
#ifdef DEBUG
        logAudio("Pausing all audio channels...");
#endif
        for (auto& [soundKey, channel] : soundChannels) {
            if (channel) {
                if (channel->setPaused(true) == FMOD_OK) {
#ifdef DEBUG
                    logAudio("Paused sound key: {}", soundKey);
#endif
                }
                else {
#ifdef DEBUG
                    logError("Failed to pause sound key: {}.", soundKey);
#endif
                }
            }
            else {
#ifdef DEBUG
                logError("Channel for sound key '{}' is null or invalid.", soundKey);
#endif
            }
        }
    }

    //=============================================================================
    // ResumeAll: Resumes playback on all paused audio channels.
    //=============================================================================
    void AudioSystem::ResumeAll() {
#ifdef DEBUG
        logAudio("Resuming all audio channels...");
#endif
        for (auto& [soundKey, channel] : soundChannels) {
            if (channel) {
                bool isPaused = false;
                if (channel->getPaused(&isPaused) == FMOD_OK && isPaused) {
                    if (channel->setPaused(false) == FMOD_OK) {
#ifdef DEBUG
                        logAudio("Resumed sound key: {}", soundKey);
#endif
                    }
                    else {
#ifdef DEBUG
                        logError("Failed to resume sound key: {}.", soundKey);
#endif
                    }
                }
                else if (!isPaused) {
#ifdef DEBUG
                    logAudio("Sound key '{}' is already playing.", soundKey);
#endif
                }
            }
            else {
#ifdef DEBUG
                logError("Channel for sound key '{}' is null or invalid.", soundKey);
#endif
            }
        }
    }

    //=============================================================================
    // TransitionMusic: Smoothly crossfades from the current music to new music.
    //=============================================================================
    void AudioSystem::TransitionMusic(const std::string& newMusicKey, float fadeDuration) {
        // If the new music is the same as the current, do nothing.
        if (newMusicKey == currentMusicKey)
            return;

        FMOD::Sound* pNewSound = assetsManager->GetSounds(newMusicKey);
        if (!pNewSound) {
#ifdef _DEBUG
            spdlog::error("TransitionMusic: Failed to load sound: {}", newMusicKey);
#endif
            return;
        }
        pNewSound->setMode(FMOD_LOOP_NORMAL);
        FMOD::Channel* pNewChannel = nullptr;
        // Play the new sound in a paused state so we can set its volume.
        pSystem->playSound(pNewSound, nullptr, true, &pNewChannel);
        if (!pNewChannel) {
#ifdef _DEBUG
            spdlog::error("TransitionMusic: Failed to obtain a channel for: {}", newMusicKey);
#endif
            return;
        }
        pNewChannel->setVolume(0.0f);
        pNewChannel->setPaused(false);

        newMusicKeyTransition = newMusicKey;
        newMusicChannel = pNewChannel;
        transitionDuration = fadeDuration;
        transitionTimer = 0.0f;
        isTransitioning = true;
    }

}