using FruitLoops;
using System;
using System.Collections.Generic;

namespace Fruitloops
{
    /**
     * @class Rock
     * @brief Behaviour component for managing rocks in the game world.
     *
     * This component handles the rock's HP and destruction when its HP reaches zero.
     */
    public class Rock : BehaviourComponent
    {
        private readonly Unit _unit;
        private HealthBarComponent _healthBarComponent;

        // -----------------------[ CONSTRUCTORS ]---------------------------
        public Rock() { }

        /**
        * @brief Constructs a Rock instance with specified attributes.
        *
        * This constructor initializes the rock's position and HP, and registers the rock's behavior.
        *
        * @param entity The entity representing the rock.
        * @param position The initial position of the rock in the world.
        * @param health The current health of the rock.
        * @param maxHealth The maximum health of the rock.
        */
        private Rock(Entity entity, Vector2D position, int health, int maxHealth)
        {
            Debug.LogInfo("[Rock] Rock constructor called!");
            _unit = new Unit(position, health, maxHealth, 0, 0, 0);

            var behaviour = new BehaviourFCT(entity, Start, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            // Initialize HealthBarComponent
            Entity healthBarFiller = new Entity(InternalCalls.Entity_GetEntityByName("rock_healthbar(filler)"));
            Entity healthBarEmpty = new Entity(InternalCalls.Entity_GetEntityByName("rock_healthbar(empty)"));
            _healthBarComponent = new HealthBarComponent(entity, healthBarFiller, healthBarEmpty);
        }

        /**
         * @brief Factory method for creating a Rock behaviour component.
         *
         * This method extracts rock attributes from the entity and initializes a Rock instance.
         *
         * @param entity The entity that will be associated with the Rock component.
         * @return A new Rock instance linked to the given entity.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            Debug.LogInfo("[Rock][CreateBehaviour] Starting for entity {0}", entity.ID);

            Vector2D position = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(entity.ID, ref position);

            int health = InternalCalls.UnitComponent_GetHealth(entity.ID);
            int maxHealth = InternalCalls.UnitComponent_GetMaxHealth(entity.ID);

            Debug.LogInfo("[Rock] Stats: Position={0}, Health={1}/{2}", position, health, maxHealth);

            return new Rock(entity, position, health, maxHealth);
        }

        // -----------------------[ LIFECYCLE METHODS ]----------------------
        private static void Start(Entity entity)
        {
            Debug.LogInfo("[Rock] Start called for entity {0}", entity.ID);
        }

        private static void Update(Entity entity, float deltaTime)
        {
            var rock = GameLogicSystem.Instance.GetBehaviour<Rock>(entity);
            if (rock == null)
                return;

            // Update health bar
            rock._healthBarComponent.UpdateHealthBar();

            // Check if rock should be destroyed
            int currentHealth = InternalCalls.UnitComponent_GetHealth(entity.ID);
            if (currentHealth <= 0)
            {
                Debug.LogInfo("[Rock] Rock {0} has been destroyed.", entity.ID);
                DestroyRock(entity);
            }
        }

        private static void End(Entity entity)
        {
            Debug.LogInfo("[Rock] Cleanup complete for entity {0}", entity.ID);
        }

        // -----------------------[ ROCK DESTRUCTION ]----------------------
        private static void DestroyRock(Entity entity)
        {
            // Play destruction animation & sound
            InternalCalls.AnimationComponent_PlayAnimation(entity.ID, "RockBreak");
            InternalCalls.Audio_PlaySound("SFX_Rock_Break");

            // Remove rock entity from the game
            //GameLogicSystem.Instance.DestroyEntity(entity);

            Debug.LogInfo("[Rock] Entity {0} removed from game.", entity.ID);
        }
    }
}
