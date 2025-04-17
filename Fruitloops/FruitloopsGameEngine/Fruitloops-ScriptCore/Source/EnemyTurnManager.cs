/******************************************************************************
/*!
\file  EnemyTurnManager.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    March 11, 2025
\brief  Priority queue implementation for enemy turns.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

using System;
using System.Collections.Generic;

namespace FruitLoops
{
    public class PriorityQueue<T>
    {
        private List<Tuple<T, int>> _elements = new List<Tuple<T, int>>();

        /** @brief Gets the number of elements in the priority queue. */
        public int Count => _elements.Count;

        /**
         * @brief Adds an item to the priority queue with a specified priority.
         * 
         * @param item The item to enqueue.
         * @param priority The priority value (higher values indicate higher priority).
         */
        public void Enqueue(T item, int priority)
        {
            _elements.Add(Tuple.Create(item, priority));
        }

        /**
         * @brief Removes and returns the item with the highest priority.
         * 
         * @return The item with the highest priority.
         * @note Uses a linear search to find the highest priority item and removes it.
         */
        public T Dequeue()
        {
            int bestIndex = 0;
            for (int i = 0; i < _elements.Count; i++)
            {
                if (_elements[i].Item2 > _elements[bestIndex].Item2)
                {
                    bestIndex = i;
                }
            }
            T bestItem = _elements[bestIndex].Item1;
            _elements.RemoveAt(bestIndex);
            return bestItem;
        }

        /**
         * @brief Clears all elements from the priority queue.
         */
        public void Clear()
        {
            _elements.Clear();
        }
    }

    public class EnemyTurnManager
    {
        private PriorityQueue<Entity> _enemyQueue = new PriorityQueue<Entity>();
        private Entity _currentEnemy = null;

        /**
         * @brief Starts an enemy turn by queuing alive enemies based on speed.
         * 
         * @param enemies The list of enemy entities to process.
         * @note Clears the queue, enqueues alive enemies with speed-based priority,
         * and activates the first enemy.
         */
        public void StartEnemyTurn(List<Entity> enemies)
        {
            _enemyQueue.Clear();
            foreach (var enemy in enemies)
            {
                if (InternalCalls.UnitComponent_GetIsAlive(enemy.ID))
                {
                    int speed = InternalCalls.UnitComponent_GetSpeed(enemy.ID);
                    _enemyQueue.Enqueue(enemy, speed);
                }
            }
            ActivateNextEnemy();
        }

        /**
         * @brief Retrieves the currently active enemy entity.
         * 
         * @return The Entity representing the current enemy, or null if none active.
         */
        public Entity GetCurrentEnemy() => _currentEnemy;

        /**
         * @brief Marks an enemy as finished and activates the next enemy.
         * 
         * @param entity The Entity that finished its turn.
         * @note Only proceeds if the finished entity matches the current enemy.
         */
        public void EnemyFinished(Entity entity)
        {
            if (_currentEnemy?.ID == entity.ID)
            {
                ActivateNextEnemy();
            }
        }

        /**
         * @brief Activates the next enemy in the queue or switches turns if none remain.
         * 
         * @note Dequeues the next enemy, resets its energy, and sets it as current;
         * if the queue is empty, switches back to player turn.
         */
        private void ActivateNextEnemy()
        {
            if (_enemyQueue.Count > 0)
            {
                _currentEnemy = _enemyQueue.Dequeue();
                InternalCalls.UnitComponent_SetEnergy(_currentEnemy.ID,
                    InternalCalls.UnitComponent_GetMaxEnergy(_currentEnemy.ID));
            }
            else
            {
                _currentEnemy = null;
                UnitManager.Instance.SwitchTurn(); // Switch back to player
            }
        }
    }
}