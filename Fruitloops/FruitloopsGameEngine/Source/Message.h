/**
 * @file Message.h
 * @author Anson Teng
 * @date  Nov 8, 2024
 * @brief This file contains the definition of the MessageManager class and various
 *        event/message classes used in the FruitLoops game engine.
 *
 * The MessageManager class manages dynamic message types, allowing for registration and
 * retrieval of message IDs. The IMessage class serves as the base class for all event-related
 * messages, with subclasses like Cooking, ItemCollectedMessage, and others representing
 * specific game events.
 *
 * Messages are identified using EventID, and each message type contains relevant data.
 *
 * 
 * @note EventID is used as a unique identifier for different message types, and entities
 *       and actions are associated with each message.
 * 
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once
#include "GameTypes.h"

namespace FruitLoops {

    /**
     * @class MessageManager
     * @brief A class that manages dynamic message types as EventIDs.
     *
     * The MessageManager class provides static functions to register and retrieve message IDs.
     * It allows for the dynamic addition of message types that can be used throughout the game.
     */    class MessageManager {
    public:
        /**
         * @brief Retrieves the list of registered message IDs.
         *
         * This function returns a reference to a static vector that holds all registered EventID types.
         * The list of message IDs is used to track the available event types.
         *
         * @return A reference to a vector containing EventID values.
         */
        static std::vector<EventID>& GetMessageIds() {
            static std::vector<EventID> messageIds;
            return messageIds;
        }

        /**
         * @brief Registers a new message ID to the list of available message types.
         *
         * This function allows for the dynamic registration of new EventID types.
         * It adds a new EventID to the internal list of message IDs, enabling it to be used
         * for message handling throughout the game.
         *
         * @param id The EventID to register.
         */
        static void RegisterMessageId(const EventID& id) {
            GetMessageIds().push_back(id);
        }
    };

     /**
      * @class IMessage
      * @brief A base class for event messages.
      *
      * IMessage serves as the base class for all event messages. It encapsulates an EventID,
      * which uniquely identifies the message type. Specific message types inherit from IMessage
      * and add data relevant to the event.
      */
    class IMessage {
    protected:
        EventID messageID;

    public:
        /**
         * @brief Constructor for the base IMessage class.
         *
         * This constructor initializes the message ID with a default value. It is typically used
         * by derived classes to set their own message ID.
         */
        IMessage() : messageID() {}

        /**
         * @brief Constructor for the IMessage class with a specific EventID.
         *
         * This constructor initializes the message with a given EventID, which identifies the
         * type of message.
         *
         * @param id The EventID that identifies the message type.
         */
        IMessage(const EventID& id) : messageID(id) {}
        virtual ~IMessage() = default;

        /**
         * @brief Retrieves the EventID associated with this message.
         *
         * This function returns the EventID that uniquely identifies the type of message.
         * It is used by observers and the MessageManager to identify and handle specific messages.
         *
         * @return The EventID of the message.
         */
        inline EventID GetId() const noexcept { return messageID; }
    };


    //events below

    /**
     * @class Cooking
     * @brief Represents a message for a crafting event.
     *
     * This message is used to notify that a crafting process has occurred, including
     * the items involved in the crafting and the resulting crafted item.
     */

    class Cooking : public IMessage {
    public:
        /**
         * @brief Constructor for a Cooking message.
         *
         * This constructor initializes a Cooking message with two items to be crafted together
         * and the resulting crafted item.
         *
         * @param item1 The first item in the crafting process.
         * @param item2 The second item in the crafting process.
         * @param craftedItem The resulting crafted item.
         */
        Cooking(Entity item1, Entity item2, Entity craftedItem)
            : IMessage(EventID::Cooking), item1ID(item1), item2ID(item2), craftedItemID(craftedItem) {}

        // Additional information for crafting (example: item IDs or quantities)
        Entity item1ID;
        Entity item2ID;
        Entity craftedItemID;
    };

    /**
     * @class ItemCollectedMessage
     * @brief Represents a message for when an item is collected.
     *
     * This message is used to notify that an item has been collected by a player.
     */
    class ItemCollectedMessage : public IMessage {
    public:

        /**
         * @brief Constructor for an ItemCollectedMessage.
         *
         * This constructor initializes the message with the player and item that were involved
         * in the collection event.
         *
         * @param character The player who collected the item.
         * @param item The item that was collected.
         */
        ItemCollectedMessage(Entity character, Entity item)
            : IMessage(EventID::ItemCollected), Player(character), itemID(item) {}
        Entity Player;
        Entity itemID;
    };

    /**
 * @class TurnEndEvent
 * @brief Represents a message for when a turn ends.
 *
 * This message notifies that a turn has ended and contains information about the
 * specific turn that is ending.
 */
    class TurnEndEvent : public IMessage {
    public:
        /**
         * @brief Constructor for a TurnEndEvent message.
         *
         * This constructor initializes the message with the ending turn's state.
         *
         * @param endingTurn The state of the turn that is ending.
         */
        explicit TurnEndEvent(TurnState endingTurn)
            : IMessage(EventID::TurnEnd), turnEnding(endingTurn) {}

        TurnState turnEnding; // Indicates which turn is ending
    };

    class TurnStartEvent : public IMessage {
    public:

    };

    /**
     * @class ObjectSpawnedMessage
     * @brief Represents a message for when an object is spawned.
     *
     * This message is used to notify that an object has been spawned in the game world.
     */
    class ObjectSpawnedMessage : public IMessage {
    public:
        /**
         * @brief Constructor for an ObjectSpawnedMessage.
         *
         * This constructor initializes the message with the entity that has been spawned.
         *
         * @param entity The entity that was spawned.
         */
        ObjectSpawnedMessage(Entity entity)
            : IMessage(EventID::ObjectSpawned), spawnedEntity(entity) {}
        Entity spawnedEntity;
    };

    class FontSpawnedMessage : public IMessage {
    public:
        /**
         * @brief Constructor for an ObjectSpawnedMessage.
         *
         * This constructor initializes the message with the entity that has been spawned.
         *
         * @param entity The entity that was spawned.
         */
        FontSpawnedMessage(Entity entity)
            : IMessage(EventID::FontSpawned), spawnedEntity(entity) {}
        Entity spawnedEntity;
    };

    class FontDeletedMessage : public IMessage {
    public:
        /**
 * @brief Constructor for an ObjectDeletedMessage.
 *
 * This constructor initializes the message with the entity that was deleted.
 *
 * @param entity The entity that was deleted.
 */
        FontDeletedMessage(Entity entity)
            : IMessage(EventID::FontDeleted), deletedEntity(entity) {}
        Entity deletedEntity;
    };

    /**
 * @class ObjectDeletedMessage
 * @brief Represents a message for when an object is deleted.
 *
 * This message is used to notify that an object has been deleted from the game world.
 */
    class ObjectDeletedMessage : public IMessage {
    public:
        /**
 * @brief Constructor for an ObjectDeletedMessage.
 *
 * This constructor initializes the message with the entity that was deleted.
 *
 * @param entity The entity that was deleted.
 */
        ObjectDeletedMessage(Entity entity)
            : IMessage(EventID::ObjectDeleted), deletedEntity(entity) {}
        Entity deletedEntity;
    };

    /**
 * @class KeyPressMessage
 * @brief Represents a message for when a key is pressed.
 *
 * This message is used to notify when a key press event occurs in the game.
 */
    class KeyPressMessage : public IMessage {
    public:
        /**
 * @brief Constructor for a KeyPressMessage.
 *
 * This constructor initializes the message with the key pressed and its action (press or release).
 *
 * @param key The key that was pressed.
 * @param action The action (press or release) associated with the key.
 */
        KeyPressMessage(int key, int action)
            : IMessage(EventID::KeyPress), key(key), action(action) {}

        int key;
        int action;
    };

    class RigidBodyComponentChange : public IMessage {
    public:

        RigidBodyComponentChange(bool action)
            : IMessage(EventID::RigidBodyComponentChange), act(action) {}

        bool act;
    };

    class DrawGridMessage : public IMessage {
    public:

        DrawGridMessage(bool action)
            : IMessage(EventID::DrawGrid), act(action) {}

        bool act;
    };

    class ObjectSelectedMessage : public IMessage {
    public:

        ObjectSelectedMessage(Entity ent)
            : IMessage(EventID::ObjectSelected), entity(ent) {}

        Entity entity;
    };

}