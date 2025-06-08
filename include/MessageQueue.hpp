#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class MessageQueue {
    private:
        std::queue<T> queue_;
        mutable std::mutex mutex_;
        std::condition_variable cond_;
    
    public:
        // Adds item to the queue
        void push(const T& item) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                queue_.push(item);
            }
            cond_.notify_one();
        }

        // Pops item from the queue and blocks if needed
        T pop() {
            std::unique_lock<std::mutex> lock(mutex_);

            while (queue_.empty()) {
                cond_.wait(lock);
            }
            
            T item = queue_.front();
            queue_.pop();
            return item;
        }

        // Checks if the queue is empty
        bool empty() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.empty();
        }
};
