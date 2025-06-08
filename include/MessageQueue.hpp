#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

// Not using std namespace to avoid conflicts with other libraries
using std::queue;
using std::mutex;
using std::lock_guard;
using std::unique_lock;
using std::condition_variable;

template <typename T>
class MessageQueue {
    private:
        queue<T> queue_;
        mutable mutex mutex_;
        condition_variable cond_;
    
    public:
        // Adds item to the queue
        void push(const T& item) {
            {
                lock_guard<mutex> lock(mutex_);
                queue_.push(item);
            }
            // lock is released since scope changes
            cond_.notify_one();
        }

        // Pops item from the queue and blocks if needed
        T pop() {
            unique_lock<mutex> lock(mutex_);

            // Wait until there is an item in the queue
            while (queue_.empty()) {
                cond_.wait(lock);
            }
            
            // Pop item
            T item = queue_.front();
            queue_.pop();
            return item;
        }

        // Checks if the queue is empty
        bool empty() const {
            lock_guard<mutex> lock(mutex_);
            return queue_.empty();
        }
};