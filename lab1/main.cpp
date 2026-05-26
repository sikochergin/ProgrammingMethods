/**
 * @file main.cpp
 * @brief Главный файл.
 *
 * В файле реализованы три алгоритма сортировки:
 * сортировка выбором, сортировка простыми вставками и пирамидальная сортировка.
 * Также выполняется сравнение этих алгоритмов со стандартной функцией std::sort.
 *
 * Программа считывает CSV-файлы с данными о служащих воинского полка,
 * сортирует данные разными алгоритмами, измеряет время работы сортировок
 * и записывает результаты в файл sortingtime.csv.
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <algorithm>

#include "soldier.h"
#include "data.h"

using namespace std::chrono;

/**
 * @brief Сортировка выбором.
 *
 * Алгоритм последовательно ищет минимальный элемент
 * в неотсортированной части массива и ставит его в начало этой части.
 *
 * @tparam T Тип элементов массива.
 * @param arr Массив элементов.
 * @param size Размер массива.
 */
template <class T> void selectionSort(T arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        int minIndex = i;

        for (int j = i + 1; j < size; j++) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }

        if (minIndex != i) {
            swap(arr[i], arr[minIndex]);
        }
    }
}

/**
 * @brief Сортировка простыми вставками.
 *
 * Алгоритм делит массив на отсортированную и неотсортированную части.
 * Каждый следующий элемент из неотсортированной части вставляется
 * на подходящее место в отсортированной части.
 *
 * @tparam T Тип элементов массива.
 * @param arr Массив элементов.
 * @param size Размер массива.
 */
template <class T> void insertionSort(T arr[], int size) {
    for (int i = 1; i < size; i++) {
        T current = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > current) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = current;
    }
}

/**
 * @brief Восстанавливает свойство max-heap для поддерева.
 *
 * Функция рассматривает элемент с индексом root и его потомков.
 * Если один из потомков больше родителя, элементы меняются местами,
 * после чего функция рекурсивно вызывается для измененного поддерева.
 *
 * @tparam T Тип элементов массива.
 * @param arr Массив элементов.
 * @param size Размер рабочей части массива.
 * @param root Индекс корня поддерева.
 */
template <class T> void heapify(T arr[], int size, int root) {
    int largest = root;
    int left = 2 * root + 1;
    int right = 2 * root + 2;

    if (left < size && arr[left] > arr[largest]) {
        largest = left;
    }

    if (right < size && arr[right] > arr[largest]) {
        largest = right;
    }

    if (largest != root) {
        swap(arr[root], arr[largest]);
        heapify(arr, size, largest);
    }
}

/**
 * @brief Пирамидальная сортировка.
 *
 * Алгоритм сначала строит max-heap, а затем последовательно переносит
 * максимальный элемент из вершины кучи в конец массива.
 *
 * @tparam T Тип элементов массива.
 * @param arr Массив элементов.
 * @param size Размер массива.
 */
template <class T> void heapSort(T arr[], int size) {
    for (int i = size / 2 - 1; i >= 0; i--) {
        heapify(arr, size, i);
    }

    for (int i = size - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

/**
 * @brief Измеряет время выполнения переданной функции.
 *
 * Время измеряется в микросекундах.
 *
 * @tparam Func Тип функции.
 * @param func Функция, время выполнения которой нужно измерить.
 * @return Время выполнения в микросекундах.
 */
template <class Func> long long measureTime(Func func) {
    auto start = high_resolution_clock::now();

    func();

    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start).count();
}


/**
 * @brief Главная функция программы.
 *
 * Считывает все CSV-файлы, запускает сортировки,
 * измеряет время выполнения и сохраняет результаты в sortingtime.csv.
 *
 * @return Код завершения программы.
 */
int main() {
    vector<int> dataSizes = {100, 200, 500, 1000, 2000, 5000, 10000, 15000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};

    ofstream resultFile("sortingtime.csv");

    if (!resultFile.is_open()) {
        cout << "Ошибка создания файла sortingtime.csv" << endl;
        return 1;
    }

    resultFile << "size,algorithm,time_microseconds" << endl;

    for (int size : dataSizes) {
        string fileName = "mil_csv_folder/soldiers_" + to_string(size) + ".csv";

        cout << "Чтение файла: " << fileName << endl;

        vector<Soldier> sourceData = readSoldiersFromCSV(fileName);

        if (sourceData.empty()) {
            cout << "Файл пустой или не был прочитан: " << fileName << endl;
            continue;
        }

        cout << "Количество записей: " << sourceData.size() << endl;

        vector<Soldier> dataSelection = sourceData;
        vector<Soldier> dataInsertion = sourceData;
        vector<Soldier> dataHeap = sourceData;
        vector<Soldier> dataStd = sourceData;

        long long selectionTime = measureTime([&]() {
            selectionSort(dataSelection.data(), static_cast<int>(dataSelection.size()));
        });

        resultFile << size << ",selectionSort," << selectionTime << endl;

        cout << "selectionSort завершена" << endl;


        long long insertionTime = measureTime([&]() {
            insertionSort(dataInsertion.data(), static_cast<int>(dataInsertion.size()));
        });

        resultFile << size << ",insertionSort," << insertionTime << endl;

        cout << "insertionSort завершена" << endl;


        long long heapTime = measureTime([&]() {
            heapSort(dataHeap.data(), static_cast<int>(dataHeap.size()));
        });

        resultFile << size << ",heapSort," << heapTime << endl;

        cout << "heapSort завершена" << endl;


        long long stdSortTime = measureTime([&]() {
            sort(dataStd.begin(), dataStd.end());
        });

        resultFile << size << ",stdSort," << stdSortTime << endl;

        cout << "std::sort завершена" << endl;

        cout << "----------------------------------------" << endl;
    }

    resultFile.close();

    cout << "Работа программы завершена. Результаты записаны в sortingtime.csv" << endl;

    return 0;
}