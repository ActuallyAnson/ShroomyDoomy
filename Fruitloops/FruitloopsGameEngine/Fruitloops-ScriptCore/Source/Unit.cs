/******************************************************************************
/*!
\file  Unit.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    March 11, 2025
\brief  This file implements the Unit class, which represents a game entity 
		with health, energy, and movement capabilities. 

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

using System;

namespace FruitLoops
{
	public class Unit
	{
		public Vector2D Position { get; set; }
		public int Health { get; private set; }
		public int MaxHealth { get; private set; }
		public int AttackPower { get; private set; }
		public int Energy { get; private set; }
		public int MaxEnergy { get; private set; }

		public bool IsAlive {get; private set; }

		public bool IsMoving { get; private set; }
		
		public int Speed { get; private set; }

		public Unit(Vector2D position, int health, int maxHealth, int attackPower, int energy, int maxEnergy, bool isAlive, bool isMoving, int speed)
		{
			Position = position;
			Health = health;
			MaxHealth = maxHealth;
			AttackPower = attackPower;
			Energy = energy;
			MaxEnergy = maxEnergy;
            IsAlive = isAlive;
			IsMoving = isMoving;
			Speed = speed;
		}

		public virtual void Update(float deltaTime)
		{
			//Console.WriteLine($"Unit updating with deltaTime {deltaTime}");
			// General update logic for all units
		}

		public void ResetHealth()
		{
			Health = MaxHealth;
		}

		public void ResetEnergy()
		{
			Energy = MaxEnergy;
		}

		public void ResetStats()
		{
			Position = new Vector2D(); // Reset position to default (0,0)
			ResetHealth();
			ResetEnergy();
		}
	}
}
