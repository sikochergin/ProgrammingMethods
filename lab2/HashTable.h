#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <vector>
#include <string>
#include <iostream>

#include "../lab1/soldier.h"

using namespace std;

/**
 * @brief Хэш-таблица для поиска объектов Soldier по ФИО.
 *
 * Используется открытая адресация с линейным пробированием.
 *
 * Ключом является поле fullName.
 */
class HashTable {
private:
    /**
     * @brief Ячейка хэш-таблицы.
     *
     * Одна ячейка хранит один ключ fullName
     * и всех солдат с таким ФИО.
     */
    struct Cell {
        string key; // ФИО солдата
        vector<Soldier> values; // Все солдаты с таким ФИО
        bool occupied; // Занята ли ячейка

        /**
         * @brief Конструктор пустой ячейки.
         */
        Cell()
        {
            occupied = false;
        }
    };

    vector<Cell> table; // Основной массив хэш-таблицы
    int sizet; // Размер таблицы
    int collisionCount; // Количество коллизий

    /**
     * @brief Вычисляет хэш строки.
     *
     * @param key Строковый ключ.
     * @return int Индекс в таблице.
     */
    int hashFunction(const string& key) const
    {
        const int P = 31;
        unsigned long long hash = 0;
        unsigned long long power = 1;

        for (char symbol : key) {
            hash += static_cast<unsigned char>(symbol) * power;
            power *= P;
        }

        return hash % sizet;
    }

public:
    /**
     * @brief Конструктор хэш-таблицы.
     *
     * @param size Предполагаемое количество элементов.
     */
    HashTable(int size)
    {
        sizet = size * 2 + 1;
        collisionCount = 0;
        table.resize(sizet);
    }

    /**
     * @brief Добавляет одного солдата в хэш-таблицу.
     *
     * Если ФИО уже есть в таблице, солдат добавляется
     * в существующий список values.
     *
     * Если ячейка занята другим ФИО, используется
     * линейное пробирование.
     *
     * @param soldier Солдат, которого нужно добавить.
     */
    void insert(const Soldier& soldier)
    {
        string key = soldier.getFullName();

        int index = hashFunction(key);
        int startIndex = index;

        bool counted = false; // коллизию для одной вставки считаем не более одного раза

        while (table[index].occupied) {
            if (table[index].key == key) {
                table[index].values.push_back(soldier);

                if (counted){
                    collisionCount--;
                }

                return; // дубликат того же ключа — это не коллизия
            }

            // исходная ячейка занята ЧУЖИМ ключом — это коллизия,
            // но фиксируем её только один раз за вставку нового ключа,
            // а не на каждом шаге линейного пробирования
            if (!counted) {
                collisionCount++;
                counted = true;
            }

            index = (index + 1) % sizet;

            if (index == startIndex) {
                cout<<"Hash table is full"<<endl;
                return;
            }
        }

        table[index].key = key;
        table[index].values.push_back(soldier);
        table[index].occupied = true;
    }

    /**
     * @brief Строит хэш-таблицу по вектору солдат.
     *
     * @param soldiers Вектор солдат.
     */
    void build(const vector<Soldier>& soldiers)
    {
        for (const Soldier& soldier : soldiers) {
            insert(soldier);
        }
    }

    /**
     * @brief Ищет всех солдат с заданным ФИО.
     *
     * Поиск начинается с хэша ключа.
     * Если в ячейке лежит другой ключ, поиск идёт дальше
     * по правилу линейного пробирования.
     *
     * @param key ФИО, которое нужно найти.
     * @return vector<Soldier> Вектор найденных солдат.
     */
    vector<Soldier> search(const string& key) const
    {
        int index = hashFunction(key);
        int startIndex = index;

        while (table[index].occupied) {
            if (table[index].key == key) {
                return table[index].values;
            }

            index = (index + 1) % sizet;

            if (index == startIndex) {
                break;
            }
        }

        return vector<Soldier>();
    }

    /**
     * @brief Возвращает количество коллизий.
     *
     * @return int Количество коллизий при вставке элементов.
     */
    int getCollisionCount() const
    {
        return collisionCount;
    }
};

#endif