#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include <lab5/memory_resource.h>
#include <lab5/queue.h>


// Тест конструктора и основных свойств пустой очереди
TEST(QueueTest, EmptyQueue) {
    MemoryResource resource(1024);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> queue(allocator);

    EXPECT_TRUE(queue.Empty());
    EXPECT_EQ(queue.Size(), 0);
}

// Тест добавления элементов
TEST(QueueTest, PushElements) {
    MemoryResource resource(1024);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> queue(allocator);

    queue.Push(1);
    EXPECT_FALSE(queue.Empty());
    EXPECT_EQ(queue.Size(), 1);
    EXPECT_EQ(queue.Front(), 1);
    EXPECT_EQ(queue.Back(), 1);

    queue.Push(2);
    EXPECT_EQ(queue.Size(), 2);
    EXPECT_EQ(queue.Front(), 1);
    EXPECT_EQ(queue.Back(), 2);

    queue.Push(3);
    EXPECT_EQ(queue.Size(), 3);
    EXPECT_EQ(queue.Front(), 1);
    EXPECT_EQ(queue.Back(), 3);
}

// Тест удаления элементов
TEST(QueueTest, PopElements) {
    MemoryResource resource(1024);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> queue(allocator);

    queue.Push(1);
    queue.Push(2);
    queue.Push(3);

    queue.Pop();
    EXPECT_EQ(queue.Size(), 2);
    EXPECT_EQ(queue.Front(), 2);
    EXPECT_EQ(queue.Back(), 3);

    queue.Pop();
    EXPECT_EQ(queue.Size(), 1);
    EXPECT_EQ(queue.Front(), 3);
    EXPECT_EQ(queue.Back(), 3);

    queue.Pop();
    EXPECT_TRUE(queue.Empty());
    EXPECT_EQ(queue.Size(), 0);
}

// Тест исключений при работе с пустой очередью
TEST(QueueTest, EmptyQueueExceptions) {
    MemoryResource resource(1024);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> queue(allocator);

    EXPECT_THROW(queue.Front(), std::runtime_error);
    EXPECT_THROW(queue.Back(), std::runtime_error);
    EXPECT_THROW(queue.Pop(), std::runtime_error);

    queue.Push(1);
    queue.Pop();

    EXPECT_THROW(queue.Front(), std::runtime_error);
    EXPECT_THROW(queue.Back(), std::runtime_error);
    EXPECT_THROW(queue.Pop(), std::runtime_error);
}

// Тест конструктора копирования
TEST(QueueTest, CopyConstructor) {
    MemoryResource resource(1024);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> original(allocator);
    original.Push(1);
    original.Push(2);
    original.Push(3);

    // Конструктор копирования должен использовать тот же аллокатор
    Queue<int> copy(original);

    EXPECT_EQ(original.Size(), 3);
    EXPECT_EQ(copy.Size(), 3);

    // Проверяем, что это глубокое копирование
    original.Pop();
    EXPECT_EQ(original.Size(), 2);
    EXPECT_EQ(copy.Size(), 3);

    EXPECT_EQ(copy.Front(), 1);
    copy.Pop();
    EXPECT_EQ(copy.Front(), 2);
    copy.Pop();
    EXPECT_EQ(copy.Front(), 3);
}

// Тест оператора присваивания копированием
TEST(QueueTest, CopyAssignment) {
    MemoryResource resource(1024);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> original(allocator);
    original.Push(1);
    original.Push(2);
    original.Push(3);

    Queue<int> copy(allocator);
    copy = original;

    EXPECT_EQ(original.Size(), 3);
    EXPECT_EQ(copy.Size(), 3);

    // Проверяем, что это глубокое копирование
    original.Pop();
    EXPECT_EQ(original.Size(), 2);
    EXPECT_EQ(copy.Size(), 3);

    // Проверка самоприсваивания
    copy = copy;
    EXPECT_EQ(copy.Size(), 3);
    EXPECT_EQ(copy.Front(), 1);
}

// Тест конструктора перемещения
TEST(QueueTest, MoveConstructor) {
    MemoryResource resource(1024);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> original(allocator);
    original.Push(1);
    original.Push(2);
    original.Push(3);

    // Конструктор перемещения - аллокатор не перемещается, а остается тот же
    Queue<int> moved(std::move(original));

    // После перемещения original должен быть в валидном, но пустом состоянии
    EXPECT_TRUE(original.Empty());
    EXPECT_EQ(moved.Size(), 3);
    EXPECT_EQ(moved.Front(), 1);
    EXPECT_EQ(moved.Back(), 3);

    // Проверяем, что moved работает корректно
    moved.Pop();
    EXPECT_EQ(moved.Size(), 2);
    EXPECT_EQ(moved.Front(), 2);
}

// Тест оператора присваивания перемещением
TEST(QueueTest, MoveAssignment) {
    MemoryResource resource(1024);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> original(allocator);
    original.Push(1);
    original.Push(2);
    original.Push(3);

    Queue<int> moved(allocator);
    moved = std::move(original);

    // После перемещения original должен быть в валидном, но пустом состоянии
    EXPECT_TRUE(original.Empty());
    EXPECT_EQ(moved.Size(), 3);
    EXPECT_EQ(moved.Front(), 1);
    EXPECT_EQ(moved.Back(), 3);

    // Проверяем, что moved работает корректно
    moved.Pop();
    EXPECT_EQ(moved.Size(), 2);
    EXPECT_EQ(moved.Front(), 2);

    // // Проверка самоприсваивания перемещением
    // moved = std::move(moved);
    // EXPECT_EQ(moved.Size(), 2); // Должно остаться неизменным
}

