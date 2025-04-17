/******************************************************************************
/*!
\file  GameMessageHub.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    March 11, 2025
\brief  Implements the central message hub for the observer pattern.

All content � 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;
using System.Collections.Generic;

namespace FruitLoops
{
    /**
     * @brief Central message hub that acts as both a subject and an observer.
     * 
     * This singleton class manages message passing between game components using
     * the observer pattern. Components can register as observers for specific message types
     * and can also send messages through this hub.
     */
    public class GameMessageHub : IGameSubject, IGameObserver
    {
        // Singleton instance
        private static GameMessageHub _instance;
        public static GameMessageHub Instance
        {
            get
            {
                if (_instance == null)
                {
                    _instance = new GameMessageHub();
                }
                return _instance;
            }
        }
        
        // Dictionary mapping message types to lists of observers
        private Dictionary<GameMessageType, List<IGameObserver>> _observers = 
            new Dictionary<GameMessageType, List<IGameObserver>>();
        
        // Private constructor (singleton)
        private GameMessageHub() {}
        
        /**
         * @brief Register an observer for all message types.
         * 
         * @param observer The observer to add.
         */
        public void AddObserver(IGameObserver observer)
        {
            foreach (GameMessageType messageType in Enum.GetValues(typeof(GameMessageType)))
            {
                RegisterObserver(messageType, observer);
            }
        }
        
        /**
         * @brief Register an observer for a specific message type.
         * 
         * @param messageType The message type to observe.
         * @param observer The observer to add.
         */
        public void RegisterObserver(GameMessageType messageType, IGameObserver observer)
        {
            if (!_observers.ContainsKey(messageType))
            {
                _observers[messageType] = new List<IGameObserver>();
            }
            
            if (!_observers[messageType].Contains(observer))
            {
                _observers[messageType].Add(observer);
                Debug.LogInfo("[GameMessageHub] Observer registered for {0}", messageType);
            }
        }
        
        /**
         * @brief Unregister an observer from all message types.
         * 
         * @param observer The observer to remove.
         */
        public void RemoveObserver(IGameObserver observer)
        {
            foreach (var type in _observers.Keys)
            {
                if (_observers[type].Contains(observer))
                {
                    _observers[type].Remove(observer);
                }
            }
        }
        
        /**
         * @brief Unregister an observer from a specific message type.
         * 
         * @param messageType The message type.
         * @param observer The observer to remove.
         */
        public void UnregisterObserver(GameMessageType messageType, IGameObserver observer)
        {
            if (_observers.ContainsKey(messageType))
            {
                _observers[messageType].Remove(observer);
            }
        }
        
        /**
         * @brief Send a message to all registered observers for the corresponding message type.
         * 
         * @param message The message to send.
         */
        public void NotifyObservers(GameMessage message)
        {
            //Debug.LogInfo("[GameMessageHub] Broadcasting message: {0}", message.MessageType);
            
            if (_observers.TryGetValue(message.MessageType, out var observerList))
            {
                // Create a copy of the list to prevent modification issues during iteration
                var observers = new List<IGameObserver>(observerList);
                
                foreach (var observer in observers)
                {
                    try
                    {
                        observer.OnNotify(message);
                    }
                    catch (Exception ex)
                    {
                        Debug.LogError("[GameMessageHub] Error notifying observer: {0}", ex.Message);
                    }
                }
            }
        }
        
        /**
         * @brief Implementation for IGameObserver interface.
         * 
         * When acting as an observer, the hub simply re-broadcasts the message.
         * 
         * @param message The message received.
         */
        public void OnNotify(GameMessage message)
        {
            // When acting as an observer, just re-broadcast the message
            NotifyObservers(message);
        }

        /**
         * @brief Convenient method to send a message directly through the hub.
         * 
         * @param messageType The type of message.
         * @param source The source entity.
         * @param target The target entity (optional).
         * @param value A numeric value to include (optional).
         * @param position A position vector (optional).
         * @param data Additional data (optional).
         */
        public void SendMessage(
            GameMessageType messageType,
            Entity source = null,
            Entity target = null,
            int value = 0,
            Vector2D? position = null,
            object data = null)
        {
            var message = new GameMessage(messageType, source, target, value, position ?? Vector2D.Zero, data);
            NotifyObservers(message);
        }
    }
}
