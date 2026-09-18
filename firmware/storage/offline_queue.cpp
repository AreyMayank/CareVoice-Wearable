#include "offline_queue.h"

OfflineQueue::OfflineQueue(size_t max_capacity)
    : capacity(max_capacity) {}

bool OfflineQueue::push(const EventPayload& payload) {
    if (isFull()) {
        // Drop oldest non-emergency event if queue overflows
        queue.erase(queue.begin());
    }
    queue.push_back(payload);
    return true;
}

bool OfflineQueue::pop(EventPayload& out_payload) {
    if (isEmpty()) return false;
    out_payload = queue.front();
    queue.erase(queue.begin());
    return true;
}

bool OfflineQueue::peek(EventPayload& out_payload) const {
    if (isEmpty()) return false;
    out_payload = queue.front();
    return true;
}

size_t OfflineQueue::count() const {
    return queue.size();
}

bool OfflineQueue::isEmpty() const {
    return queue.empty();
}

bool OfflineQueue::isFull() const {
    return queue.size() >= capacity;
}

void OfflineQueue::clear() {
    queue.clear();
}
