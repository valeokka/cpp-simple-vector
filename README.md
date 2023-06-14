# cpp-simple-vector
# Яндекс Практикум: упрощенный вектор, финальный проект
Сильно упрощенный аналог `std::vector` со сходной структурой и функционалом.

В основе `SimpleVector` — массив в динамической памяти. В контейнере хранится адрес начала массива, информацию о его текущем размере и вместимости.
Поддерживает семантику перемещения.

Сконструированный по умолчанию вектор имеет нулевой размер и вместимость, а указатель на массив равен nullptr.

Как и `std::vector`, класс `SimpleVector` может изменять свой размер в сторону увеличения и уменьшения. Для этого служит метод Resize

Очистку `SimpleVector` выполняет метод `Clear`. Он не изменяет его вместимость и семантически эквивалентен вызову `Resize` с аргументом, равным нулю.

# Основные функции

## Конструкторы:
- `Конструктор по умолчанию:` создает пустой вектор с нулевой вместимостью, не выделяя динамическую память и не выбрасывая исключений.
- `Параметризованный конструктор:` создает вектор заданного размера, инициализируя элементы значением по умолчанию для типа Type. Вектор имеет одинаковый размер и вместимость. Если  размер нулевой, то динамическая память для его элементов не выделяется.
- `Конструктор из std::initializer_list:` создает вектор, содержащий копию элементов переданного initializer_list. Размер и вместимость вектора совпадают с размерами и вместимостью initializer_list.
## Деструктор:
- При разрушении освобождает память, занимаемую элементами.
## Основные методы:
- `GetSize():` возвращает количество элементов в векторе. Не выбрасывает исключений.
- `GetCapacity():` возвращает вместимость вектора. Не выбрасывает исключений.
- `IsEmpty():` сообщает, является ли вектор пустым. Не выбрасывает исключений.
- `Оператор []:` обеспечивает доступ к элементам вектора по индексу. Имеет две версии - константную и неконстантную. Не выбрасывает исключений. Для корректной работы оператора индекс элемента массива не должен выходить за пределы массива.
- `At:` обеспечивает доступ к элементам вектора по индексу, аналогично методу at класса vector. При выходе индекса за пределы массива выбрасывает исключение std::out_of_range.
- `Clear:` очищает вектор, не изменяя его вместимости. Не выбрасывает исключений.
- `Resize:` изменяет количество элементов в векторе с обеспечением строгой гарантии безопасности исключений.
- Методы `begin, end, cbegin и cend:` возвращают итераторы, указывающие на начало и конец вектора. Используют указатели в качестве итераторов. Эти методы объявлены со спецификатором noexcept.
- `Оператор присваивания:` обеспечивает строгую гарантию безопасности исключений.
- Метод `PushBack:` добавляет элемент в конец вектора с обеспечением строгой гарантии безопасности исключений.
- Метод `PopBack:` удаляет последний элемент вектора. Не выбрасывает исключений.
- Метод `Insert:` вставляет элемент в произвольное место вектора, обеспечивая базовую гарантию безопасности исключений.
- Метод `Erase:` удаляет элемент из произвольной позиции вектора, обеспечивая базовую гарантию безопасности исключений.
- Метод `swap:` обменивает содержимое вектора с другим вектором без выбрасывания исключений. Операция выполняется за время O(1).
- Операторы `== и !=:` сравнивают два вектора и возвращают true, если их размеры равны и содержат одинаковые элементы. Равенство вместимости не требуется для равенства векторов.
- Операторы `<, >, <=, >=:` выполняют лексикографическое сравнение содержимого двух векторов.
- `Reserve:` задает вектору емкость, предварительно выделяя необходимую память. Этот метод повышает эффективность кода, если заранее известно приблизительное количество элементов в векторе. При добавлении новых элементов копирование происходит реже или совсем не выполняется, благодаря выделению нужного объема памяти. Если новая емкость больше текущей вместимости, то память перевыделяется, а элементы вектора копируются в новый сегмент памяти.

# Сложность операций выполняемых над `SimpleVector:`
- Произвольный доступ - константа O(1)
- Вставка или удаление элементов в конце - амортизируемая константа O(1)
- Вставка или удаление элементов - линейно по расстоянию до конца вектора O(n)

# Сборка и установка
Сборка с помощью любой IDE либо сборка из командной строки

# Системные требования
Компилятор С++ с поддержкой стандарта C++17  и выше

