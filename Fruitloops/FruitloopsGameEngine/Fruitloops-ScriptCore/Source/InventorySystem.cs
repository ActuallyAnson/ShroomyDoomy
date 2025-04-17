/******************************************************************************
/*!
\file  InventorySystem.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng
\date    March 14, 2025
\brief  Implements the InventorySystem class, a singleton behavior component for 
        managing items, inventory slots, crafting UI, and inventory display in the game.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace FruitLoops
{
    /**
     * @brief Manages the game's inventory system as a singleton behavior component.
     * 
     * @note Handles item addition/removal, slot cycling, crafting integration,
     * UI texture updates, and inventory display. Inherits from BehaviourComponent.
     */
    public class InventorySystem : BehaviourComponent
    {
        private static InventorySystem _instance; // Singleton instance

        private Dictionary<string, int> items = new Dictionary<string, int>(); // Dictionary-based inventory
        private List<string> itemOrder = new List<string>(); // List for cycling order

        private int currentIndex1 = -1; // Slot 1 index
        private int currentIndex2 = -1; // Slot 2 index

        /**
         * @brief Default constructor for InventorySystem.
         */
        public InventorySystem() { }

        /**
         * @brief Constructs an InventorySystem instance for a specific entity.
         * 
         * @param entity The Entity to associate with this inventory system.
         * @note Registers the instance as a behavior component with the GameLogicSystem.
         */
        public InventorySystem(Entity entity)
        {
            Debug.LogInfo("[InventorySystem] Creating InventorySystem for entity {0}", entity.ID);

            // Register InventorySystem as a behavior
            var behaviour = new BehaviourFCT(entity, Init, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            Debug.LogInfo("[InventorySystem] InventorySystem registered as a behavior.");
        }

        /**
         * @brief Creates or retrieves the singleton InventorySystem instance.
         * 
         * @param entity The Entity to associate with the inventory system.
         * @return The singleton InventorySystem instance.
         * @note Creates a new instance if none exists, otherwise returns the existing one.
         */
        public static InventorySystem CreateBehaviour(Entity entity)
        {
            Debug.LogInfo("[InventorySystem] CreateBehaviour called for entity " + entity.ID);

            if (_instance == null)
            {
                _instance = new InventorySystem(entity);
                Debug.LogInfo("[InventorySystem] Singleton instance created and registered.");
            }
            return _instance;
        }

        /**
         * @brief Gets the singleton instance of InventorySystem.
         * 
         * @return The singleton InventorySystem instance.
         * @note Logs an error if accessed before creation.
         */
        public static InventorySystem Instance
        {
            get
            {
                if (_instance == null)
                {
                    Debug.LogError("[InventorySystem] Instance requested before creation! This can break update loops.");
                }
                return _instance;
            }
        }

        /**
         * @brief Initializes the InventorySystem for an entity.
         * 
         * @param entity The Entity to associate with the inventory system.
         * @note Creates and registers the singleton instance if not already initialized.
         */
        public static void Init(Entity entity)      // this part seems wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        {
            if (_instance == null)
            {
                _instance = new InventorySystem(entity);
                GameLogicSystem.Instance.RegisterLogicComponent(_instance);
                Debug.LogInfo("Inventory System Initialized and Registered for Updates");
            }
        }

        public static void ResetInstance()
        {
            _instance = null;
        }

        private bool _sizzlingSoundPlaying = false;  // Tracks if the sound is already playing
        private string _sizzlingSoundName = "SFX_Cooking";

        /**
         * @brief Updates the inventory system state.
         * 
         * @param entity The Entity associated with the inventory system.
         * @param deltaTime The time elapsed since the last update in seconds.
         * @note Manages crafting UI updates and sizzling sound playback based on crafting status.
         */
        public static void Update(Entity entity, float deltaTime)           // add a debug and not running when level switches
        {
            if (_instance == null)
            {
                Debug.LogWarning("[InventorySystem] Update called but InventorySystem instance is null.");
                return;
            }

            //Debug.LogInfo($"[InventorySystem] Update running for entity {entity.ID} at deltaTime {deltaTime}");

            if (CraftingSystem.Instance.CanCraft())
            {
                Debug.LogInfo("Inventory System is being updated here");
                CraftingSystem.Instance.UpdateCookButton();
                InventorySystem.Instance.UpdateCraftingOutput();
                if (!InventorySystem.Instance._sizzlingSoundPlaying)
                {
                    InternalCalls.Audio_PlaySound(InventorySystem.Instance._sizzlingSoundName, 0.5f); // The 'true' parameter enables looping
                    InventorySystem.Instance._sizzlingSoundPlaying = true;
                }
            }
            else
            {
                CraftingSystem.Instance.HideCookButton();
            }
            if (_instance == null) return;
        }

        /**
         * @brief Cleans up the InventorySystem.
         * 
         * @param entity The Entity associated with the inventory system.
         * @note Logs detachment; no additional cleanup implemented.
         */
        public static void End(Entity entity)
        {
            Debug.LogInfo("Inventory System Detached");
        }

        /**
         * @brief Adds an item to the inventory.
         * 
         * @param itemName The name of the item to add.
         * @note Increments count if item exists, otherwise adds it with count 1 and updates order.
         */
        public void AddItem(string itemName)
        {
            if (string.IsNullOrEmpty(itemName))
            {
                Debug.LogWarning("[InventorySystem] Attempted to add an invalid item (null or empty).");
                return;
            }

            if (items.ContainsKey(itemName))
            {
                items[itemName]++; // Increase count
            }
            else
            {
                items[itemName] = 1; // Add new item
                itemOrder.Add(itemName); // Maintain order
            }

            Debug.LogInfo($"[InventorySystem] Added {itemName}. Count: {items[itemName]}");
            DisplayInventory();
        }

        /**
         * @brief Removes an item from the inventory.
         * 
         * @param itemName The name of the item to remove.
         * @note Decrements count; removes item completely if count reaches zero.
         */
        public void RemoveItem(string itemName)
        {
            if (!items.ContainsKey(itemName))
            {
                Debug.LogWarning($"[InventorySystem] Tried to remove {itemName}, but it was not found in inventory.");
                return;
            }

            Debug.LogInfo($"[InventorySystem] Removing item: {itemName}. Current count: {items[itemName]}");

            items[itemName]--;

            if (items[itemName] <= 0)
            {
                items.Remove(itemName); // Remove from dictionary
                itemOrder.Remove(itemName); // Remove from list

                Debug.LogInfo($"[InventorySystem] {itemName} removed from inventory completely.");
            }

            DisplayInventory();
        }

        /*------------------ List Manipulation Functions ------------------*/

        /**
         * @brief Gets the number of unique items in the inventory.
         * 
         * @return The count of items in the itemOrder list.
         */
        public int GetListCount()
        {
            return itemOrder.Count;
        }

        /**
         * @brief Gets the last item added to the inventory.
         * 
         * @return The name of the last item in the itemOrder list.
         */
        public string GetLastItem()
        {
            return itemOrder[itemOrder.Count - 1];
        }

        /**
         * @brief Cycles to the next item in the specified slot.
         * 
         * @param slot The slot number (1 or 2) to cycle.
         * @note Handles single-item and multi-item cases, avoiding duplicates between slots.
         */
        public void NextItem(int slot)
        {
            if (items.Count == 0) return; // No items to cycle

            if (items.Count == 1) // Only 1 item in inventory
            {
                if (slot == 1 && currentIndex2 != 0) // Ensure only one slot can display it
                {
                    currentIndex1 = 0;
                    UpdateItemTexture(1);
                }
                else if (slot == 2 && currentIndex1 != 0)
                {
                    currentIndex2 = 0;
                    UpdateItemTexture(2);
                }
            }
            else // More than 1 item in inventory
            {
                if (slot == 1)
                {
                    int newIndex = currentIndex1;
                    do
                    {
                        newIndex = (newIndex + 1) % items.Count; // Cycle forward
                    } while (newIndex == currentIndex2 && items.Count > 1); // Prevent duplicate display

                    currentIndex1 = newIndex;
                }
                else if (slot == 2)
                {
                    int newIndex = currentIndex2;
                    do
                    {
                        newIndex = (newIndex + 1) % items.Count; // Cycle forward
                    } while (newIndex == currentIndex1 && items.Count > 1); // Prevent duplicate display

                    currentIndex2 = newIndex;
                }
            }
            UpdateItemTexture(slot);
        }

        /**
         * @brief Cycles to the previous item in the specified slot.
         * 
         * @param slot The slot number (1 or 2) to cycle.
         * @note Handles single-item and multi-item cases, avoiding duplicates between slots.
         */
        public void PreviousItem(int slot)
        {
            if (items.Count == 0) return; // No items to cycle

            if (items.Count == 1) // Only 1 item in inventory
            {
                if (slot == 1 && currentIndex2 != 0)
                {
                    currentIndex1 = 0;
                    UpdateItemTexture(1);
                }
                else if (slot == 2 && currentIndex1 != 0)
                {
                    currentIndex2 = 0;
                    UpdateItemTexture(2);
                }
            }
            else // More than 1 item in inventory
            {
                if (slot == 1)
                {
                    int newIndex = currentIndex1;
                    do
                    {
                        newIndex = (newIndex - 1 + items.Count) % items.Count; // Cycle backward
                    } while (newIndex == currentIndex2 && items.Count > 1); // Prevent duplicate display

                    currentIndex1 = newIndex;
                }
                else if (slot == 2)
                {
                    int newIndex = currentIndex2;
                    do
                    {
                        newIndex = (newIndex - 1 + items.Count) % items.Count; // Cycle backward
                    } while (newIndex == currentIndex1 && items.Count > 1); // Prevent duplicate display

                    currentIndex2 = newIndex;
                }
            }

            UpdateItemTexture(slot);
        }

        /*------------------ Dictionary Manipulation Functions ------------------*/

        /**
         * @brief Gets the count of a specific item in the inventory.
         * 
         * @param itemName The name of the item to check.
         * @return The count of the item, or 0 if not found or dictionary is null.
         */
        public int GetItemCount(string itemName)
        {
            if (items == null || !items.ContainsKey(itemName))
            {
                return 0; // Item not found or dictionary is null
            }

            return items[itemName]; // Return the specific count of the item
        }

        /**
         * @brief Resets the inventory slot pointers to their initial state.
         */
        public void InventoryPointersReset()
        {
            currentIndex1 = -1;
            currentIndex2 = -1;
        }

        /*------------------ Item Slot Texture Manipulation Functions ------------------*/

        /**
         * @brief Updates the texture of an inventory slot based on the current item.
         * 
         * @param slot The slot number (1 or 2) to update.
         * @note Sets default texture if no item is selected, otherwise uses the current item's texture.
         */
        public void UpdateItemTexture(int slot)
        {
            string newTexture;

            if (slot == 1)
            {
                if (currentIndex1 == -1)
                {
                    newTexture = "inventory_slot_1"; // Reset Slot 1 if empty
                }
                else
                {
                    newTexture = itemOrder[currentIndex1]; // Assign correct item
                }
                ulong InventorySlotOneID = InternalCalls.Entity_GetEntityByName("InventorySlot1");
                InternalCalls.SetEntityTextureByID(InventorySlotOneID, newTexture); // Reset Slot 1 to default texture
            }
            else if (slot == 2)
            {
                if (currentIndex2 == -2) // Note: Likely a typo, should be -1?
                {
                    newTexture = "inventory_slot_2"; // Reset Slot 2 if empty
                }
                else
                {
                    newTexture = itemOrder[currentIndex2]; // Assign correct item
                }
                ulong InventorySlotTwoID = InternalCalls.Entity_GetEntityByName("InventorySlot2");
                InternalCalls.SetEntityTextureByID(InventorySlotTwoID, newTexture); // Reset Slot 1 to default texture
            }
        }

        /**
         * @brief Sets the texture for the crafting output display.
         * 
         * @param craftingoutputtexture The texture name to set for the output slot.
         */
        public void SetOutputTexture(string craftingoutputtexture)
        {
            ulong OutputTextureID = InternalCalls.Entity_GetEntityByName("InventoryOutput");
            InternalCalls.SetEntityTextureByID(OutputTextureID, craftingoutputtexture);
        }

        /**
         * @brief Resets the texture of an inventory slot to its default state.
         * 
         * @param slot The slot number (1 or 2) to reset.
         */
        public void ResetItemTexture(int slot)
        {
            Debug.LogInfo($"[InventoryManager] Resetting Slot {slot} texture to default");

            if (slot == 1)
            {
                ulong InventorySlotOneID = InternalCalls.Entity_GetEntityByName("InventorySlot1");
                InternalCalls.SetEntityTextureByID(InventorySlotOneID, "inventory_slot_1"); // Reset Slot 1 to default texture
            }
            else if (slot == 2)
            {
                ulong InventorySlotTwoID = InternalCalls.Entity_GetEntityByName("InventorySlot2");
                InternalCalls.SetEntityTextureByID(InventorySlotTwoID, "inventory_slot_2"); // Reset Slot 1 to default texture
            }
        }

        /**
         * @brief Resets the crafting output texture to its default state.
         */
        public void ResetCraftedTexture()
        {
            ulong InventoryOutputID = InternalCalls.Entity_GetEntityByName("InventoryOutput");
            InternalCalls.SetEntityTextureByID(InventoryOutputID, "inventory_output"); // Reset Slot 1 to default texture
        }

        public void ResetSlot()
        {
            ulong Inventoryslot1 = InternalCalls.Entity_GetEntityByName("InventorySlot1");
            InternalCalls.SetEntityTextureByID(Inventoryslot1, "inventory_slot_1"); // Reset Slot 1 to default texture
            ulong Inventoryslot2 = InternalCalls.Entity_GetEntityByName("InventorySlot2");
            InternalCalls.SetEntityTextureByID(Inventoryslot1, "inventory_slot_2"); // Reset Slot 1 to default texture
        }

        /*------------------ Debugging Functions ------------------*/

        /**
         * @brief Displays the current inventory contents for debugging.
         * 
         * @note Logs each item's name and count, or indicates if the inventory is empty.
         */
        public void DisplayInventory()
        {
            Debug.LogInfo("[InventorySystem] Current Inventory:");

            if (items == null || items.Count == 0)
            {
                Debug.LogInfo("  (Inventory is empty)");
                return;
            }

            foreach (var item in items)
            {
                Debug.LogInfo($"  {item.Key}: {item.Value}"); // Display item name and count
            }
        }

        /*------------------ Crafting Functions ------------------*/

        /**
         * @brief Updates the crafting output texture based on current slot items.
         * 
         * @note Checks the items in slots 1 and 2, sets the crafted item texture if valid,
         * otherwise resets to default.
         */
        public void UpdateCraftingOutput()
        {
            string item1 = GetCurrentItem(1); // Slot 1 Item
            string item2 = GetCurrentItem(2); // Slot 2 Item

            if (!string.IsNullOrEmpty(item1) && !string.IsNullOrEmpty(item2))
            {
                string craftedItem = CraftingSystem.Instance.GetCraftedItem(item1, item2);
                if (!string.IsNullOrEmpty(craftedItem))
                {
                    ulong InventoryOutputID = InternalCalls.Entity_GetEntityByName("InventoryOutput");
                    InternalCalls.SetEntityTextureByID(InventoryOutputID, craftedItem);
                }
                else
                {
                    ResetCraftedTexture();  // Reset if no valid recipe
                }
            }
            else
            {
                ResetCraftedTexture(); // Reset if any slot is empty
            }
        }

        /**
         * @brief Gets the current item displayed in a specified slot.
         * 
         * @param slot The slot number (1 or 2) to check.
         * @return The name of the current item in the slot, or null if invalid.
         */
        public string GetCurrentItem(int slot)
        {
            if (slot == 1 && currentIndex1 >= 0 && currentIndex1 < items.Count)
            {
                return itemOrder[currentIndex1]; // Return currently displayed item in Slot 1
            }
            if (slot == 2 && currentIndex2 >= 0 && currentIndex2 < items.Count)
            {
                return itemOrder[currentIndex2]; // Return currently displayed item in Slot 2
            }

            return null; // Return null if no valid item is displayed
        }

        /*------------------ Inventory UI Functions ------------------*/

        /**
         * @brief Displays the inventory UI with item counts.
         * 
         * @note Shows an empty bag message if inventory is empty; otherwise displays icons
         * and counts for specific items (fish, cheese, bread, tomato, chicken).
         */
        public void DisplayInventoryUI()
        {
            Debug.LogInfo("[InventorySystem] Updating inventory UI.");

            // Check if inventory is empty
            if (items.Count == 0)
            {
                Debug.LogInfo("[InventorySystem] Inventory is empty, displaying empty bag.");

                // Find and show the empty bag icon
                ulong emptyBagIcon = InternalCalls.Entity_GetEntityByName("innerBag");
                if (emptyBagIcon != 0)
                {
                    Vector2D bagVisibleScale = new Vector2D(500.0f, 500.0f); // Set visible size
                    InternalCalls.Transform_SetScale(emptyBagIcon, ref bagVisibleScale);
                }

                ulong txtBox1 = InternalCalls.Entity_GetEntityByName("bagEmpty1");
                InternalCalls.Font_SetFont(txtBox1, "Your inventory is empty.", 0.65f);

                ulong txtBox2 = InternalCalls.Entity_GetEntityByName("bagEmpty2");
                InternalCalls.Font_SetFont(txtBox2, "Find some ingredients to", 0.65f);

                ulong txtBox3 = InternalCalls.Entity_GetEntityByName("bagEmpty3");
                InternalCalls.Font_SetFont(txtBox3, "put in it!", 0.65f);

                return;
            }
            else
            {
                ulong emptyBagIcon = InternalCalls.Entity_GetEntityByName("innerBag");
                if (emptyBagIcon != 0)
                {
                    Vector2D bagVisibleScale = new Vector2D(500.0f, 500.0f); // Set visible size
                    InternalCalls.Transform_SetScale(emptyBagIcon, ref bagVisibleScale);
                }

                // Displaying of Fish Icon in bag
                if (items.ContainsKey("fish"))
                {
                    ulong fishBagIcon = InternalCalls.Entity_GetEntityByName("fishBag");
                    if (fishBagIcon != 0)
                    {
                        Vector2D fishBagVisibleScale = new Vector2D(100.0f, 100.0f); // Set visible size
                        InternalCalls.Transform_SetScale(fishBagIcon, ref fishBagVisibleScale);

                        // set text
                        ulong txtBox = InternalCalls.Entity_GetEntityByName("bagSlotFish");
                        int fishCount = GetItemCount("fish");
                        string fishstring = "x" + fishCount.ToString();
                        InternalCalls.Font_SetFont(txtBox, fishstring, 1.0f);
                    }
                }

                // Displaying of Cheese Icon in bag
                if (items.ContainsKey("cheese"))
                {
                    ulong cheeseBagIcon = InternalCalls.Entity_GetEntityByName("cheeseBag");
                    if (cheeseBagIcon != 0)
                    {
                        Vector2D cheeseBagVisibleScale = new Vector2D(100.0f, 100.0f); // Set visible size
                        InternalCalls.Transform_SetScale(cheeseBagIcon, ref cheeseBagVisibleScale);

                        // set text
                        ulong txtBox = InternalCalls.Entity_GetEntityByName("bagSlotCheese");
                        int cheeseCount = GetItemCount("cheese");
                        string cheesestring = "x" + cheeseCount.ToString();
                        InternalCalls.Font_SetFont(txtBox, cheesestring, 1.0f);
                    }
                }

                // Displaying of Bread Icon in bag
                if (items.ContainsKey("bread"))
                {
                    ulong breadBagIcon = InternalCalls.Entity_GetEntityByName("breadBag");
                    if (breadBagIcon != 0)
                    {
                        Vector2D breadBagVisibleScale = new Vector2D(100.0f, 100.0f); // Set visible size
                        InternalCalls.Transform_SetScale(breadBagIcon, ref breadBagVisibleScale);

                        // set text
                        ulong txtBox = InternalCalls.Entity_GetEntityByName("bagSlotBread");
                        int breadCount = GetItemCount("bread");
                        string breadstring = "x" + breadCount.ToString();
                        InternalCalls.Font_SetFont(txtBox, breadstring, 1.0f);
                    }
                }

                // Displaying of Tomato Icon in bag
                if (items.ContainsKey("tomato"))
                {
                    ulong tomatoBagIcon = InternalCalls.Entity_GetEntityByName("tomatoBag");
                    if (tomatoBagIcon != 0)
                    {
                        Vector2D tomatoBagVisibleScale = new Vector2D(100.0f, 100.0f); // Set visible size
                        InternalCalls.Transform_SetScale(tomatoBagIcon, ref tomatoBagVisibleScale);

                        // set text
                        ulong txtBox = InternalCalls.Entity_GetEntityByName("bagSlotTomato");
                        int toamtoCount = GetItemCount("tomato"); // Fixed typo: "toamto" to "tomato"
                        string tomatostring = "x" + toamtoCount.ToString();
                        InternalCalls.Font_SetFont(txtBox, tomatostring, 1.0f);
                    }
                }

                // Displaying of Chicken Icon in bag
                if (items.ContainsKey("chicken"))
                {
                    ulong chickenBagIcon = InternalCalls.Entity_GetEntityByName("chickenBag");
                    if (chickenBagIcon != 0)
                    {
                        Vector2D chickenBagVisibleScale = new Vector2D(100.0f, 100.0f); // Set visible size
                        InternalCalls.Transform_SetScale(chickenBagIcon, ref chickenBagVisibleScale);

                        // set text
                        ulong txtBox = InternalCalls.Entity_GetEntityByName("bagSlotChicken");
                        int chickenCount = GetItemCount("chicken");
                        string chickenstring = "x" + chickenCount.ToString();
                        InternalCalls.Font_SetFont(txtBox, chickenstring, 1.0f);
                    }
                }
            }
        }

        /**
         * @brief Hides the inventory UI by resetting scales and text.
         * 
         * @note Sets all inventory UI elements (bag, icons, and text) to zero scale or empty text.
         */
        public void HideInventoryUI()
        {
            // Find and show the empty bag icon
            ulong emptyBagIcon = InternalCalls.Entity_GetEntityByName("innerBag");
            if (emptyBagIcon != 0)
            {
                Vector2D bagVisibleScale = new Vector2D(0.0f, 0.0f); // Set visible size
                InternalCalls.Transform_SetScale(emptyBagIcon, ref bagVisibleScale);
            }

            ulong txtBox1 = InternalCalls.Entity_GetEntityByName("bagEmpty1");
            InternalCalls.Font_SetFont(txtBox1, "", 0.0f);

            ulong txtBox2 = InternalCalls.Entity_GetEntityByName("bagEmpty2");
            InternalCalls.Font_SetFont(txtBox2, "", 0.0f);

            ulong txtBox3 = InternalCalls.Entity_GetEntityByName("bagEmpty3");
            InternalCalls.Font_SetFont(txtBox3, "", 0.0f);

            ulong fishBagIcon = InternalCalls.Entity_GetEntityByName("fishBag");
            if (fishBagIcon != 0)
            {
                Vector2D fishBagVisibleScale = new Vector2D(0.0f, 0.0f); // Set visible size
                InternalCalls.Transform_SetScale(fishBagIcon, ref fishBagVisibleScale);
            }
            ulong fishBox = InternalCalls.Entity_GetEntityByName("bagSlotFish");
            InternalCalls.Font_SetFont(fishBox, "", 0.0f);

            ulong cheeseBagIcon = InternalCalls.Entity_GetEntityByName("cheeseBag");
            if (cheeseBagIcon != 0)
            {
                Vector2D cheeseBagVisibleScale = new Vector2D(0.0f, 0.0f); // Set visible size
                InternalCalls.Transform_SetScale(cheeseBagIcon, ref cheeseBagVisibleScale);
            }
            ulong cheeseBox = InternalCalls.Entity_GetEntityByName("bagSlotCheese");
            InternalCalls.Font_SetFont(cheeseBox, "", 0.0f);

            ulong breadBagIcon = InternalCalls.Entity_GetEntityByName("breadBag");
            if (breadBagIcon != 0)
            {
                Vector2D breadBagVisibleScale = new Vector2D(0.0f, 0.0f); // Set visible size
                InternalCalls.Transform_SetScale(breadBagIcon, ref breadBagVisibleScale);
            }
            ulong breadBox = InternalCalls.Entity_GetEntityByName("bagSlotBread");
            InternalCalls.Font_SetFont(breadBox, "", 0.0f);

            ulong tomatoBagIcon = InternalCalls.Entity_GetEntityByName("tomatoBag");
            if (tomatoBagIcon != 0)
            {
                Vector2D tomatoBagVisibleScale = new Vector2D(0.0f, 0.0f); // Set visible size
                InternalCalls.Transform_SetScale(tomatoBagIcon, ref tomatoBagVisibleScale);
            }
            ulong tomatoBox = InternalCalls.Entity_GetEntityByName("bagSlotTomato");
            InternalCalls.Font_SetFont(tomatoBox, "", 0.0f);

            ulong chickenBagIcon = InternalCalls.Entity_GetEntityByName("chickenBag");
            if (chickenBagIcon != 0)
            {
                Vector2D chickenBagVisibleScale = new Vector2D(0.0f, 0.0f); // Set visible size
                InternalCalls.Transform_SetScale(chickenBagIcon, ref chickenBagVisibleScale);
            }
            ulong chickenBox = InternalCalls.Entity_GetEntityByName("bagSlotChicken");
            InternalCalls.Font_SetFont(chickenBox, "", 0.0f);

            return;
        }

        /**
         * @brief Completely resets the inventory system and UI.
         * 
         * @note Call this method when the level is restarted to clear all inventory data and visuals.
         */
        public void ResetInventorySystem()
        {
            Debug.LogInfo("[InventorySystem] ResetInventorySystem called.");

            // 1. Clear internal data
            items.Clear();
            itemOrder.Clear();

            // 2. Reset inventory slot indices
            currentIndex1 = -1;
            currentIndex2 = -1;

            // 3. Reset inventory slot visuals
            ResetItemTexture(1);
            ResetItemTexture(2);

            // 4. Reset crafting output slot
            ResetCraftedTexture();

            // 5. Hide inventory UI if it's showing
            HideInventoryUI();

            // 6. Stop any crafting sounds
            _sizzlingSoundPlaying = false;

            Debug.LogInfo("[InventorySystem] Inventory fully cleared and reset.");
        }

        //public void ResetPointers()
        //{
        //    currentIndex1 = -1;
        //    currentIndex2 = -1;
        //}

    }
}