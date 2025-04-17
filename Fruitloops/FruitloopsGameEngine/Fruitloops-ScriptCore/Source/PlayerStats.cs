using FruitLoops;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Fruitloops
{
    public static class PlayerStats
    {
        public static int Health = 100;
        public static int MaxHealth = 100;
        public static int AttackPower = 50;
        public static int Energy = 3;
        public static int MaxEnergy = 3;
        public static bool IsAlive = true;
        public static bool RangedUnlocked = false;

        public static void SaveFromPlayer(ulong entityID)
        {
            Entity entity = new Entity(entityID);

            Health = InternalCalls.UnitComponent_GetHealth(entity.ID);
            MaxHealth = InternalCalls.UnitComponent_GetMaxHealth(entity.ID);
            AttackPower = InternalCalls.UnitComponent_GetAttackPower(entity.ID);
            Energy = InternalCalls.UnitComponent_GetEnergy(entity.ID);
            MaxEnergy = InternalCalls.UnitComponent_GetMaxEnergy(entity.ID);
            IsAlive = InternalCalls.UnitComponent_GetIsAlive(entity.ID);
            RangedUnlocked = Player.rangedunlocked;
        }


        public static void ApplyToPlayer(Entity entity)
        {
            InternalCalls.UnitComponent_SetHealth(entity.ID, Health);
            InternalCalls.UnitComponent_SetMaxHealth(entity.ID, MaxHealth);
            InternalCalls.UnitComponent_SetAttackPower(entity.ID, AttackPower);
            InternalCalls.UnitComponent_SetMaxEnergy(entity.ID, MaxEnergy);
            InternalCalls.UnitComponent_SetIsAlive(entity.ID, IsAlive);
            Player.rangedunlocked = RangedUnlocked;
        }

        public static void ResettoDefault(Entity entity)
        {
            InternalCalls.UnitComponent_SetHealth(entity.ID, 100);
            InternalCalls.UnitComponent_SetMaxHealth(entity.ID, 100);
            InternalCalls.UnitComponent_SetAttackPower(entity.ID, 10);
            InternalCalls.UnitComponent_SetMaxEnergy(entity.ID, 4);
            InternalCalls.UnitComponent_SetIsAlive(entity.ID, IsAlive);
            Player.rangedunlocked = RangedUnlocked;
        }
    }
}
