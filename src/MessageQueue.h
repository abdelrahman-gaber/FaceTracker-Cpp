#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <mutex>
#include <deque>
#include <condition_variable>

template <class T>
class MessageQueue
{
public:
    void Send(T&& msg) {
        std::lock_guard<std::mutex> uLock(_mutex);
        // add the coming element to the queue
        if (_queue.size() < _max_size) {
            _queue.emplace_back(std::move(msg));
        }
        else {
            // remove one element from the front and add the new elemnt to the back of the queue
            _queue.pop_front();
            _queue.emplace_back(std::move(msg));
        }
        _condition.notify_one();
    }

    T Receive() {
        std::unique_lock<std::mutex> uLock(_mutex);
        _condition.wait(uLock, [this] { return !_queue.empty(); });

        // return one element from the front of the queue
        T msg = std::move(_queue.front());
        _queue.pop_front();
        return msg;
    }

    void SetMaxQueueSize(int max_size) {
        _max_size = max_size;
    }

    int GetQueueSize() {
        std::lock_guard<std::mutex> uLock(_mutex);
        int size = _queue.size();
        return size;
    }

private:
    int _max_size=100;
    std::deque<T> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif
