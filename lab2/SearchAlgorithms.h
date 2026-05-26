#ifndef SEARCH_ALGORITHMS_H
#define SEARCH_ALGORITHMS_H

#include <vector>
#include <string>
#include <map>

#include "../lab1/soldier.h"

using namespace std;

/**
 * @brief Выполняет линейный поиск солдат по ФИО.
 *
 * Функция проходит по всему массиву soldiers
 * и выбирает всех солдат, у которых поле fullName
 * совпадает с переданным ключом key.
 *
 * @param soldiers Вектор солдат, по которому выполняется поиск.
 * @param key ФИО, которое нужно найти.
 * @return vector<Soldier> Вектор найденных солдат.
 */
vector<Soldier> linearSearchByFullName(const vector<Soldier>& soldiers, const string& key) 
{
    vector<Soldier> result;

    for (const Soldier& soldier : soldiers) {
        if (soldier.getFullName() == key) {
            result.push_back(soldier);
        }
    }

    return result;
}

/**
 * @brief Строит стандартную структуру multimap для поиска по ФИО.
 *
 * В multimap ключом является ФИО солдата,
 * а значением - объект Soldier.
 *
 * multimap используется потому, что одинаковое ФИО
 * может встречаться у нескольких объектов.
 *
 * @param soldiers Вектор солдат.
 * @return multimap<string, Soldier> Структура для поиска по ФИО.
 */
multimap<string, Soldier> buildMultiMap(const vector<Soldier>& soldiers) 
{
    multimap<string, Soldier> result;

    for (const Soldier& soldier : soldiers) {
        result.insert({soldier.getFullName(), soldier});
    }

    return result;
}

/**
 * @brief Выполняет поиск всех солдат с заданным ФИО в multimap.
 *
 * Для поиска используется метод equal_range,
 * который возвращает диапазон всех элементов с ключом key.
 *
 * @param mp Готовая структура multimap.
 * @param key ФИО, которое нужно найти.
 * @return vector<Soldier> Вектор найденных солдат.
 */
vector<Soldier> searchInMultiMap(const multimap<string, Soldier>& mp, const string& key) 
{
    vector<Soldier> result;

    auto range = mp.equal_range(key);

    for (auto it = range.first; it != range.second; ++it) {
        result.push_back(it->second);
    }

    return result;
}

#endif