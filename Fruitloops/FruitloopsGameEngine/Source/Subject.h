/**
 * @file Subject.h
 * @author Anson Teng
 * @date  Nov 8, 2024
 * @brief This file contains the definition of the Subject class, which is part of the
 *        observer pattern implementation.
 *
 * The Subject class is responsible for managing and notifying multiple observers about
 * specific events or messages. It allows observers to register and unregister for
 * specific events, and notifies them when an event occurs.
 *
 * Observers can be notified either through message-specific handlers (when a message
 * is received) or general event handlers (when a general event occurs).
 *
 * @note This class uses EventID as a unique identifier for different event types.
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once
#include "Observer.h" // Ensure you include the observer header

namespace FruitLoops {
    
    /**
     * @class Subject
     * @brief A class that manages and notifies observers about events.
     *
     * The Subject class maintains a collection of observers that are interested in specific
     * events, identified by EventID. Observers can register for specific events, and the
     * Subject class will notify all registered observers when the event occurs.
     *
     * The notifications can be directed to message-specific handlers (if available) or
     * general event handlers.
     */

    class Subject {
    private:
        // Multimap to handle multiple observers for a single message type
        std::unordered_multimap<EventID, Observer*> observers;

    public:
        /**
         * @brief Registers an observer for a specific event type (EventID).
         *
         * This function registers an observer to be notified when an event with the specified
         * EventID occurs. Multiple observers can be registered for the same event.
         *
         * @param messageId The EventID identifying the event type to register for.
         * @param observer A pointer to the Observer to be notified when the event occurs.
         */

        void RegisterObserver(const EventID& messageId, Observer* observer) {
            observers.emplace(messageId, observer);
            //spdlog::info("Observer {} registered for EventID {}", observer->GetId(), static_cast<int>(messageId));
        }

        /**
         * @brief Unregisters an observer from a specific event type (EventID).
         *
         * This function removes the observer from the list of observers for a specific event
         * type. If the observer is registered for multiple event types, it will still be notified
         * for other events it is registered to.
         *
         * @param messageId The EventID identifying the event type to unregister from.
         * @param observer A pointer to the Observer to remove from the notification list.
         */
        void UnregisterObserver(const EventID& messageId, Observer* observer) {
            auto range = observers.equal_range(messageId);
            for (auto it = range.first; it != range.second; ) {
                if (it->second == observer) {
                    it = observers.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

        /**
         * @brief Notifies all registered observers for a specific message.
         *
         * This function notifies all observers that are registered for the event type associated
         * with the message's EventID. It will first attempt to notify message-specific handlers
         * and then attempt to notify general event handlers if no message-specific handler is found.
         *
         * @param message The message to notify observers about, containing the EventID for the event.
         *
         * @note The message is deleted after notification to avoid memory leaks.
         */
         void NotifyObservers(IMessage* message) {
            auto range = observers.equal_range(message->GetId());
            for (auto it = range.first; it != range.second; ++it) {
                Observer* observer = it->second;

                // Try notifying message-specific handlers first
                observer->NotifyMessageHandler(message);
                //spdlog::info("Notifying observer: {}", observer->GetId());

                // If no specific handler for IMessage* was registered, notify general handlers
                observer->NotifyGeneralHandler(message->GetId());
            }
            delete message;
         }
    };
}