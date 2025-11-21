#ifndef MAI_OOP_2025_QUEUE_H
#define MAI_OOP_2025_QUEUE_H

#include <memory>


template<typename T>
class Queue {
public:

    struct Node {
        T data;

        Node *prev, *next;
    };

public:

    using value_type = T;

    using pointer = T *;

    using const_pointer = const T *;

    using reference = T &;

    using const_reference = const T &;

    using size_type = std::uint64_t;

    using difference_type = std::ptrdiff_t;

    using allocator_type = std::pmr::polymorphic_allocator<Node>;

    using allocator_traits = std::allocator_traits<allocator_type>;

public:

    Queue(const allocator_type &allocator)
            : _head(nullptr),
              _tail(nullptr),
              _size(0),
              _allocator(allocator) {}

    Queue(const Queue<T> &queue)
            : _head(nullptr),
              _tail(nullptr),
              _size(0),
              _allocator(queue._allocator) {
        Copy(queue);
    }

    Queue(Queue<T> &&queue) noexcept
            : _head(nullptr),
              _tail(nullptr),
              _size(0),
              _allocator(queue._allocator) {
        std::swap(_head, queue._head);
        std::swap(_tail, queue._tail);
        std::swap(_size, queue._size);
    }

public:

    ~Queue() {
        Clear();
    }

public:

    auto Front() -> reference {
        if (Empty()) {
            throw std::runtime_error("[ERROR] Queue is empty!");
        }

        return _head->data;
    }

    auto Front() const -> const_reference {
        if (Empty()) {
            throw std::runtime_error("[ERROR] Queue is empty!");
        }

        return _head->data;
    }

    auto Back() -> reference {
        if (Empty()) {
            throw std::runtime_error("[ERROR] Queue is empty!");
        }

        return _tail->data;
    }

    auto Back() const -> const_reference {
        if (Empty()) {
            throw std::runtime_error("[ERROR] Queue is empty!");
        }

        return _tail->data;
    }

    auto Size() const -> size_type {
        return _size;
    }

    auto Empty() const -> bool {
        return Size() == 0;
    }

    auto Push(const_reference data) -> void {
        Node *node = CreateNode(data, nullptr, _tail);

        if (Empty()) {
            _head = node;
        } else {
            _tail->prev = node;
        }

        _tail = node;

        ++_size;
    }

    auto Pop() -> void {
        if (Empty()) {
            throw std::runtime_error("[ERROR] Queue is empty!");
        }

        Node *node = _head;

        if (Size() == 1) {
            _tail = nullptr;
        } else {
            _head->prev->next = nullptr;
        }

        _head = _head->prev;

        --_size;

        DestroyNode(node);
    }

public:

    auto operator=(const Queue<T> &queue) -> Queue<T> & {
        if (this == &queue) {
            return *this;
        }

        Clear();
        Copy(queue);

        return *this;
    }

    auto operator=(Queue<T> &&queue) noexcept -> Queue<T> & {
        if (this == &queue) {
            return *this;
        }

        Clear();

        std::swap(_head, queue._head);
        std::swap(_tail, queue._tail);
        std::swap(_size, queue._size);

        return *this;
    }

private:

    Node *CreateNode(const_reference data,
                     Node *prev,
                     Node *next) {
        Node *node = allocator_traits::allocate(_allocator, 1);

        allocator_traits::construct(_allocator, node, data, prev, next);

        return node;
    }

    void DestroyNode(Node *node) {
        allocator_traits::destroy(_allocator, node);

        allocator_traits::deallocate(_allocator, node, 1);
    }

    void Clear() {
        for (Node *node = _head; node != nullptr;) {
            Node *prev = node->prev;

            DestroyNode(node);

            node = prev;
        }

        _head = nullptr;
        _tail = nullptr;
        _size = 0;
    }

    void Copy(const Queue<T> &queue) {
        for (Node *node = queue._head; node != nullptr; node = node->prev) {
            Push(node->data);
        }
    }

private:

    Node *_head, *_tail;

    size_type _size;

    allocator_type _allocator;
};

#endif //MAI_OOP_2025_QUEUE_H