#pragma once
#include "array_ptr.h"
#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <utility>

class ReserveProxyObj
{
public:
    ReserveProxyObj(size_t capacity_to_reserve)
        : capacity_(capacity_to_reserve) {}

    size_t GetSize()
    {
        return capacity_;
    }

private:
    size_t capacity_ = 0;
};

ReserveProxyObj Reserve(const size_t capacity_to_reserve)
{
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector
{
public:
    using Iterator = Type *;
    using ConstIterator = const Type *;

    SimpleVector() noexcept = default;

    SimpleVector(const SimpleVector &other)
    {
        if (!other.IsEmpty())
        {
            SimpleVector temp(other.capacity_);
            std::copy(other.begin(), other.end(), temp.begin());
            temp.size_ = other.size_;
            temp.capacity_ = other.capacity_;
            swap(temp);
        }
    }

        SimpleVector(SimpleVector&& other)
    {
        if (!other.IsEmpty())
        {
            SimpleVector temp(std::move(other.capacity_));
            std::move(other.begin(), other.end(), temp.begin());
            temp.size_ = std::exchange(other.size_, 0);
            temp.capacity_ = std::exchange(other.size_, 0);
            swap(temp);
        }
    }

    explicit SimpleVector(size_t size)
        : items_(size),
          size_(size),
          capacity_(size)
    {
        std::generate(items_.Get(), items_.Get() + size, [](){return Type();});
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type &value)
        : items_(size),
          size_(size),
          capacity_(size)
    {
        std::fill(items_.Get(), items_.Get() + size, value);
    }

    SimpleVector(ReserveProxyObj obj)
    {
        Reserve(obj.GetSize());
    }
    // Создаёт вектор из std::initializer_list
    SimpleVector(const std::initializer_list<Type> init)
        : items_(init.size()),
          size_(init.size()),
          capacity_(init.size())
    {
        std::copy(init.begin(), init.end(), items_.Get());
    }

    SimpleVector(std::initializer_list<Type>&& init)
        : items_(init.size()),
          size_(init.size()),
          capacity_(init.size())
    {
        std::move(init.begin(), init.end(), items_.Get());
    }

    SimpleVector& operator=(const SimpleVector& rhs)
    {
        if (rhs != *this)
        {
            delete[] items_.Release();
            SimpleVector temp(rhs);
            swap(temp);
        }
        return *this;
    }

        SimpleVector& operator=(SimpleVector&& rhs)
    {
        if (rhs != *this)
        {
            delete[] items_.Release();
            SimpleVector temp(std::move(rhs));
            swap(temp);
        }
        return *this;
    }
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора

    void PushBack(const Type &item)
    {
        if (size_ < capacity_)
        {
            items_[size_] = item;
            size_ += 1;
        }
        else
        {
            ArrayPtr<Type> temp(capacity_ + 1);
            std::copy(items_.Get(), items_.Get() + size_, temp.Get());
            items_.swap(temp);
            capacity_ += 1;
            items_[size_] = item;
            size_ += 1;
        }
    }

        void PushBack(Type&& item)
    {
        if (size_ < capacity_)
        {
            items_[size_] = std::move(item);
            size_ += 1;
        }
        else
        {
            ArrayPtr<Type> temp(capacity_ + 1);
            std::move(items_.Get(), items_.Get() + size_, temp.Get());
            items_.swap(temp);
            capacity_ += 1;
            items_[size_] = std::move(item);
            size_ += 1;
        }
    }


    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value){
        assert(pos >= cbegin() && pos <= cend());
        int position = std::distance(cbegin(), pos);
        if (size_ < capacity_)
        {
            std::copy_backward((Iterator)pos, end(), end() + 1);
            items_[position] = value;
        }
        else
        {
            SimpleVector temp(std::max(size_t(1), capacity_ * 2));
            std::copy(begin(), (Iterator)pos, temp.begin());
            std::copy_backward((Iterator)pos, end(), temp.end());
            temp[position] = value;
            items_.swap(temp.items_);
            capacity_ = std::max(size_t(1), capacity_ * 2);
        }   
        ++size_;
        return Iterator(&items_[position]);
    }

    Iterator Insert(ConstIterator pos, Type &&value)
    {
        assert(pos >= cbegin() && pos <= cend());
        int position = std::distance(cbegin(), pos);
        if (size_ < capacity_)
        {
            std::move_backward((Iterator)pos,end(), end() + 1);
            items_[position] = std::move(value);
        }
        else
        {
            SimpleVector temp(std::max(size_t(1), capacity_ * 2));
            std::move(begin(), (Iterator)pos, temp.begin());
            std::move_backward((Iterator)pos, end(), temp.end() );
            temp[position] = std::move(value);
            items_.swap(temp.items_);
            capacity_ = std::max(size_t(1), capacity_ * 2);
        }
        ++size_;
        return Iterator(&items_[position]);
    }
    
    void Reserve(size_t new_capacity)
    {
        if (new_capacity > capacity_)
        {
            if (IsEmpty())
            {
                ArrayPtr<Type> temp(new_capacity);
                items_.swap(temp);
                capacity_ = new_capacity;
            }
            else
            {
                SimpleVector temp(new_capacity);
                std::move(begin(), end(), temp.begin());
                temp.size_ = size_;
                swap(temp);
                capacity_ = new_capacity;
            }
        }
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept
    {
        if (size_ != 0)
        {
            --size_;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(const ConstIterator &pos)
    {
        assert(pos >= cbegin() && pos <= cend());
        std::copy((Iterator)pos + 1, end(), (Iterator)pos);
        --size_;
        return (Iterator)pos;
    }

    Iterator Erase(ConstIterator &&pos)
    {
        assert(pos >= cbegin() && pos <= cend());
        std::move((Iterator)pos + 1, end(), (Iterator)pos);
        --size_;
        return (Iterator)pos;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector &other) noexcept
    {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept { return size_; }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept { return capacity_; }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept
    {
        return size_ == 0;
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept
    {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size)
    {
        if (new_size > size_)
        {
            capacity_ = std::max(capacity_ * 2, new_size);
            ArrayPtr<Type> temp(capacity_);
            std::generate(temp.Get(), temp.Get() + new_size, [](){return Type();});
            std::move(items_.Get(), items_.Get() + size_, temp.Get());
            items_.swap(temp);
            size_ = new_size;
        }

        else
        {
            size_ = new_size;
        }
    }

    // Возвращает ссылку на элемент с индексом index
    Type &operator[](size_t index) noexcept
    {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type &operator[](size_t index) const noexcept
    {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type &At(size_t index)
    {
        if (index >= size_)
        {
            throw std::out_of_range("index > size");
        }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type &At(size_t index) const
    {
        if (index >= size_)
        {
            throw std::out_of_range("index > size");
        }
        return items_[index];
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept { return items_.Get(); }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept { return items_.Get() + size_; }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept { return items_.Get(); }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept { return items_.Get() + size_; }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept { return items_.Get(); }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept { return items_.Get() + size_; }

private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{

    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator>(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{
    return rhs < lhs;
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{
    return lhs < rhs || lhs == rhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{
    // Заглушка. Напишите тело самостоятельно
    return lhs > rhs || lhs == rhs;
}   