/**
 * @file data.h
 * @brief Функции для чтения данных о служащих из CSV-файлов.
 *
 * Файл содержит функцию, которая открывает CSV-файл, считывает данные
 * о служащих воинского полка и преобразует каждую строку файла
 * в объект класса Soldier.
 */

#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "soldier.h"

using namespace std;

/**
 * @brief Читает данные о служащих из CSV-файла.
 *
 * CSV-файл должен иметь следующий формат:
 * fullName,rank,companyNumber,age
 *
 * Первая строка считается заголовком и пропускается.
 * Каждая следующая строка преобразуется в объект Soldier.
 *
 * @param fileName Имя CSV-файла.
 * @return Вектор объектов Soldier.
 */
vector<Soldier> readSoldiersFromCSV(const string& fileName) {
    vector<Soldier> soldiers;

    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Ошибка открытия файла: " << fileName << endl;
        return soldiers;
    } //Вернем пустой  soldiers если файл не открывается

    string line;

    //Пропускаем первую строку с названиями столбцов, читаем просто заголовок
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line); //Создаем stringstream
        
        string fullName;
        string rank;
        string companyNumberString;
        string ageString;

        getline(ss, fullName, ',');
        getline(ss, rank, ',');
        getline(ss, companyNumberString, ',');
        getline(ss, ageString, ',');

        int companyNumber = stoi(companyNumberString);
        int age = stoi(ageString);

        Soldier soldier(fullName, rank, companyNumber, age); 
        soldiers.push_back(soldier); //добавляем новый объект Soldier в soldiers.
    }

    file.close();

    return soldiers;
}

#endif