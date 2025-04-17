/******************************************************************************
/*!
\file  HealthBarComponent.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    Feb 03, 2024
\brief  This file implements the HealthBarComponent class, which manages the UI
       for an entity's health bar. It updates the health bar filler and empty components
       based on the target entity's current health and provides functionality to hide
       the health bar UI.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{
    /**
     * @brief Manages the health bar UI for an entity.
     *
     * The HealthBarComponent updates the scale and position of the health bar filler
     * based on the target entity's current health. It also restores the empty bar's scale
     * and provides a method to hide the health bar.
     */
    public class HealthBarComponent : Component
    {
        private Entity _targetEntity;
        private Entity _healthBarFiller;
        private Entity _healthBarEmpty;
        //private Entity _attackSymbol;
		//private Entity _rangeSymbol;
		private Vector2D _initialEmptyScale;
        private Vector2D _initialFillerScale;
        private Vector2D _initialFillerPosition;

        /**
         * @brief Constructs a new HealthBarComponent.
         *
         * Initializes the component with the target entity and the entities representing the
         * health bar filler and empty parts. The initial scale and position of the health bar entities
         * are stored for later use.
         *
         * @param initialTarget The entity whose health will be displayed.
         * @param healthBarFiller The entity representing the filler part of the health bar.
         * @param healthBarEmpty The entity representing the empty part of the health bar.
         */
        public HealthBarComponent(Entity initialTarget, Entity healthBarFiller, Entity healthBarEmpty)
        {
            _targetEntity = initialTarget;
            _healthBarFiller = healthBarFiller;
            _healthBarEmpty = healthBarEmpty;

            // Store the initial scale and position of the health bar entities
            InternalCalls.Transform_GetScale(_healthBarEmpty.ID, out _initialEmptyScale);
            //Debug.LogInfo("[Enemy] HealthBar empty initialised with scale {0}, {1}", _initialEmptyScale.X, _initialEmptyScale.Y);
            InternalCalls.Transform_GetScale(_healthBarFiller.ID, out _initialFillerScale);
            //Debug.LogInfo("[Enemy] HealthBar filler initialised with scale {0}, {1}", _initialFillerScale.X, _initialFillerScale.Y);
            InternalCalls.Transform_GetTranslation(_healthBarFiller.ID, out _initialFillerPosition);
        }

        /**
         * @brief Sets the target entity for the health bar.
         *
         * Updates the entity whose health will be represented by the health bar.
         *
         * @param target The new target entity.
         */
        public void SetTarget(Entity target)
        {
            _targetEntity = target;
        }

        /**
         * @brief Updates the health bar UI based on the target entity's current health.
         *
         * Restores the empty health bar's scale, calculates the current health percentage,
         * adjusts the filler scale accordingly, and repositions the filler so that the health
         * bar appears to shrink from right to left.
         */
        public void UpdateHealthBar()
        {
            if (_targetEntity == null)
                return;

            // Restore the empty asset's scale to its initial value
            InternalCalls.Transform_SetScale(_healthBarEmpty.ID, ref _initialEmptyScale);
            int currentHealth = InternalCalls.UnitComponent_GetHealth(_targetEntity.ID);
            int maxHealth = InternalCalls.UnitComponent_GetMaxHealth(_targetEntity.ID);
            float healthPercentage = MathUtils.Clamp((float)currentHealth / maxHealth, 0.0f, 1.0f);

            // Calculate the new scale for the health bar filler based on the health percentage
            Vector2D newFillerScale = new Vector2D(_initialFillerScale.X * healthPercentage, _initialFillerScale.Y);
            InternalCalls.Transform_SetScale(_healthBarFiller.ID, ref newFillerScale);

            // Calculate the new position for the health bar filler (so it "shrinks" from right to left)
            float offsetX = (_initialFillerScale.X - newFillerScale.X) / 2.0f;
            Vector2D newPosition = new Vector2D(_initialFillerPosition.X - offsetX, _initialFillerPosition.Y);
            InternalCalls.Transform_SetTranslation(_healthBarFiller.ID, ref newPosition);
        }

        /**
         * @brief Hides the health bar UI.
         *
         * Sets the scale of both the health bar filler and empty entities to zero, effectively
         * hiding the UI.
         */
        public void HideUI()
        {
            Vector2D zeroScale = new Vector2D(0, 0);
            InternalCalls.Transform_SetScale(_healthBarFiller.ID, ref zeroScale);
            InternalCalls.Transform_SetScale(_healthBarEmpty.ID, ref zeroScale);
        }
    }
}
