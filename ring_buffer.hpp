
#pragma once

#include <array>
#include <atomic>
#include <memory>
#include <optional>


template <typename T>
struct inner_type { using type = T; };

template <typename T>
struct inner_type<std::unique_ptr<T>> { using type = T; };

template <typename T, size_t N>
class RingBuffer {
public:
    RingBuffer() = default;

    ~RingBuffer() = default;

    bool push(T&& item) {
        size_t t = tail_.load(std::memory_order_relaxed);
        bool evicted = is_full();

        if (is_full()) {
            head_.fetch_add(1, std::memory_order_relaxed);
        }

        buffer_[t % capacity()] = std::move(item);
        tail_.store(t + 1, std::memory_order_release);
        return evicted;
    }

    std::optional<T> pop() {
        if (is_empty()) {
            return std::nullopt;
        }
        size_t h = head_.load(std::memory_order_relaxed);
        head_.store(h + 1, std::memory_order_release);
        return std::move(buffer_[h % capacity()]);
    }

    std::optional<typename inner_type<T>::type> peek() const {
        if (is_empty()) {
            return std::nullopt;
        }
        size_t h = head_.load(std::memory_order_relaxed);
        if constexpr (std::is_same_v<T, typename inner_type<T>::type>) {
            return buffer_[h % capacity()];
        } else {
            return *buffer_[h % capacity()];
        }
    }

    size_t size() const noexcept{
        size_t h = head_.load(std::memory_order_acquire);
        size_t t = tail_.load(std::memory_order_acquire);
        return t - h;
    }

    size_t capacity() const noexcept {
        return N;
    }

    bool is_empty() const noexcept {
        return size() == 0;
    }

    bool is_full() const noexcept {
        return size() >= N;
    }

private:
    std::array<T, N> buffer_;
    std::atomic<size_t> head_{0};
    std::atomic<size_t> tail_{0};
};

void test_ring_buffer();