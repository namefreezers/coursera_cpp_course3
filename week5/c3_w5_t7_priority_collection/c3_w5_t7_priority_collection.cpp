#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <vector>

using namespace std;

template<typename T>
class PriorityCollection {
public:
    using Id = size_t;
    using Priority = int;

    static const int DELETED = -1;

    // Добавить объект с нулевым приоритетом
    // с помощью перемещения и вернуть его идентификатор
    Id Add(T&& object) {
        objects.push_back(move(object));
        object_priorities.push_back(0);
        priorities[{0, object_priorities.size() - 1}] = prev(objects.end());

        return object_priorities.size() - 1;
    }

    // Добавить все элементы диапазона [range_begin, range_end)
    // с помощью перемещения, записав выданные им идентификаторы
    // в диапазон [ids_begin, ...)
    template<typename ObjInputIt, typename IdOutputIt>
    void Add(ObjInputIt range_begin, ObjInputIt range_end, IdOutputIt ids_begin) {
        transform(
                make_move_iterator(range_begin),
                make_move_iterator(range_end),
                ids_begin,
                [this](T&& to_add) {  // todo: test &
//                    return Add(move(to_add));
                    return Add(to_add);
                }
        );
    }


// Определить, принадлежит ли идентификатор какому-либо
// хранящемуся в контейнере объекту
    bool IsValid(Id id) const {
        return id < object_priorities.size() && object_priorities[id] != DELETED;
    }

// Получить объект по идентификатору
    const T &Get(Id id) const {
        return *priorities.at({object_priorities[id], id});
    }

// Увеличить приоритет объекта на 1
    void Promote(Id id) {
        // find
        auto it_old = priorities.find({object_priorities[id], id});
        // promote 1
        object_priorities[id]++;
        // add new
        priorities[{object_priorities[id], id}] = it_old->second;
        // delete old
        priorities.erase(it_old);
    }

// Получить объект с максимальным приоритетом и его приоритет
    pair<const T &, int> GetMax() const {
        auto it_pr_last = prev(priorities.end());
        return {*(it_pr_last->second), it_pr_last->/*map key*/first./*priority*/first};
    }

// Аналогично GetMax, но удаляет элемент из контейнера
    pair<T, int> PopMax() {
        auto it_pr_last = prev(priorities.end());

        typename list<T>::iterator& obj_it = it_pr_last->second;
        Priority priority = it_pr_last->first.first;
        Id id = it_pr_last->first.second;

        pair<T, int> res = {
                move(*obj_it),
                priority
        };

        objects.erase(obj_it);
        object_priorities[id] = DELETED;
        priorities.erase(it_pr_last);

        return res;
    }

private:
    list<T> objects;
    vector<Priority> object_priorities;
    map<pair<Priority, Id>, typename list<T>::iterator> priorities;
};


class StringNonCopyable : public string {
public:
    using string::string;  // Позволяет использовать конструкторы строки
    StringNonCopyable(const StringNonCopyable &) = delete;

    StringNonCopyable(StringNonCopyable &&) = default;

    StringNonCopyable &operator=(const StringNonCopyable &) = delete;

    StringNonCopyable &operator=(StringNonCopyable &&) = default;
};

void TestNoCopy() {
    PriorityCollection<StringNonCopyable> strings;
    const auto white_id = strings.Add("white");
    const auto yellow_id = strings.Add("yellow");
    const auto red_id = strings.Add("red");

    strings.Promote(yellow_id);
    for (int i = 0; i < 2; ++i) {
        strings.Promote(red_id);
    }
    strings.Promote(yellow_id);
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "red");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "yellow");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "white");
        ASSERT_EQUAL(item.second, 0);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestNoCopy);
    return 0;
}
