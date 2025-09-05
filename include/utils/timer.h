//
// Created by hubin on 25-7-20.
//

#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <cstdio>
#include <iostream>
#include <vector>
#include <functional>
#include <unordered_map>

constexpr int TIME_OUT_MS = 1000;

using Clock = std::chrono::high_resolution_clock;
using TimeStamp = Clock::time_point;
using MS = std::chrono::milliseconds;
using TimeOutCallback = std::function<void()>;

struct TimerNode {
    int id;
    TimeStamp expires;
    TimeOutCallback cb;

    bool operator<(const TimerNode &t) const {
        return expires < t.expires;
    }
};

class HeapTimer {
public:
    HeapTimer() {
        heap.reserve(1024);
    }

    ~HeapTimer() = default;

    void Add(const int id, const int timeout, const TimeOutCallback &cb) {
        if (id <= 0 || timeout <= 0) {
            perror("HeapTimer->Add");
        }
        if (ref.count(id) == 0) {
            const size_t n = heap.size();
            ref[id] = n;
            heap.push_back({id, Clock::now() + MS(timeout), cb});
            siftUp(n);
        } else {
            const size_t idx = ref[id];
            heap[idx].cb = cb;
            Adjust(id, timeout);
        }
    }

    void DoWork(const int id) {
        if (id <= 0 || ref.count(id) <= 0 || heap.empty()) {
            perror("HeapTimer->DoWork");
        }
        heap[ref[id]].cb();
        del(ref[id]);
    }

    void Clear() {
        ref.clear();
        heap.clear();
    }

    void Tick() {
        if (heap.empty()) {
            return;
        }
        while (!heap.empty()) {
            TimerNode node = heap.front();
            if (std::chrono::duration_cast<MS>(node.expires - Clock::now()).count() > 0) {
                break;
            }
            printf("超时关闭连接:%d\n", node.id);
            node.cb();
            del(0);
        }
    }

    size_t GetNextTick() {
        Tick();
        size_t res = 10 * TIME_OUT_MS;
        if (!heap.empty()) {
            res = std::chrono::duration_cast<MS>(heap.front().expires - Clock::now()).count();
        }
        return res;
    }

    void Adjust(const int id, const int newExpires) {
        if (heap.empty() || ref.count(id) <= 0) {
            perror("HeapTimer->Adjust");
        }
        heap[ref[id]].expires = Clock::now() + MS(newExpires);
        if (!siftDown(ref[id], heap.size())) {
            siftUp(ref[id]);
        }
    }

private:
    void del(const size_t idx) {
        if (heap.empty() || idx >= heap.size()) {
            perror("HeapTimer->del");
        }
        const size_t n = heap.size() - 1;
        if (idx < n) {
            swapNode(idx, n);
            if (!siftDown(idx, n)) {
                siftUp(idx);
            }
        }
        ref.erase(heap[n].id);
        heap.pop_back();
    }

    void siftUp(size_t j) {
        while (true) {
            const size_t i = (j - 1) / 2;
            // 一定要加上i == j看是否是到了堆顶了没有
            if (i == j || heap[i] < heap[j]) {
                break;
            }
            swapNode(i, j);
            j = i;
        }
    }

    bool siftDown(const size_t i, const size_t n) {
        size_t parent = i;
        while (true) {
            const size_t leftChild = 2 * parent + 1;
            const size_t rightChild = leftChild + 1;
            size_t minChild = leftChild;
            if (leftChild >= n) {
                break;
            }
            // 如果没有rightChild则minChild就直接是leftChild
            if (rightChild < n && heap[rightChild] < heap[leftChild]) {
                minChild = rightChild;
            }
            if (heap[parent] < heap[minChild]) {
                break;
            }
            swapNode(parent, minChild);
            parent = minChild;
        }
        return parent > i;
    }

    void swapNode(const size_t i, const size_t j) {
        if (i >= heap.size() || j >= heap.size()) {
            std::cout << ("HeapTimer->swapNode") << std::endl;
        }
        std::swap(heap[i], heap[j]);
        ref[heap[i].id] = i;
        ref[heap[j].id] = j;
    }

    std::vector<TimerNode> heap;
    std::unordered_map<int, size_t> ref;
};

#endif //TIMER_H
