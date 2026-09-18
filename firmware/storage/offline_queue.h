#ifndef OFFLINE_QUEUE_H
#define OFFLINE_QUEUE_H

#include "../events/event_manager.h"
#include <vector>
#include <cstddef>

class OfflineQueue {
public:
    OfflineQueue(size_t max_capacity = 20);

    bool push(const EventPayload& payload);
    bool pop(EventPayload& out_payload);
    bool peek(EventPayload& out_payload) const;
    size_t count() const;
    bool isEmpty() const;
    bool isFull() const;
    void clear();

private:
    size_t capacity;
    std::vector<EventPayload> queue;
};

#endif // OFFLINE_QUEUE_H
