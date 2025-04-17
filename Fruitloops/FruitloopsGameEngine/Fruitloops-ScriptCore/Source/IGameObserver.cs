/******************************************************************************
/*!
\file  IGameObserver.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    March 11, 2025
\brief  Defines the observer interface for the observer pattern.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

namespace FruitLoops
{
    /**
     * @brief Interface for objects that observe game messages.
     */
    public interface IGameObserver
    {
        /**
         * @brief Method called when a subscribed event occurs.
         * 
         * @param message The message containing event details.
         */
        void OnNotify(GameMessage message);
    }
}
