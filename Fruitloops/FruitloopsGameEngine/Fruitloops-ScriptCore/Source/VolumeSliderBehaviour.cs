/******************************************************************************
/*!
\file  VolumeSliderBehaviour.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date   1 April, 2025
\brief  This file implements the VolumeSliderBehaviour class, a behaviour component
       that controls the volume slider in the game. The VolumeSliderBehaviour class
       handles user input for adjusting the volume, updating the visual fill bar,
       and setting the background music volume.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{
    public class VolumeSliderBehaviour : BehaviourComponent
    {
        private Entity _sliderFill;
        private Entity _sliderTrack;
        private float _currentVolume = 0.5f;
        private float _sliderWidth;
        private bool _isDragging = false;
        private bool _clickProcessed = false;
        private ButtonComponent _localButtonComponent;
        private Vector2D _originalTrackScale;
        private Vector2D _originalFillScale;
        private Vector2D _trackPosition;

        // Default constructor required by Mono
        public VolumeSliderBehaviour() { }

        /**
       * @brief Constructs a new VolumeSliderBehaviour for the specified entity.
       *
       * @param entity The entity to which this behavior is attached.
       */
        public VolumeSliderBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[VolumeSlider] Creating VolumeSliderBehaviour for entity {0}", entity.ID);

                // Add button component for click behavior
                if (!entity.HasLocalComponent<ButtonComponent>())
                {
                    _localButtonComponent = entity.AddLocalComponent(new ButtonComponent());
                    Debug.LogInfo("[VolumeSlider] Added ButtonComponent to entity {0}", entity.ID);
                }
                else
                {
                    _localButtonComponent = entity.GetLocalComponent<ButtonComponent>();
                    Debug.LogInfo("[VolumeSlider] Retrieved existing ButtonComponent from entity {0}", entity.ID);
                }

                _sliderTrack = entity;
                Debug.LogInfo("[VolumeSlider] Set _sliderTrack to entity {0}", entity.ID);

                // Try to find the fill entity directly by name using GetEntityByName
                try
                {
                    Debug.LogInfo("[VolumeSlider] Looking for fill entity directly by ID");

                    // Try a range of entity IDs to find one with the right name
                    for (ulong id = 1; id < 100; id++)
                    {
                        try
                        {
                            string entityName = InternalCalls.Entity_GetEntityName(id);
                            if (entityName == "PauseMenu_BGMSliderFill")
                            {
                                _sliderFill = new Entity(id);
                                Debug.LogInfo("[VolumeSlider] Found fill entity with ID {0}", id);
                                break;
                            }
                        }
                        catch { /* Ignore errors for non-existent entities */ }
                    }
                }
                catch (Exception ex)
                {
                    Debug.LogWarning("[VolumeSlider] Error searching for fill entity: {0}", ex.Message);
                }

                if (_sliderFill == null)
                {
                    Debug.LogWarning("[VolumeSlider] Could not find fill entity. Slider will work without visual feedback.");
                }

                // Store original properties
                Debug.LogInfo("[VolumeSlider] Getting track scale");
                InternalCalls.Transform_GetScale(_sliderTrack.ID, out _originalTrackScale);

                Debug.LogInfo("[VolumeSlider] Getting track position");
                InternalCalls.Transform_GetTranslation(_sliderTrack.ID, out _trackPosition);

                if (_sliderFill != null)
                {
                    Debug.LogInfo("[VolumeSlider] Getting fill scale");
                    try
                    {
                        InternalCalls.Transform_GetScale(_sliderFill.ID, out _originalFillScale);
                    }
                    catch (Exception ex)
                    {
                        Debug.LogError("[VolumeSlider] Error getting fill scale: {0}", ex.Message);
                        _sliderFill = null; // Reset fill reference since it's not usable
                    }
                }

                _sliderWidth = _originalTrackScale.X;
                Debug.LogInfo("[VolumeSlider] Slider width: {0}", _sliderWidth);

                // Get current volume
                Debug.LogInfo("[VolumeSlider] Getting current BGM volume");
                InternalCalls.Audio_GetBGMVolume(out _currentVolume);
                Debug.LogInfo("[VolumeSlider] Initialized with volume {0}", _currentVolume);

                // Register behavior
                Debug.LogInfo("[VolumeSlider] Registering behavior");
                var behaviour = new BehaviourFCT(entity, Init, Update, End);
                GameLogicSystem.Instance.RegisterLogicComponent(this);
                SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
                SetOwner(entity);
                Debug.LogInfo("[VolumeSlider] Constructor completed successfully");
            }
            catch (Exception ex)
            {
                Debug.LogError("[VolumeSlider] Exception in constructor: {0}\nStack trace: {1}", ex.Message, ex.StackTrace);
                throw; // Re-throw to ensure the error is properly reported
            }
        }
       
        /**
       * @brief Creates a new VolumeSliderBehaviour for the specified entity.
       *
       * @param entity The entity to which this behavior is attached.
       * @return The created VolumeSliderBehaviour instance.
       */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                return new VolumeSliderBehaviour(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[VolumeSlider] Exception: {0}", ex.Message);
                return null;
            }
        }

        /**
       * @brief Initializes the VolumeSliderBehaviour for the specified entity.
       *
       * @param entity The entity to initialize.
       */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[VolumeSlider] Initializing for entity {0}", entity.ID);
        }

        /**
       * @brief Updates the VolumeSliderBehaviour for the specified entity.
       *
       * @param entity The entity to update.
       * @param deltaTime The time elapsed since the last update.
       */
        public static void Update(Entity entity, float deltaTime)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<VolumeSliderBehaviour>(entity);
            if (instance == null || instance._localButtonComponent == null)
            {
                return;
            }

            // Check if pause menu is active
            bool isPauseMenuVisible = InternalCalls.LevelSwitch_GetLayerStatus("PauseMenuLayer");
            if (!isPauseMenuVisible)
            {
                return;
            }

            // Always update the fill bar every frame when pause menu is visible
            if (instance._sliderFill != null)
            {
                instance.UpdateFillBar();
            }

            try
            {
                bool isOverTrack = Input_IsMouseOverEntity(entity.ID);
                bool mouseDown = Input.IsMouseButtonDown(0);

                // Initial click or drag
                if (mouseDown)
                {
                    // Handle initial click
                    if (isOverTrack && !instance._clickProcessed)
                    {
                        instance._isDragging = true;
                        instance._clickProcessed = true;
                        InternalCalls.Audio_PlaySound("SFX_ButtonClick", 0.3f);
                    }

                    // Handle dragging
                    if (instance._isDragging)
                    {
                        Vector2D cursorPos;
                        InternalCalls.Input_GetCursorPosition(out cursorPos);
                        instance.UpdateVolumeFromMouseX(cursorPos.X);
                    }
                }
                else
                {
                    // Handle release
                    if (instance._isDragging)
                    {
                        instance._isDragging = false;
                        InternalCalls.Audio_PlaySound("SFX_ButtonClick", 0.2f);
                    }
                    instance._clickProcessed = false;
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[VolumeSlider] Error: {0}", ex.Message);
            }
        }

        /**
        * @brief Ends the VolumeSliderBehaviour for the specified entity.
        *
        * @param entity The entity to end.
        */
        public static void End(Entity entity)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<VolumeSliderBehaviour>(entity);
            if (instance == null)
            {
                return;
            }

            if (instance._localButtonComponent != null)
            {
                instance._localButtonComponent = null;
            }

            instance._isDragging = false;
            instance._sliderFill = null;
            instance._sliderTrack = null;
        }


        /**
        * @brief Updates the volume based on the mouse X position.
        *
        * @param mouseX The X position of the mouse cursor.
        */
        private void UpdateVolumeFromMouseX(float mouseX)
        {
            // Get track position and scale
            Vector2D trackPos;
            Vector2D trackScale;
            InternalCalls.Transform_GetTranslation(_sliderTrack.ID, out trackPos);
            InternalCalls.Transform_GetScale(_sliderTrack.ID, out trackScale);

            // calculate slider bounds
            float halfWidth = trackScale.X / 2.0f;
            float leftBound = trackPos.X - halfWidth;
            float rightBound = trackPos.X + halfWidth;

            // Clamp mouse position to slider bounds
            float clampedX = Math.Max(leftBound, Math.Min(rightBound, mouseX));

            // Calculate volume (0-1)
            _currentVolume = (clampedX - leftBound) / trackScale.X;
            _currentVolume = Math.Max(0.0f, Math.Min(1.0f, _currentVolume));

            // Update UI and audio
            if (_sliderFill != null)
            {
                UpdateFillBar();
            }

            InternalCalls.Audio_SetBGMVolume(_currentVolume);
            Debug.LogInfo("[VolumeSlider] Volume set to {0}", _currentVolume);
        }

        /**
        * @brief Updates the fill bar based on the current volume.
        */
        private void UpdateFillBar()
        {
            if (_sliderFill == null) return;

            try
            {
                // Get the slider track's current position and scale
                Vector2D trackPos;
                Vector2D trackScale;
                InternalCalls.Transform_GetTranslation(_sliderTrack.ID, out trackPos);
                InternalCalls.Transform_GetScale(_sliderTrack.ID, out trackScale);

                // Calculate the actual width of the track
                float trackWidth = trackScale.X;

                // Compute fill bar width based on volume (scaled to track width)
                float fillWidth = trackWidth * _currentVolume;

                // Only apply minimum width if volume is not zero
                // Use a percentage of track width for minimum instead of fixed pixels
                float minWidth = trackWidth * 0.02f; // 2% of track width
                if (_currentVolume > 0 && fillWidth < minWidth)
                    fillWidth = minWidth;

                //Debug.LogInfo("[VolumeSlider] Current volume: {0}, Fill width: {1}", _currentVolume, fillWidth);

                // Calculate the exact left edge of the track
                float trackLeftEdge = trackPos.X - (trackWidth / 2);
                //Debug.LogInfo("[VolumeSlider] Track left edge: {0}", trackLeftEdge);

                // Set new fill bar scale
                Vector2D newFillScale = new Vector2D(fillWidth, _originalFillScale.Y);
                InternalCalls.Transform_SetScale(_sliderFill.ID, ref newFillScale);

                // Position the fill bar so its left edge aligns exactly with the track's left edge
                float newFillPosX = trackLeftEdge + (fillWidth / 2);
                Vector2D newFillPos = new Vector2D(newFillPosX, trackPos.Y);
                InternalCalls.Transform_SetTranslation(_sliderFill.ID, ref newFillPos);

                Debug.LogInfo("[VolumeSlider] Track left edge: {0}, Updated fill bar - Width: {1}, Position: {2},{3}",
                    trackLeftEdge, fillWidth, newFillPosX, trackPos.Y);
            }
            catch (Exception ex)
            {
                Debug.LogError("[VolumeSlider] Error updating fill bar: {0}", ex.Message);
            }
        }

        /**
        * @brief Checks if the mouse cursor is over the specified entity.
        *
        * @param entityID The ID of the entity to check.
        * @return True if the mouse cursor is over the entity; otherwise, false.
        */
        private static bool Input_IsMouseOverEntity(ulong entityID)
        {
            Vector2D entityPos = new Vector2D();
            Vector2D entityScale = new Vector2D();

            try
            {
                InternalCalls.Transform_GetTranslation(entityID, out entityPos);
                InternalCalls.Transform_GetScale(entityID, out entityScale);
            }
            catch (Exception)
            {
                return false;
            }

            Vector2D cursorPos = Input.GetCursorPosition();

            float left = entityPos.X - (entityScale.X / 2);
            float right = entityPos.X + (entityScale.X / 2);
            float bottom = entityPos.Y - (entityScale.Y / 2);
            float top = entityPos.Y + (entityScale.Y / 2);

            return (cursorPos.X >= left && cursorPos.X <= right &&
                    cursorPos.Y >= bottom && cursorPos.Y <= top);
        }
    }
}

