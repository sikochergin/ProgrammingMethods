#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <map>

#include "../lab1/data.h"

#include "SearchAlgorithms.h"
#include "BinarySearchTree.h"
#include "HashTable.h"
#include "RedBlackTree.h"

using namespace std;
using namespace chrono;

/**
 * @brief Измеряет время выполнения переданной функции в микросекундах.
 *
 * @param action Функция или лямбда-выражение, время выполнения которого нужно измерить.
 * @return long long Время выполнения в микросекундах.
 */
template <typename Function>
long long measureTime(Function action)
{
    auto start = high_resolution_clock::now();

    action();

    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start).count();
}

int main()
{
    vector<int> sizes = {100, 200, 500, 1000, 2000, 5000, 10000, 20000, 30000, 40000, 50000, 65000, 80000, 100000, 250000, 500000, 1000000};

    ofstream timingsFile("search_time.csv");
    ofstream collisionsFile("collision_count.csv");

    if (!timingsFile.is_open()) {
        cout << "Ошибка: не удалось открыть файл search_time.csv" << endl;
        return 1;
    }

    if (!collisionsFile.is_open()) {
        cout << "Ошибка: не удалось открыть файл collision_count.csv" << endl;
        return 1;
    }

    timingsFile << "size,algorithm,microseconds,found_count\n";
    collisionsFile << "size,collisions\n";

    for (int size : sizes) {
        string fileName = "mil_csv_folder/soldiers_" + to_string(size) + ".csv";

        cout << "Обработка файла: " << fileName << endl;

        vector<Soldier> soldiers = readSoldiersFromCSV(fileName);

        if (soldiers.empty()) {
            cout << "Файл пустой или не был прочитан: " << fileName << endl;
            continue;
        }

        string key = soldiers[soldiers.size() / 2].getFullName();

        cout << "Ключ поиска: " << key << endl;

        vector<Soldier> foundLinear;
        vector<Soldier> foundBST;
        vector<Soldier> foundRBTree;
        vector<Soldier> foundHashTable;
        vector<Soldier> foundMultiMap;

        long long linearTime = measureTime([&]() {
            foundLinear = linearSearchByFullName(soldiers, key);
        });

        timingsFile << size << ",linear_search," << linearTime << "," << foundLinear.size() << "\n";

        BinarySearchTree bst;
        bst.build(soldiers);

        long long bstTime = measureTime([&]() {
            foundBST = bst.search(key);
        });

        timingsFile << size << ",binary_search_tree," << bstTime << "," << foundBST.size() << "\n";

        RedBlackTree rbTree;
        rbTree.build(soldiers);

        long long rbTreeTime = measureTime([&]() {
            foundRBTree = rbTree.search(key);
        });

        timingsFile << size << ",red_black_tree," << rbTreeTime << "," << foundRBTree.size() << "\n";

        HashTable hashTable(size);
        hashTable.build(soldiers);

        long long hashTableTime = measureTime([&]() {
            foundHashTable = hashTable.search(key);
        });

        timingsFile << size << ",hash_table," << hashTableTime << "," << foundHashTable.size() << "\n";
        collisionsFile << size << "," << hashTable.getCollisionCount() << "\n";

        multimap<string, Soldier> soldierMultiMap = buildMultiMap(soldiers);

        long long multiMapTime = measureTime([&]() {
            foundMultiMap = searchInMultiMap(soldierMultiMap, key);
        });

        timingsFile << size << ",multimap," << multiMapTime << "," << foundMultiMap.size() << "\n";

        cout << "Найдено элементов: " << foundLinear.size() << endl;
        cout << "Готово для размера: " << size << endl;
        cout << "-----------------------------" << endl;
    }

    timingsFile.close();
    collisionsFile.close();

    cout << "Программа завершена" << endl;

    return 0;
}