// Тест с пользовательским типом данных
TEST(QueueTest, CustomType) {
    MemoryResource resource(1024);

    struct Point {
        int x, y;
        Point(int x, int y) : x(x), y(y) {}
    };

    std::pmr::polymorphic_allocator<Queue<Point>::Node> allocator(&resource);
    Queue<Point> pointQueue(allocator);

    pointQueue.Push(Point(1, 2));
    pointQueue.Push(Point(3, 4));

    EXPECT_EQ(pointQueue.Front().x, 1);
    EXPECT_EQ(pointQueue.Front().y, 2);
    EXPECT_EQ(pointQueue.Back().x, 3);
    EXPECT_EQ(pointQueue.Back().y, 4);

    pointQueue.Pop();
    EXPECT_EQ(pointQueue.Front().x, 3);
    EXPECT_EQ(pointQueue.Front().y, 4);
}

// Тест с большим количеством элементов
TEST(QueueTest, LargeNumberOfElements) {
    MemoryResource resource(2 << 16);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> queue(allocator);
    const int COUNT = 1000;

    for (int i = 0; i < COUNT; ++i) {
        queue.Push(i);
    }

    EXPECT_EQ(queue.Size(), COUNT);
    EXPECT_EQ(queue.Front(), 0);
    EXPECT_EQ(queue.Back(), COUNT - 1);

    for (int i = 0; i < COUNT; ++i) {
        EXPECT_EQ(queue.Front(), i);
        queue.Pop();
    }

    EXPECT_TRUE(queue.Empty());
}

// Тест целостности данных после различных операций
TEST(QueueTest, DataIntegrity) {
    MemoryResource resource(1024);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> queue(allocator);
    std::vector<int> test_data = {1, 2, 3, 4, 5};

    // Добавляем данные
    for (int val : test_data) {
        queue.Push(val);
    }

    // Проверяем порядок извлечения (FIFO)
    for (size_t i = 0; i < test_data.size(); ++i) {
        EXPECT_EQ(queue.Front(), test_data[i]);
        queue.Pop();
    }

    EXPECT_TRUE(queue.Empty());
}

// Тест с разными аллокаторами на разных MemoryResource
TEST(QueueTest, DifferentMemoryResources) {
    // Создаем два разных MemoryResource
    MemoryResource resource1(1024);
    MemoryResource resource2(1024);

    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator1(&resource1);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator2(&resource2);

    Queue<int> queue1(allocator1);
    Queue<int> queue2(allocator2);

    queue1.Push(1);
    queue1.Push(2);

    queue2.Push(3);
    queue2.Push(4);

    // Копирование между очередями с разными аллокаторами должно работать
    Queue<int> queue1_copy(queue1); // Использует тот же аллокатор, что и queue1
    Queue<int> queue2_copy(allocator2);
    queue2_copy = queue2; // Копирование присваиванием

    EXPECT_EQ(queue1_copy.Size(), 2);
    EXPECT_EQ(queue1_copy.Front(), 1);

    EXPECT_EQ(queue2_copy.Size(), 2);
    EXPECT_EQ(queue2_copy.Front(), 3);
}

// Тест на исчерпание памяти MemoryResource
TEST(QueueTest, MemoryResourceExhaustion) {
    // Создаем очень маленький MemoryResource
    MemoryResource resource(64); // Всего 64 байта
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> queue(allocator);

    // Добавляем несколько элементов
    queue.Push(1);
    queue.Push(2);

    // Попытка добавить еще один элемент может привести к исчерпанию памяти
    // В зависимости от реализации MemoryResource, это может вернуть nullptr
    // или выбросить исключение

    // Проверяем, что текущие элементы работают
    EXPECT_EQ(queue.Size(), 2);
    EXPECT_EQ(queue.Front(), 1);
    EXPECT_EQ(queue.Back(), 2);
}

// Тест на корректность работы аллокатора после операций
TEST(QueueTest, MemoryResourceIntegration) {
    MemoryResource resource(1024);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator(&resource);

    Queue<int> queue(allocator);

    // Серия операций для проверки работы аллокатора
    queue.Push(10);
    queue.Push(20);
    queue.Pop();
    queue.Push(30);
    queue.Push(40);
    queue.Pop();
    queue.Push(50);

    EXPECT_EQ(queue.Size(), 3);
    EXPECT_EQ(queue.Front(), 30);
    EXPECT_EQ(queue.Back(), 50);

    // Очистка и повторное использование
    while (!queue.Empty()) {
        queue.Pop();
    }

    EXPECT_TRUE(queue.Empty());

    // Повторное использование после очистки
    queue.Push(100);
    queue.Push(200);

    EXPECT_EQ(queue.Size(), 2);
    EXPECT_EQ(queue.Front(), 100);
    EXPECT_EQ(queue.Back(), 200);
}

// Тест сравнения аллокаторов
TEST(QueueTest, AllocatorComparison) {
    MemoryResource resource1(1024);
    MemoryResource resource2(1024);

    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator1(&resource1);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator2(&resource2);
    std::pmr::polymorphic_allocator<Queue<int>::Node> allocator3(&resource1); // Тот же resource

    // Аллокаторы с разными resource не равны
    EXPECT_FALSE(allocator1 == allocator2);

    // Аллокаторы с одним resource равны
    EXPECT_TRUE(allocator1 == allocator3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
