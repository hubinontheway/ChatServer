//
// Created by hubin on 25-7-23.
//

#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <iostream>

template<typename T>
class ThreadPool {
    std::vector<std::thread> threads;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> stop{};
    size_t maxTaskNum;
    std::queue<T> taskQueue;

public:
    ThreadPool(const size_t threadNum, const size_t maxTaskNum) {
        this->maxTaskNum = maxTaskNum;
        this->stop = false;
        this->threads.reserve(threadNum);
        for (size_t i = 0; i < threadNum; i++) {
            this->threads.emplace_back([this]() {
                try {
                    this->ThreadRun();
                } catch (const std::exception &e) {
                    std::cerr << "Caught exception: " << e.what() << std::endl;
                }
            });
        }
    }

    ~ThreadPool() {
        this->stop = true;
        this->cv.notify_all();
        for (auto &thread: this->threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void ThreadRun() {
        while (!this->stop) {
            T task; {
                std::unique_lock<std::mutex> lock(this->mutex);
                this->cv.wait(lock, [this]() {
                    return this->stop || !this->taskQueue.empty();
                });
                if (this->taskQueue.empty()) {
                    continue;
                }
                task = std::move(this->taskQueue.front());
                this->taskQueue.pop();
            }
            try {
                task();
            } catch (const std::exception &e) {
                std::cerr << "Caught exception: " << e.what() << std::endl;
            }
        }
    }

    bool Append(T task) {
        std::lock_guard<std::mutex> lock(this->mutex);
        if (this->taskQueue.size() >= maxTaskNum || this->stop) {
            return false;
        }
        this->taskQueue.emplace(std::move(task));

        // 唤醒一个正在 wait() 上等待的线程，使其重新获得锁并检查条件是否满足，进而继续执行。
        cv.notify_one();
        return true;
    }
};

#endif //THREAD_POOL_H
