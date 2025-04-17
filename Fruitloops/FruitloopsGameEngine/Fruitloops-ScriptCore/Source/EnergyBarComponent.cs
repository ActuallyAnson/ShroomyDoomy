/******************************************************************************
/*!
\file  EnergyBarComponent.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    Feb 03, 2025
\brief  This file implements the EnergyBarComponent class, which manages the UI for
       an entity's energy bar. It updates the energy bar filler and empty components
       based on the entity's current energy.
       
All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{
    /**
     * @brief Manages the energy bar UI for an entity.
     *
     * The EnergyBarComponent updates the scale and position of the energy bar filler
     * based on the entity's current energy level. It also provides methods to set the target
     * entity and to hide the energy bar UI.
     */
    public class EnergyBarComponent : Component
    {
        private Entity _targetEntity;
        private Entity _energyBarFiller;
        private Entity _energyBarEmpty;
        private Vector2D _initialEmptyScale;
        private Vector2D _initialFillerScale;
        private Vector2D _initialFillerPosition;
        private int _previewEnergyCost = 0;


        /**
         * @brief Constructs a new EnergyBarComponent.
         *
         * Initializes the energy bar UI by storing the initial scale and position of the
         * energy bar filler and empty entities.
         *
         * @param initialTarget The initial target entity whose energy is tracked.
         * @param energyBarFiller The energy bar filler entity.
         * @param energyBarEmpty The empty energy bar entity.
         */
        public EnergyBarComponent(Entity initialTarget, Entity energyBarFiller, Entity energyBarEmpty)
        {
            _targetEntity = initialTarget;
            _energyBarFiller = energyBarFiller;
            _energyBarEmpty = energyBarEmpty;

            // Store the initial scale and position of the energy bar entities

            InternalCalls.Transform_GetScale(_energyBarEmpty.ID, out _initialEmptyScale);
            InternalCalls.Transform_GetScale(_energyBarFiller.ID, out _initialFillerScale);
            InternalCalls.Transform_GetTranslation(_energyBarFiller.ID, out _initialFillerPosition);
        }

        /**
         * @brief Sets the target entity for the energy bar.
         *
         * @param target The entity whose energy should be displayed.
         */
        public void SetTarget(Entity target)
        {
            _targetEntity = target;
        }

        /**
         * @brief Updates the energy bar UI based on the target entity's energy.
         *
         * Restores the empty bar's scale, calculates the energy percentage, adjusts the filler scale,
         * and repositions the filler so that it shrinks from right to left.
         */
        /**
         * @brief Updates the energy bar UI based on the target entity's energy.
         * 
         * @param previewEnergyCost Optional parameter showing how much energy would be used
         */
        public void UpdateEnergyBar(int previewEnergyCost = 0)
        {
            if (_targetEntity == null || _energyBarFiller == null || _energyBarEmpty == null)
                return;

            // Store the preview value for later use
            _previewEnergyCost = previewEnergyCost;

            // Restore the empty bar's original scale
            InternalCalls.Transform_SetScale(_energyBarEmpty.ID, ref _initialEmptyScale);

            int currentEnergy = InternalCalls.UnitComponent_GetEnergy(_targetEntity.ID);
            int maxEnergy = InternalCalls.UnitComponent_GetMaxEnergy(_targetEntity.ID);

            if (maxEnergy <= 0)
                return;

            // Calculate the energy percentage based on actual energy
            float energyPercentage;

            if (previewEnergyCost > 0)
            {
                // Show a preview of what the energy would be after the action
                int previewRemaining = Math.Max(0, currentEnergy - previewEnergyCost);
                energyPercentage = (float)previewRemaining / maxEnergy;

                // Could add visual indicator here like different color
            }
            else
            {
                // Normal display of current energy
                energyPercentage = (float)currentEnergy / maxEnergy;
            }

            // Scale the filler to match energy percentage
            Vector2D newFillerScale = new Vector2D(
                _initialFillerScale.X * energyPercentage,
                _initialFillerScale.Y
            );

            // Repositioning to handle right-aligned bar
            Vector2D newFillerPosition = new Vector2D(
                _initialFillerPosition.X - (_initialFillerScale.X - newFillerScale.X) / 2,
                _initialFillerPosition.Y
            );

            // Apply new scale and position
            InternalCalls.Transform_SetScale(_energyBarFiller.ID, ref newFillerScale);
            InternalCalls.Transform_SetTranslation(_energyBarFiller.ID, ref newFillerPosition);
        }

        /**
         * @brief Hide the energy bar UI based on the target entity's energy.
         * 
         */
        public void HideUI()
        {
            Vector2D zeroScale = Vector2D.Zero;
            InternalCalls.Transform_SetScale(_energyBarFiller.ID, ref zeroScale);
            InternalCalls.Transform_SetScale(_energyBarEmpty.ID, ref zeroScale);
        }
    }
}