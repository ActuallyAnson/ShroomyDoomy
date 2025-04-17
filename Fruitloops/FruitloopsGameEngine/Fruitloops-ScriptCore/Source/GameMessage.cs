/******************************************************************************
/*!
\file  GameMessage.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    March 11, 2025
\brief  Defines the message data container for the observer pattern.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

namespace FruitLoops
{
    /**
     * @brief Data container for messages in the observer pattern.
     */
    public class GameMessage
    {
        // Source entity that generated the message
        public Entity Source { get; private set; }
        
        // Optional target entity (for attacks, etc.)
        public Entity Target { get; private set; }
        
        // Message type
        public GameMessageType MessageType { get; private set; }
        
        // Additional value (damage amount, health change, etc.)
        public int Value { get; private set; }
        
        // Position data if needed
        public Vector2D Position { get; private set; }
        
        // Additional data object
        public object Data { get; private set; }

        // Constructor with all parameters
        public GameMessage(
            GameMessageType messageType,
            Entity source,
            Entity target,
            int value,
            Vector2D position,
            object data)
        {
            MessageType = messageType;
            Source = source;
            Target = target;
            Value = value;
            Position = position;
            Data = data;
        }

        // Constructor without position
        public GameMessage(
            GameMessageType messageType,
            Entity source = null,
            Entity target = null,
            int value = 0,
            object data = null)
            : this(messageType, source, target, value, Vector2D.Zero, data)
        {
        }
    }
}
