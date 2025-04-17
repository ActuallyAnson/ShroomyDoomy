/**
 * @file Observer.h
 * @author Anson Teng
 * @brief This file contains the definition of the Observer class.
 * @date  Nov 8, 2024
 *
 * The Observer class allows for the registration and notification of message handlers
 * in response to specific events. It supports handling both message-specific events
 * (with IMessage) and general events (with void handler functions).
 *
 * The Observer pattern is used to notify registered handlers when an event occurs.
 * The class is designed to handle events using an EventID, which uniquely identifies
 * the event type.
 *
 * @note This class relies on the EventID to identify events, with two separate handler
 *       maps to differentiate message-specific handlers and general handlers.
 *  Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once

#include "Message.h"


namespace FruitLoops {

    /**
 * @class Observer
 * @brief A class that observes and handles events in the system.
 *
 * The Observer class is part of the observer pattern, designed to listen for events
 * and notify registered handlers when those events occur. Handlers can be registered
 * for specific message types (using IMessage) or for general event types that do not
 * require message data.
 *
 * Handlers are registered using an EventID, which serves as a unique identifier for
 * each type of event.
 */

    class Observer {
    private:
        std::string messageID;
        // OLD ---- Map of message IDs to handler functions (used to be just imessage)
        //std::unordered_map<std::string, std::function<void(IMessage*)>> m_handlers;

        // Separate maps for message-type and general handlers using EventID
        std::unordered_map<EventID, std::function<void(IMessage*)>> messageHandlers;
        std::unordered_map<EventID, std::function<void()>> generalHandlers;

    public:
        /**
 * @brief Constructs an Observer with a specific ID.
 *
 * The constructor initializes the Observer with a unique identifier, which can be used
 * to distinguish this Observer from others.
 *
 * @param id The unique identifier for the Observer.
 */
        Observer(const std::string& id) : messageID(id) {}

        // Register a handler for specific message types using EventID
        /**
         * @brief Registers a handler for a specific message type.
         *
         * This function registers a handler function for an event identified by the given EventID.
         * The handler will be called when a message with the corresponding EventID is received.
         *
         * @param eventId The EventID identifying the event type.
         * @param handler The function to handle the event, which takes a pointer to an IMessage object.
         */
        void RegisterHandler(EventID eventId, std::function<void(IMessage*)> handler) {
            messageHandlers[eventId] = handler;
        }

        /**
         * @brief Registers a handler for a general event without message data.
         *
         * This overloaded function allows the registration of a handler function for an event
         * that does not require any message data. The handler will be called when the corresponding
         * EventID is notified.
         *
         * @param eventId The EventID identifying the event type.
         * @param handler The function to handle the event, which takes no arguments.
         */
        void RegisterHandler(EventID eventId, std::function<void()> handler) {
            generalHandlers[eventId] = handler;
        }


        // Unregister both message-specific and general handlers
        /**
         * @brief Unregisters a handler for a specific event.
         *
         * This function removes both message-specific and general handlers associated with
         * the given EventID. After calling this function, no handler will be called for that event.
         *
         * @param eventId The EventID identifying the event type to unregister.
         */
        void UnregisterHandler(EventID eventId) {
            messageHandlers.erase(eventId);
            generalHandlers.erase(eventId);
        }

        /**
          * @brief Notifies the appropriate handler for a message event.
          *
          * This function looks up and calls the handler registered for a specific message
          * type identified by the EventID of the message. It is called when a message is received
          * by the Observer.
          *
          * @param message The message that triggered the notification.
          */
        virtual void NotifyMessageHandler(IMessage* message) {
            auto it = messageHandlers.find(message->GetId());
            if (it != messageHandlers.end()) {
                it->second(message); // Call specific message handler
            }
        }

        // Notify for general (void) handlers
        /**
         * @brief Notifies the appropriate handler for a general event.
         *
         * This function looks up and calls the general event handler registered for the
         * given EventID. It is called when an event occurs that does not require message data.
         *
         * @param eventId The EventID of the event to notify.
         */
        virtual void NotifyGeneralHandler(EventID eventId) {
            auto it = generalHandlers.find(eventId);
            if (it != generalHandlers.end()) {
                it->second();  // Call general handler
            }
        }

        /**
         * @brief Gets the unique ID of the Observer.
         *
         * This function returns the unique identifier of the Observer, which was set during
         * construction.
         *
         * @return The ID of the Observer.
         */
        std::string GetId() const { return messageID; }
    };

}