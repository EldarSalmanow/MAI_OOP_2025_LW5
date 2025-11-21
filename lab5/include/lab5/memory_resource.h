#ifndef MAI_OOP_2025_MEMORY_RESOURCE_H
#define MAI_OOP_2025_MEMORY_RESOURCE_H

#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>


class MemoryResource : public std::pmr::memory_resource {
private:

    struct Block {
        std::uint64_t offset;

        std::uint64_t size;
    };

public:

    explicit MemoryResource(std::uint64_t size) {
        assert(size > 0);

        _buffer = new char[size];
        _size = size;
    }

    ~MemoryResource() override {
        delete[] _buffer;
    }

private:

    void *do_allocate(std::size_t bytes,
                      std::size_t alignment) override {
        (void) alignment;

        for (uint64_t i = 0; i < _used.size() + 1; ++i) {
            std::uint64_t begin = 0;
            std::uint64_t end = _size;

            if (_used.empty()) {

            } else if (i == 0) {
                end = _used[0].offset;
            } else if (i == _used.size()) {
                begin = _used[_used.size() - 1].offset + _used[_used.size() - 1].size;
            } else {
                begin = _used[i - 1].offset + _used[i - 1].size;
                end = _used[i].offset;
            }

            if (end - begin >= bytes) {
                _used.emplace_back(begin, bytes);

                std::ranges::sort(_used, [] (const Block &first, const Block &second) {
                    return first.offset < second.offset;
                });

                return _buffer + begin;
            }
        }

        return nullptr;
    }

    void do_deallocate(void *pointer,
                       std::size_t bytes,
                       std::size_t alignment) override {
        (void) alignment;

        std::uint64_t offset = static_cast<char *>(pointer) - _buffer;

        auto block = std::ranges::find_if(_used, [offset, bytes] (const Block &block) {
            return block.offset == offset
                && block.size == bytes;
        });

        if (block == _used.end()) {
            return;
        }

        _used.erase(block);
    }

    bool do_is_equal(const memory_resource &other) const noexcept override {
        return this == &other;
    }

private:

    char *_buffer;
    uint64_t _size;

    std::vector<Block> _used;
};

#endif //MAI_OOP_2025_MEMORY_RESOURCE_H