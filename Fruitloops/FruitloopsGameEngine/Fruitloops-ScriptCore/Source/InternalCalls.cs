/******************************************************************************
/*!
\file  InternalCalls.cs
\Proj name  Shroomy Doomy
\author  Benjamin
\date    Feb 03, 2024
\brief  This file defines the InternalCalls static class, which provides a set
       of externally implemented functions for engine-level operations. These
       functions are used by managed code to interact with the underlying engine
       and perform tasks such as entity management, transformation, input handling,
       component manipulation, level switching, animation control, audio playback,
       and more.

All content � 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using FruitLoops;
using System;
using System.Runtime.CompilerServices;

namespace FruitLoops
{
	internal static class InternalCalls
	{

		/*---------------------------ENTITY---------------------------*/

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern object GetScriptInstance(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Entity_HasECSComponent(ulong entityID, Type componentType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern object Entity_GetECSComponent(ulong entityID, Type componentType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Entity_HasECSComponentByName(ulong entityID, string componentName);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern object Entity_GetECSComponentByName(ulong entityID, string componentName);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Entity_FindECSEntityByName(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong[] Entity_GetEntitiesWithECSComponentName(string componentTypeName);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_AddECSComponent(ulong entityID, string componentTypeName);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_SetScriptComponentName(ulong entityID, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong Entity_GetEntityByName(string name);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string Entity_GetEntityName(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Entity Entity_GetEntityByTextureName(string textureName);


        /*---------------------------TRANSFORM---------------------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_GetTranslation(ulong entityID, out Vector2D translation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_SetTranslation(ulong entityID, ref Vector2D translation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_GetTranslation(Entity entityID, out Vector2D translation);
        [MethodImpl(MethodImplOptions.InternalCall)]

        internal static extern void Transform_SetTranslation(Entity entityID, ref Vector2D translation);


        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_GetScale(ulong entityID, out Vector2D scale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_SetScale(ulong entityID, ref Vector2D scale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_GetScale(Entity entityID, out Vector2D scale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_SetScale(Entity entityID, ref Vector2D scale);


        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_GetOrientation(ulong entityID, out Vector2D orientation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_SetOrientation(ulong entityID, ref Vector2D orientation);

		/*---------------------------INPUT---------------------------*/

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Input_IsMouseButtonDown(int button);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Input_IsMouseOverEntity(ulong entity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsKeyPressed(int keyCode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsKeyReleased(int keyCode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsKeyHeld(int keyCode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsAnyMouseButtonPressed();

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Input_GetCursorPosition(out Vector2D cursorPosition);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern int Input_GetMouseScrollY();

		/*---------------------------BUTTON COMPONENT---------------------------*/

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool ButtonComponent_GetIsHovered(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void ButtonComponent_SetIsHovered(ulong entityID, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool ButtonComponent_GetIsClicked(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void ButtonComponent_SetIsClicked(ulong entityID, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool ButtonComponent_GetIsEnabled(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void ButtonComponent_SetIsEnabled(ulong entityID, bool value);

		/*---------------------------TILE COMPONENT---------------------------*/

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool TileComponent_GetIsWalkable(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TileComponent_SetIsWalkable(ulong entityID, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool TileComponent_GetIsHighlighted(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void TileComponent_SetIsHighlighted(ulong entityID, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool TileComponent_GetIsHighlightedEnemy(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void TileComponent_SetIsHighlightedEnemy(ulong entityID, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TileComponent_GetPosition(ulong entityID, out Vector2D position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TileComponent_SetPosition(ulong entityID, ref Vector2D position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float TileComponent_GetMovementCost(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TileComponent_SetMovementCost(ulong entityID, float cost);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong TileComponent_GetOccupant(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TileComponent_SetOccupant(ulong entityID, ulong occupantID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TileComponent_ClearNeighbors(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TileComponent_AddNeighbor(ulong entityID, ulong neighborID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong[] TileComponent_GetNeighbors(ulong entityID);


        /*---------------------------UNIT COMPONENT---------------------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
		public static extern string UnitComponent_GetUnitType(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_SetUnitType(ulong entityID, string unitType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern int UnitComponent_GetHealth(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_SetHealth(ulong entityID, int health);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern int UnitComponent_GetMaxHealth(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_SetMaxHealth(ulong entityID, int maxHealth);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern int UnitComponent_GetAttackPower(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_SetAttackPower(ulong entityID, int attackPower);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern int UnitComponent_GetEnergy(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_SetEnergy(ulong entityID, int energy);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern int UnitComponent_GetMaxEnergy(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_SetMaxEnergy(ulong entityID, int maxEnergy);
		

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern int UnitComponent_GetSpeed(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_SetSpeed(ulong entityID, int speed);
		
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool UnitComponent_GetIsAlive(ulong entityID);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_SetIsAlive(ulong entityID, bool value);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool UnitComponent_GetIsMoving(ulong entityID);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_SetIsMoving(ulong entityID, bool value);

		

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern Vector2D UnitComponent_GetPosition(ulong entityID, ref Vector2D position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_SetPosition(ulong entityID, ref Vector2D position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern ulong UnitComponent_GetOccupiedTile(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_SetOccupiedTile(ulong entityID, ulong tileID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void UnitComponent_ResetOccupiedTile(ulong entityID);
        /*---------------------------Material MANAGER---------------------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string MaterialComponent_GetTextureType(ulong entityID);

        /*---------------------------ANIMATION MANAGER---------------------------*/
        [MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void AnimationComponent_PlayAnimation(ulong entityID, string animationName);

		[MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AnimationComponent_Stop(string animationName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool AnimationComponent_CheckEntityPlaying(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void AnimationComponent_RemoveEntityFromAnimation(ulong entityID, string animationName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AnimationComponent_ChangeAnimationTexture(string originalName, string newName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string AnimationComponent_GetAnimationName(int index, ulong entity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int AnimationComponent_GetAnimationNameSize(ulong entity);


        /*---------------------------INVENTORY MANAGER---------------------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetEntityTextureCook(string textureName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetEntityTextureEnemy(ulong entityID, string textureName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetEntityChestTexture(string textureName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetEntityTextureByID(ulong entityID, string textureName);


        /*---------------------------PLAYER ENTITY---------------------------*/
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong GetPlayerEntity();

        /*---------------------------LEVEL SWITCH---------------------------*/
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LevelSwitch_TutorialToLevel1();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool BoolSwitch_TutorialToLevel1();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LevelSwitch_MainMenuToLevel1();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool BoolSwitch_MainMenuToLevel1();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LevelSwitch_ReloadCurrLevel();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool BoolSwitch_ReloadCurrLevel();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LevelSwitch_PauseMenuToMainMenu();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LevelSwitch_ReturnMainMenu();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LevelSwitch_Level1ToLevel2();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool BoolSwitch_Level1ToLevel2();

        [MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void LevelSwitch_Level2ToLevel1();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool BoolSwitch_Level2ToLevel1();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int LevelSwitch_GetCurrLevel();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int LevelSwitch_MainMenuToTutorial();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool LevelSwitch_GetLayerStatus(string monoLayerName);

        /*---------------------------ENGINE FUNCTIONS---------------------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Engine_QuitGame();

        /*---------------------------PAUSE MENU FUNCTIONS---------------------------*/
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void PauseMenu_Toggle();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Layer_ChangeOpacity(float opacity);

        /*---------------------------AUDIO FUNCTIONS---------------------------*/
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Audio_PlaySound(string monoSoundName, float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Audio_StopSound(string monoSoundName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Audio_GetBGMVolume(out float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Audio_SetBGMVolume(float volume);

        /*---------------------------FONT FUNCTIONS---------------------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Font_SetFont(ulong entityID, string fontName, float scale);

        /*---------------------------CHEST FUNCTIONS---------------------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetChestOverlayTrigger(string chest);
    }
}
