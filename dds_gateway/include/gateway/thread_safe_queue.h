#ifndef GATEWAY_THREAD_SAFE_QUEUE_H
#define GATEWAY_THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

namespace gateway {

template <typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(size_t max_size = 4096)
        : max_size_(max_size), stopped_(false) {}

    bool push(const T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_full_.wait(lock, [this] { return queue_.size() < max_size_ || stopped_; });
        if (stopped_) return false;
        queue_.push(item);
        cv_empty_.notify_one();
        return true;
    }

    bool pop(T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_empty_.wait(lock, [this] { return !queue_.empty() || stopped_; });
        if (stopped_ && queue_.empty()) return false;
        item = queue_.front();
        queue_.pop();
        cv_full_.notify_one();
        return true;
    }

    void stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = true;
        cv_empty_.notify_all();
        cv_full_.notify_all();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_empty_;
    std::condition_variable cv_full_;
    size_t max_size_;
    bool stopped_;
};

} // namespace gateway

#endif // GATEWAY_THREAD_SAFE_QUEUE_H
