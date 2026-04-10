

#include <cassert>
#include <iostream>
#include "ring_buffer.hpp"

void test_empty_buffer() {
    RingBuffer<int, 5> rb;
    assert(rb.size() == 0);
    assert(rb.is_empty());
    assert(!rb.is_full());
    assert(!rb.pop().has_value());
    assert(!rb.peek().has_value());
    std::cout << "test_empty_buffer passed\n";
}

void test_push_pop_order() {
    RingBuffer<int, 5> rb;
    rb.push(1);
    rb.push(2);
    rb.push(3);
    assert(rb.size() == 3);
    assert(rb.pop().value() == 1);
    assert(rb.pop().value() == 2);
    assert(rb.pop().value() == 3);
    assert(rb.is_empty());
    std::cout << "test_push_pop_order passed\n";
}

void test_full_buffer() {
    RingBuffer<int, 3> rb;
    rb.push(1);
    rb.push(2);
    rb.push(3);
    assert(rb.is_full());
    assert(rb.size() == 3);
    std::cout << "test_full_buffer passed\n";
}

void test_overwrite() {
    RingBuffer<int, 3> rb;
    rb.push(1);
    rb.push(2);
    rb.push(3);
    rb.push(4);  // evicts 1
    rb.push(5);  // evicts 2
    assert(rb.size() == 3);
    assert(rb.pop().value() == 3);
    assert(rb.pop().value() == 4);
    assert(rb.pop().value() == 5);
    assert(rb.is_empty());
    std::cout << "test_overwrite passed\n";
}

void test_peek() {
    RingBuffer<int, 3> rb;
    rb.push(42);
    assert(rb.peek().value() == 42);
    assert(rb.size() == 1);  // peek must not consume
    rb.pop();
    assert(!rb.peek().has_value());
    std::cout << "test_peek passed\n";
}

void test_unique_ptr() {
    RingBuffer<std::unique_ptr<int>, 3> rb;
    rb.push(std::make_unique<int>(10));
    rb.push(std::make_unique<int>(20));
    assert(rb.peek().value() == 10);
    auto item = rb.pop();
    assert(*item.value() == 10);
    item = rb.pop();
    assert(*item.value() == 20);
    assert(rb.is_empty());
    std::cout << "test_unique_ptr passed\n";
}

void test_wrap_around() {
    RingBuffer<int, 3> rb;
    rb.push(1);
    rb.push(2);
    rb.push(3);
    rb.pop();
    rb.pop();
    rb.push(4);
    rb.push(5);
    assert(rb.pop().value() == 3);
    assert(rb.pop().value() == 4);
    assert(rb.pop().value() == 5);
    assert(rb.is_empty());
    std::cout << "test_wrap_around passed\n";
}

int main() {
    test_empty_buffer();
    test_push_pop_order();
    test_full_buffer();
    test_overwrite();
    test_peek();
    test_unique_ptr();
    test_wrap_around();
    std::cout << "all tests passed\n";
    return 0;
}
