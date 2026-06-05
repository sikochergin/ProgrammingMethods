/**
 * @file main.cpp
 * @brief ЛР2 «Алгоритмы поиска данных», вариант 8 (служащие воинского полка).
 *
 * Ключ поиска — первое нечисловое поле (ФИО, fullName). Ключи не уникальны:
 * поиск находит все вхождения. Сравниваются пять способов: линейный поиск,
 * бинарное дерево поиска (BST), красно-чёрное дерево (RBT), хэш-таблица и
 * std::multimap. Замеряется время поиска (пункты 3, 4) и число коллизий
 * хэш-функции (пункт 2).
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <random>

#include "../lab1/data.h"

#include "SearchAlgorithms.h"
#include "BinarySearchTree.h"
#include "HashTable.h"
#include "RedBlackTree.h"

using namespace std;
using namespace chrono;

/**
 * @brief Минимальное время одного поиска по набору ключей (в наносекундах).
 *
 * Прогоняет функцию поиска по всем ключам keys, повторяет весь замер
 * measureRepeats раз и возвращает минимальное время. Минимум устойчив к
 * помехам ОС: посторонняя нагрузка может только замедлить отдельный прогон,
 * поэтому самый быстрый прогон ближе всего к «чистому» времени алгоритма.
 * Результат каждого поиска аккумулируется в sink, чтобы компилятор при
 * оптимизации не выбросил вызовы как «мёртвый код».
 *
 * @tparam SearchFunc тип функции/лямбды поиска: (const string&) -> vector<Soldier>
 * @param search функция поиска (ключ -> вектор найденных солдат)
 * @param keys набор ключей, по которым выполняется поиск
 * @param measureRepeats число повторов всего замера
 * @return минимальное среднее время одного поиска, нс
 */
template <typename SearchFunc>
double measureSearch(SearchFunc search, const vector<string>& keys, int measureRepeats = 5)
{
    double best = 1e18;

    for (int r = 0; r < measureRepeats; ++r) {
        long long sink = 0;

        auto start = high_resolution_clock::now();
        for (const string& key : keys) {
            sink += search(key).size();
        }
        auto end = high_resolution_clock::now();

        volatile long long keep = sink;
        (void)keep;

        double ns = duration<double, nano>(end - start).count() / keys.size();
        if (ns < best) {
            best = ns;
        }
    }

    return best;
}

/**
 * @brief Формирует набор ключей для поиска.
 *
 * Большая часть ключей — случайные ФИО, реально присутствующие в данных,
 * каждый десятый ключ — заведомо отсутствующий (проверка сценария «промах»,
 * который для линейного поиска является худшим случаем).
 *
 * @param soldiers массив, из которого берутся существующие ФИО
 * @param count сколько ключей сформировать
 * @param rng генератор случайных чисел
 * @return вектор ключей для поиска
 */
vector<string> makeQueryKeys(const vector<Soldier>& soldiers, int count, mt19937& rng)
{
    vector<string> keys;
    uniform_int_distribution<size_t> pick(0, soldiers.size() - 1);

    for (int i = 0; i < count; ++i) {
        if (i % 10 == 0) {
            keys.push_back("НЕСУЩЕСТВУЮЩЕЕ ФИО");
        }
        else {
            keys.push_back(soldiers[pick(rng)].getFullName());
        }
    }

    return keys;
}

/**
 * @brief Замер времени поиска всеми способами (пункты 3 и 4 ТЗ).
 *
 * Для каждого размера выборки и каждого из 5 файлов строит все структуры
 * (вне замера времени), затем замеряет время поиска каждым способом и
 * записывает результат в search_time.csv. multimap включён в общее сравнение.
 *
 * @param sizes список размеров выборок
 */
