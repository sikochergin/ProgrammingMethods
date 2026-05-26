/**
 * @file soldier.h
 * @brief Описание класса Soldier и операторов сравнения.
 *
 * Файл содержит класс Soldier, который используется для хранения
 * информации о служащем воинского полка. Также здесь перегружаются
 * операторы сравнения, необходимые для работы алгоритмов сортировки.
 */

#ifndef SOLDIER_H
#define SOLDIER_H

#include <string>

using namespace std;

/**
 * @brief Возвращает числовой приоритет воинского звания.
 *
 * Чем меньше число, тем ниже звание. Такая функция нужна для того,
 * чтобы сравнивать звания не по алфавиту, а по их старшинству.
 *
 * @param rank Воинское звание.
 * @return Числовой приоритет звания.
 */
int getRankPriority(const string& rank) {
    if (rank == "рядовой") {
        return 1;
    }
    if (rank == "сержант") {
        return 2;
    }
    if (rank == "прапорщик") {
        return 3;
    }
    if (rank == "лейтенант") {
        return 4;
    }
    if (rank == "капитан") {
        return 5;
    }
    if (rank == "майор") {
        return 6;
    }
    if (rank == "полковник") {
        return 7;
    }

    return 0;
}

/**
 * @brief Класс для хранения информации о служащем воинского полка.
 *
 * Объект класса содержит ФИО, звание, номер роты и возраст.
 * Сравнение служащих выполняется по полям:
 * звание, ФИО, номер роты.
 */
class Soldier {
private:
    string fullName;       ///< ФИО служащего.
    string rank;           ///< Воинское звание.
    int companyNumber;     ///< Номер роты.
    int age;               ///< Возраст.

public:
    /**
     * @brief Конструктор.
     *
     * @param fullName ФИО служащего.
     * @param rank Воинское звание.
     * @param companyNumber Номер роты.
     * @param age Возраст.
     */
    Soldier(string fullName, string rank, int companyNumber, int age) {
        this->fullName = fullName;
        this->rank = rank;
        this->companyNumber = companyNumber;
        this->age = age;
    }

    /**
     * @brief Возвращает ФИО служащего.
     * @return ФИО служащего.
     */
    string getFullName() const {
        return fullName;
    }

    /**
     * @brief Возвращает воинское звание.
     * @return Воинское звание.
     */
    string getRank() const {
        return rank;
    }

    /**
     * @brief Возвращает номер роты.
     * @return Номер роты.
     */
    int getCompanyNumber() const {
        return companyNumber;
    }

    /**
     * @brief Возвращает возраст.
     * @return Возраст.
     */
    int getAge() const {
        return age;
    }
};

/**
 * @brief Оператор "меньше" для сравнения двух служащих.
 *
 * Сначала сравнивается звание по старшинству. Если звания одинаковые,
 * сравнивается ФИО. Если ФИО также одинаковые, сравнивается номер роты.
 *
 * @param left Первый служащий.
 * @param right Второй служащий.
 * @return true, если left должен стоять раньше right.
 */
bool operator<(const Soldier& left, const Soldier& right) {
    int leftRank = getRankPriority(left.getRank());
    int rightRank = getRankPriority(right.getRank());

    if (leftRank != rightRank) {
        return leftRank < rightRank;
    }

    if (left.getFullName() != right.getFullName()) {
        return left.getFullName() < right.getFullName();
    }

    return left.getCompanyNumber() < right.getCompanyNumber();
}

/**
 * @brief Оператор "равно" для сравнения двух служащих.
 *
 * Два служащих считаются равными для сортировки, если у них совпадают
 * звание, ФИО и номер роты.
 *
 * @param left Первый служащий.
 * @param right Второй служащий.
 * @return true, если объекты равны по полям сравнения.
 */
bool operator==(const Soldier& left, const Soldier& right) {
    return getRankPriority(left.getRank()) == getRankPriority(right.getRank())
        && left.getFullName() == right.getFullName()
        && left.getCompanyNumber() == right.getCompanyNumber();
}

/**
 * @brief Оператор "больше".
 *
 * @param left Первый служащий.
 * @param right Второй служащий.
 * @return true, если left больше right.
 */
bool operator>(const Soldier& left, const Soldier& right) {
    return right < left;
}

/**
 * @brief Оператор "меньше или равно".
 *
 * @param left Первый служащий.
 * @param right Второй служащий.
 * @return true, если left меньше или равен right.
 */
bool operator<=(const Soldier& left, const Soldier& right) {
    return !(left > right);
}

/**
 * @brief Оператор "больше или равно".
 *
 * @param left Первый служащий.
 * @param right Второй служащий.
 * @return true, если left больше или равен right.
 */
bool operator>=(const Soldier& left, const Soldier& right) {
    return !(left < right);
}

/**
 * @brief Оператор "не равно".
 *
 * @param left Первый служащий.
 * @param right Второй служащий.
 * @return true, если объекты не равны.
 */
bool operator!=(const Soldier& left, const Soldier& right) {
    return !(left == right);
}

#endif