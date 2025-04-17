/******************************************************************************
/*!
\file  GameObserverPattern.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    Feb 03, 2024
\brief  Implements the camera utils for shaking the camera.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

using System;

namespace FruitLoops
{
    public class CameraShake
    {
        private static bool isShaking = false;
        private static float shakeDuration = 0f;
        private static float shakeIntensity = 0f;
        private static float shakeTimer = 0f;
        private static Vector2D originalPosition = new Vector2D(0, 0);
        private static Entity cameraEntity = null;

        /**
         * @brief Initiates a camera shake effect with specified duration and intensity.
         * 
         * @param duration The duration of the shake effect in seconds.
         * @param intensity The magnitude of the shake effect.
         * @note Locates the camera entity by component, stores its original position,
         * and starts the shake if not already active.
         */
        public static void Shake(float duration, float intensity)
        {
            if (!isShaking)
            {
                // Find the camera entity by component instead of by name
                foreach (var entity in Entity.GetEntitiesWithECSComponentName("struct FruitLoops::CameraComponent"))
                {
                    cameraEntity = entity;
                    break; // Assume first camera is the main camera
                }

                if (cameraEntity == null || cameraEntity.ID == 0)
                {
                    Debug.LogError("[CameraShake] No entity with CameraComponent found.");
                    return;
                }

                // Store original camera position
                InternalCalls.Transform_GetTranslation(cameraEntity.ID, out originalPosition);

                // Set shake parameters
                shakeDuration = duration;
                shakeIntensity = intensity;
                shakeTimer = 0f;
                isShaking = true;

                Debug.LogInfo("[CameraShake] Started camera shake: duration={0}, intensity={1}", duration, intensity);
            }
        }

        /**
         * @brief Updates the camera shake effect based on elapsed time.
         * 
         * @param deltaTime The time elapsed since the last update in seconds.
         * @note Applies a random offset to the camera position during the shake duration,
         * gradually reducing intensity, and resets the position when complete.
         */
        public static void Update(float deltaTime)
        {
            if (isShaking)
            {
                shakeTimer += deltaTime;

                if (shakeTimer < shakeDuration)
                {
                    // Calculate shake strength (gradually decreases over time)
                    float currentIntensity = shakeIntensity * (1 - (shakeTimer / shakeDuration));

                    // Generate random offset using System.Random instead of UnityEngine
                    Random random = new Random();
                    float offsetX = ((float)random.NextDouble() * 2 - 1) * currentIntensity;
                    float offsetY = ((float)random.NextDouble() * 2 - 1) * currentIntensity;

                    // Apply offset to camera
                    Vector2D newPosition = new Vector2D(
                        originalPosition.X + offsetX,
                        originalPosition.Y + offsetY
                    );

                    InternalCalls.Transform_SetTranslation(cameraEntity.ID, ref newPosition);
                }
                else
                {
                    // Shake complete, reset camera position
                    InternalCalls.Transform_SetTranslation(cameraEntity.ID, ref originalPosition);
                    isShaking = false;
                }
            }
        }

        // To avoid creating a new Random instance every frame
        private static readonly Random _random = new Random();

        /**
         * @brief Generates a random value between -1 and 1.
         * 
         * @return A float value between -1 and 1.
         * @note Uses a static Random instance to avoid repeated instantiation.
         */
        private static float GetRandomValue()
        {
            return ((float)_random.NextDouble() * 2f) - 1f;
        }
    }
}