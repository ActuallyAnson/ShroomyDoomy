/******************************************************************************
/*!
\file  CraftingSystem.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng
\date    02 Feb, 2025
\brief  This file contains the implementation of the CraftingSystem class, which
        handles the creation of crafted items from ingredient combinations and
        applies their effects to the player.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FruitLoops;
using static FruitLoops.CraftingSystem;


namespace FruitLoops
{
    /**
     * @brief The CraftingSystem class implements a singleton-based crafting system.
     *
     * It stores a collection of crafting recipes and provides functionality to combine
     * ingredients to create a crafted item. The system also applies the effect of the
     * crafted item to the player entity.
     */
    public class CraftingSystem
    {
        /**
         * @brief Enumeration of crafted item identifiers.
         */
        public enum CraftedItems
        {
            fish_cheese_stick,
            baked_chicken_cheese,
            cheesy_sandwich,
            cheesy_roasted_tomatos,
            chicken_fish_taco,
            chicken_sandwich,
            fish_toast,
            fired_fish_with_ketchup,
            nuggets_with_ketchup,
            tomato_soup_with_bread
        }

        private static CraftingSystem _instance;
        /**
         * @brief A dictionary mapping ingredient combination keys to crafted item names.
         */
        private Dictionary<string, string> craftingRecipes;

        /**
         * @brief Gets the singleton instance of the CraftingSystem.
         *
         * If the instance does not exist, it will be created.
         *
         * @return The singleton instance of CraftingSystem.
         */
        public static CraftingSystem Instance
        {
            get
            {
                if (_instance == null)
                {
                    _instance = new CraftingSystem();
                    Console.WriteLine("[DEBUG] CraftingSystem Instance Created!");
                }
                return _instance;
            }
        }

        /**
         * @brief Private constructor to initialize the CraftingSystem.
         *
         * Initializes the craftingRecipes dictionary with predefined recipes.
         */
        private CraftingSystem()
        {
            Console.WriteLine("[DEBUG] Initializing Crafting System...");
            craftingRecipes = new Dictionary<string, string>
            {
                { "cheese_fish", "fish_cheese_stick" },
                { "fish_cheese", "fish_cheese_stick" },

                { "chicken_cheese", "baked_chicken_cheese" },
                { "cheese_chicken", "baked_chicken_cheese" },

                { "bread_cheese", "cheesy_sandwich" },
                { "cheese_bread", "cheesy_sandwich" },

                { "cheese_tomato", "cheesy_roasted_tomatos" },
                { "toamto_cheese", "cheesy_roasted_tomatos" },

                { "chicken_fish", "chicken_fish_taco" },
                { "fish_chicken", "chicken_fish_taco" },

                { "bread_chicken", "chicken_sandwich" },
                { "chicken_bread", "chicken_sandwich" },

                { "fish_bread", "fish_toast" },
                { "bread_fish", "fish_toast" },

                { "fish_tomato", "fried_fish_with_ketchup" },
                { "tomato_fish", "fried_fish_with_ketchup" },

                { "chicken_tomato", "nuggets_with_ketchup" },
                { "tomato_chicken", "nuggets_with_ketchup" },

                { "tomato_bread", "tomato_soup_with_bread" },
                { "bread_tomato", "tomato_soup_with_bread" }
            };
        }

        /**
         * @brief Attempts to craft an item from two provided ingredient names.
         *
         * The method concatenates the two ingredient names in both orders and checks if a matching
         * recipe exists in the craftingRecipes dictionary.
         *
         * @param item1 The first ingredient.
         * @param item2 The second ingredient.
         * @return The name of the crafted item if a valid recipe is found; otherwise, null.
         */
        public string GetCraftedItem(string item1, string item2)
        {
            Console.WriteLine($"[DEBUG] Crafting Request: {item1} + {item2}");

            if (string.IsNullOrEmpty(item1) || string.IsNullOrEmpty(item2))
                return null;

            string recipeKey1 = item1 + "_" + item2;
            string recipeKey2 = item2 + "_" + item1;

            if (craftingRecipes.ContainsKey(recipeKey1))
            {
                Console.WriteLine($"[DEBUG] Crafted: {craftingRecipes[recipeKey1]}");
                return craftingRecipes[recipeKey1];
            }

            if (craftingRecipes.ContainsKey(recipeKey2))
            {
                Console.WriteLine($"[DEBUG] Crafted: {craftingRecipes[recipeKey2]}");
                return craftingRecipes[recipeKey2];
            }

            Console.WriteLine("[DEBUG] No valid crafting recipe found.");
            return null;
        }

        /**
         * @brief Consumes a crafted item and applies its effect to the player.
         *
         * The method checks if the player entity exists and whether crafting requirements are met.
         * It then retrieves the current ingredients from the inventory, attempts to craft an item,
         * and applies the corresponding effect (such as increasing health or energy) to the player.
         * Finally, the used crafting ingredients are removed from the inventory.
         *
         * @param player The player entity consuming the crafted item.
         */
        public void ConsumeCraftedItem(Entity player)
        {
            if (player == null)
            {
                Debug.LogError("[CraftingSystem] Player entity not found. Cannot consume item.");
                return;
            }

            // Check if crafting is possible BEFORE modifying stats
            if (!CraftingSystem.Instance.CanCraft())
            {
                Debug.LogError("[CraftingSystem] Cannot consume item. Crafting requirements not met.");
                return;
            }

            // Get crafted item based on slot contents
            string item1 = InventorySystem.Instance.GetCurrentItem(1);
            string item2 = InventorySystem.Instance.GetCurrentItem(2);
            string craftedItem = CraftingSystem.Instance.GetCraftedItem(item1, item2);

            if (string.IsNullOrEmpty(craftedItem))
            {
                Debug.LogError("[CraftingSystem] No crafted item found to consume.");
                return;
            }

            Debug.LogInfo($"[CraftingSystem] Player is consuming {craftedItem}");
            int playerMaxHealth = InternalCalls.UnitComponent_GetMaxHealth(player.ID);
            int playerHealth = InternalCalls.UnitComponent_GetHealth(player.ID);
            int playerAttack = InternalCalls.UnitComponent_GetAttackPower(player.ID);
            int playerEnergy = InternalCalls.UnitComponent_GetEnergy(player.ID);
            int playerMaxEnergy = InternalCalls.UnitComponent_GetMaxEnergy(player.ID);

            // Apply the crafted item effect
            switch (craftedItem)
            {
                case "fish_cheese_stick":
                    //Player.rangedunlocked = true;
                    playerAttack += 150;
                    InternalCalls.UnitComponent_SetAttackPower(player.ID, playerAttack);
                    break;

                case "cheesy_roasted_tomatos":
                    Player.rangedunlocked = true;
                    break;

                case "baked_chicken_cheese":
                    playerAttack += 10;
                    InternalCalls.UnitComponent_SetAttackPower(player.ID, playerAttack);
                    break;

                case "cheesy_sandwich":
                    if (playerHealth == playerMaxHealth)
                    {
                        playerHealth += 500;
                        InternalCalls.UnitComponent_SetHealth(player.ID, playerHealth);
                        InternalCalls.UnitComponent_SetMaxHealth(player.ID, playerHealth);
                        playerAttack -= 20;
                        InternalCalls.UnitComponent_SetAttackPower(player.ID, playerAttack);
                    }
                    else
                    {
                        playerHealth += 500;
                        InternalCalls.UnitComponent_SetHealth(player.ID, playerHealth);
                        InternalCalls.UnitComponent_SetMaxHealth(player.ID, playerHealth + playerMaxHealth);
                        playerAttack -= 20;
                        InternalCalls.UnitComponent_SetAttackPower(player.ID, playerAttack);
                    }
                    break;

                case "chicken_sandwich":
                    if (playerEnergy != playerMaxEnergy)
                    {
                        playerEnergy = playerMaxEnergy;
                        InternalCalls.UnitComponent_SetEnergy(player.ID, playerMaxEnergy);
                    }
                    else
                    {
                        InternalCalls.UnitComponent_SetEnergy(player.ID, playerEnergy);
                        InternalCalls.UnitComponent_SetMaxEnergy(player.ID, playerEnergy);
                    }
                    break;

                case "nuggets_with_ketchup":
                    if (playerHealth != playerMaxHealth)
                    {
                        playerHealth = playerMaxHealth;
                        InternalCalls.UnitComponent_SetHealth(player.ID, playerMaxHealth);
                    }
                    else
                    {
                        InternalCalls.UnitComponent_SetHealth(player.ID, playerHealth);
                        InternalCalls.UnitComponent_SetMaxHealth(player.ID, playerHealth);
                    }
                    break;

                case "fried_fish_with_ketchup":
                    if (playerHealth == playerMaxHealth)
                    {
                        playerHealth += 250;
                        InternalCalls.UnitComponent_SetHealth(player.ID, playerHealth);
                        InternalCalls.UnitComponent_SetMaxHealth(player.ID, playerHealth);
                    }
                    else
                    {
                        playerHealth += 250;
                        InternalCalls.UnitComponent_SetHealth(player.ID, playerHealth);
                        InternalCalls.UnitComponent_SetMaxHealth(player.ID, playerHealth + playerMaxHealth);
                    }
                    break;

                case "fish_toast":
                    if (playerEnergy == playerMaxEnergy)
                    {
                        playerEnergy += 2;
                        InternalCalls.UnitComponent_SetEnergy(player.ID, playerEnergy);
                        InternalCalls.UnitComponent_SetMaxEnergy(player.ID, 2 + playerMaxEnergy);
                    }
                    else
                    {
                        playerEnergy += 2;
                        InternalCalls.UnitComponent_SetEnergy(player.ID, playerEnergy);
                        InternalCalls.UnitComponent_SetMaxEnergy(player.ID, 2 + playerMaxEnergy);
                    }
                    break;

                case "tomato_soup_with_bread":
                    if (playerHealth == playerMaxHealth)
                    {
                        playerHealth += 100;
                        InternalCalls.UnitComponent_SetHealth(player.ID, playerHealth);
                        InternalCalls.UnitComponent_SetMaxHealth(player.ID, playerHealth);
                        playerAttack += 20;
                        InternalCalls.UnitComponent_SetAttackPower(player.ID, playerAttack);
                    }
                    else
                    {
                        playerHealth += 100;
                        InternalCalls.UnitComponent_SetHealth(player.ID, playerHealth);
                        InternalCalls.UnitComponent_SetMaxHealth(player.ID, playerHealth + playerMaxHealth);
                        playerAttack += 20;
                        InternalCalls.UnitComponent_SetAttackPower(player.ID, playerAttack);
                    }
                    break;

                default:
                    Debug.LogWarning($"[CraftingSystem] No effect found for crafted item {craftedItem}");
                    return; // If no valid effect, stop execution before removing items
            }

            Debug.LogInfo($"[CraftingSystem] Player stats updated after consuming {craftedItem}");

            // Remove the crafting ingredients AFTER applying the effect
            CraftingSystem.Instance.RemoveCraftingIngredients();

            // [TUTORIAL LOGIC] Notify the tutorial that a dish was cooked (used for Scene 13)
            TutorialManager.OnPlayerCookedDish();
        }

        public bool CanCraft()
        {
            string item1 = InventorySystem.Instance.GetCurrentItem(1); // Slot 1 Item
            string item2 = InventorySystem.Instance.GetCurrentItem(2); // Slot 2 Item

            if (string.IsNullOrEmpty(item1) || string.IsNullOrEmpty(item2))
            {
                //HideCookButton();
                // Debug.LogWarning("[InventoryManager] Cannot craft, one or both slots are empty.");
                return false;
            }

            string craftedItem = CraftingSystem.Instance.GetCraftedItem(item1, item2);

            if (string.IsNullOrEmpty(craftedItem))
            {
                //HideCookButton();
                Debug.LogWarning($"[InventoryManager] No valid crafting result for {item1} + {item2}.");
                return false;
            }

            return true; // The craft is possible
        }

        public void RemoveCraftingIngredients()
        {
            string item1 = InventorySystem.Instance.GetCurrentItem(1); // Slot 1 Item
            string item2 = InventorySystem.Instance.GetCurrentItem(2); // Slot 2 Item

            InventorySystem.Instance.ResetItemTexture(1);
            InventorySystem.Instance.ResetItemTexture(2);

            InventorySystem.Instance.RemoveItem(item1);
            InventorySystem.Instance.RemoveItem(item2);

            InventorySystem.Instance.InventoryPointersReset();
            InventorySystem.Instance.ResetCraftedTexture();
        }

        public void UpdateCookButton()
        {
            ulong cookButton = InternalCalls.Entity_GetEntityByName("Cookbutton");
            InternalCalls.SetEntityTextureByID(cookButton, "Potwithfire");

        }

        public void HideCookButton()
        {
            ulong cookButton = InternalCalls.Entity_GetEntityByName("Cookbutton");
            InternalCalls.SetEntityTextureByID(cookButton, "Cookbutton");
        }

        public bool HasTwoFoodItems()
        {
            string item1 = InventorySystem.Instance.GetCurrentItem(1); // Get item from slot 1
            string item2 = InventorySystem.Instance.GetCurrentItem(2); // Get item from slot 2

            // If both slots contain valid food items, return true
            return !string.IsNullOrEmpty(item1) && !string.IsNullOrEmpty(item2);
        }

        public void UpdateFoodAbilities(string FoodName)
        {
            switch (FoodName)
            {
                case "fish_cheese_stick":
                    ulong abilityBox = InternalCalls.Entity_GetEntityByName("FishCheeseStickAbility");
                    InternalCalls.Font_SetFont(abilityBox, "Attack t150", 0.5f);
                    break;

                case "baked_chicken_cheese":
                    ulong abilityBox1 = InternalCalls.Entity_GetEntityByName("BakedChickenCheeseAbility");
                    InternalCalls.Font_SetFont(abilityBox1, "Attack t10", 0.5f);
                    break;

                case "cheesy_sandwich":
                    ulong abilityBox2 = InternalCalls.Entity_GetEntityByName("CheesySandwichAbility");
                    InternalCalls.Font_SetFont(abilityBox2, "HPt500 Attack   20", 0.5f);

                    ulong minussign = InternalCalls.Entity_GetEntityByName("Minus");
                    Vector2D minusScale = new Vector2D(30.0f, 30.0f); // Set visible size
                    InternalCalls.Transform_SetScale(minussign, ref minusScale);
                    break;

                case "chicken_sandwich":
                    ulong abilityBox3 = InternalCalls.Entity_GetEntityByName("ChickenSandwichAbility");
                    InternalCalls.Font_SetFont(abilityBox3, "Restore Your Energy to Max", 0.5f);
                    break;

                case "nuggets_with_ketchup":
                    ulong abilityBox4 = InternalCalls.Entity_GetEntityByName("NuggetsWithKetchupAbility");
                    InternalCalls.Font_SetFont(abilityBox4, "Restore Your HP to Max", 0.5f);
                    break;

                case "fried_fish_with_ketchup":
                    ulong abilityBox5 = InternalCalls.Entity_GetEntityByName("FriedFishWithKetchupAbility");
                    InternalCalls.Font_SetFont(abilityBox5, "HP t250", 0.5f);
                    break;

                case "fish_toast":
                    ulong abilityBox6 = InternalCalls.Entity_GetEntityByName("FishToastAbility");
                    InternalCalls.Font_SetFont(abilityBox6, "Energy t2", 0.5f);
                    break; 

                case "tomato_soup_with_bread":
                    ulong abilityBox7 = InternalCalls.Entity_GetEntityByName("TomatoSoupWithBreadAbility");
                    InternalCalls.Font_SetFont(abilityBox7, "HP t100 Attack t20", 0.5f);
                    break;

                case "cheesy_roasted_tomatos":
                    ulong abilityBox8 = InternalCalls.Entity_GetEntityByName("CheesyRoastedTomatoAbility");
                    InternalCalls.Font_SetFont(abilityBox8, "Ranged Ability Unlocked", 0.5f);
                    break;

                default:
                return; // If no valid effect, stop execution before removing items
            }
        }

        public void HideFoodAbilities()
        {
            ulong abilityBox = InternalCalls.Entity_GetEntityByName("FishCheeseStickAbility");
            InternalCalls.Font_SetFont(abilityBox, "", 0.0f);

            ulong abilityBox1 = InternalCalls.Entity_GetEntityByName("BakedChickenCheeseAbility");
            InternalCalls.Font_SetFont(abilityBox1, "", 0.0f);

            ulong abilityBox2 = InternalCalls.Entity_GetEntityByName("CheesySandwichAbility");
            InternalCalls.Font_SetFont(abilityBox2, "", 0.0f);

            ulong minussign = InternalCalls.Entity_GetEntityByName("Minus");
            Vector2D minusScale = new Vector2D(0.0f, 0.0f); // Set visible size
            InternalCalls.Transform_SetScale(minussign, ref minusScale);

            ulong abilityBox3 = InternalCalls.Entity_GetEntityByName("ChickenSandwichAbility");
            InternalCalls.Font_SetFont(abilityBox3, "", 0.0f);

            ulong abilityBox4 = InternalCalls.Entity_GetEntityByName("NuggetsWithKetchupAbility");
            InternalCalls.Font_SetFont(abilityBox4, "", 0.0f);

            ulong abilityBox5 = InternalCalls.Entity_GetEntityByName("FriedFishWithKetchupAbility");
            InternalCalls.Font_SetFont(abilityBox5, "", 0.0f);

            ulong abilityBox6 = InternalCalls.Entity_GetEntityByName("FishToastAbility");
            InternalCalls.Font_SetFont(abilityBox6, "", 0.0f);

            ulong abilityBox7 = InternalCalls.Entity_GetEntityByName("TomatoSoupWithBreadAbility");
            InternalCalls.Font_SetFont(abilityBox7, "", 0.0f);

            ulong abilityBox8 = InternalCalls.Entity_GetEntityByName("CheesyRoastedTomatoAbility");
            InternalCalls.Font_SetFont(abilityBox8, "", 0.0f);
        }
    }
}