void runTimeBenchmark(const vector<int>& sizes)
{
    ofstream out("search_time.csv");
    if (!out.is_open()) {
        cout << "Ошибка: не удалось открыть search_time.csv" << endl;
        return;
    }
    out << "run,size,algorithm,nanoseconds\n";

    mt19937 rng(42);
    const int QUERIES = 50; // не менее 10 поисков на каждой выборке

    for (int size : sizes) {
        for (int run = 0; run < 5; ++run) {
            string fileName = "mil_csv_folder/soldiers_" + to_string(size) + "_" + to_string(run) + ".csv";
            vector<Soldier> soldiers = readSoldiersFromCSV(fileName);

            if (soldiers.empty()) {
                cout << "Пропущен файл: " << fileName << endl;
                continue;
            }

            // Размер хэш-таблицы с запасом относительно числа записей —
            // для быстрого поиска (низкий коэффициент загрузки).
            HashTable hash(size);

            // Все структуры строим вне замера времени поиска.
            BinarySearchTree bst;
            bst.build(soldiers);

            RedBlackTree rbt;
            rbt.build(soldiers);

            hash.build(soldiers);

            multimap<string, Soldier> mm = buildMultiMap(soldiers);

            vector<string> keys = makeQueryKeys(soldiers, QUERIES, rng);

            double tLinear = measureSearch(
                [&](const string& k) { return linearSearchByFullName(soldiers, k); }, keys);
            double tBst = measureSearch(
                [&](const string& k) { return bst.search(k); }, keys);
            double tRbt = measureSearch(
                [&](const string& k) { return rbt.search(k); }, keys);
            double tHash = measureSearch(
                [&](const string& k) { return hash.search(k); }, keys);
            double tMm = measureSearch(
                [&](const string& k) { return searchInMultiMap(mm, k); }, keys);

            out << run << "," << size << ",linear_search,"     << tLinear << "\n";
            out << run << "," << size << ",binary_search_tree," << tBst    << "\n";
            out << run << "," << size << ",red_black_tree,"     << tRbt    << "\n";
            out << run << "," << size << ",hash_table,"         << tHash   << "\n";
            out << run << "," << size << ",multimap,"           << tMm     << "\n";

            cout << "Время: size=" << size << " run=" << run << " готово" << endl;
        }
    }

    out.close();
    cout << "Время поиска записано в search_time.csv" << endl;
}

/**
 * @brief Исследование коллизий хэш-функции (пункт 2 ТЗ).
 *
 * В отличие от замера времени, здесь хэш-таблица берётся ФИКСИРОВАННОГО
 * размера. Тогда с ростом выборки растёт коэффициент загрузки (отношение
 * числа ключей к числу ячеек), и число коллизий увеличивается, выходя на
 * насыщение — график получается содержательным. Размер таблицы выбран
 * заведомо больше числа возможных уникальных ФИО (~11000), чтобы при
 * открытой адресации таблица не переполнилась и данные не потерялись.
 *
 * @param sizes список размеров выборок
 */
void runCollisionStudy(const vector<int>& sizes)
{
    ofstream out("collisions.csv");
    if (!out.is_open()) {
        cout << "Ошибка: не удалось открыть collisions.csv" << endl;
        return;
    }
    out << "run,size,collisions\n";

    const int FIXED_TABLE_SIZE = 20011; // простое число, с запасом над числом уникальных ФИО

    for (int size : sizes) {
        for (int run = 0; run < 5; ++run) {
            string fileName = "mil_csv_folder/soldiers_" + to_string(size) + "_" + to_string(run) + ".csv";
            vector<Soldier> soldiers = readSoldiersFromCSV(fileName);

            if (soldiers.empty()) {
                continue;
            }

            HashTable hash(FIXED_TABLE_SIZE);
            hash.build(soldiers);

            out << run << "," << size << "," << hash.getCollisionCount() << "\n";
        }
        cout << "Коллизии: size=" << size << " готово" << endl;
    }

    out.close();
    cout << "Коллизии записаны в collisions.csv" << endl;
}

/**
 * @brief Точка входа: запускает замер времени и исследование коллизий.
 */
int main()
{
    vector<int> sizes = {
        100, 200, 500, 1000, 2000, 5000, 10000, 20000, 30000,
        40000, 50000, 65000, 80000, 100000, 250000, 500000, 1000000
    };

    runTimeBenchmark(sizes);
    runCollisionStudy(sizes);

    cout << "Программа завершена" << endl;
    return 0;
}
