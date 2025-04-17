/******************************************************************************
/*!
\file  IGameSubject.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    March 11, 2025
\brief  Defines the subject interface for the observer pattern.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

namespace FruitLoops
{
    /**
     * @brief Interface for objects that send game messages.
     */
    public interface IGameSubject
    {
        /**
         * @brief Adds an observer to be notified of events.
         * 
         * @param observer The observer to add.
         */
        void AddObserver(IGameObserver observer);
        
        /**
         * @brief Removes an observer.
         * 
         * @param observer The observer to remove.
         */
        void RemoveObserver(IGameObserver observer);
        
        /**
         * @brief Notifies all registered observers about an event.
         * 
         * @param message The message containing event details.
         */
        void NotifyObservers(GameMessage message);
    }
}
