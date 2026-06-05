#ifndef MIDDLE_SQUARE_WEYL_H
#define MIDDLE_SQUARE_WEYL_H

#include <cstdint>
#include <string>

/**
 * @brief Модифицированный генератор на основе метода серединных квадратов.
 *
 * Базовая идея метода серединных квадратов: state = state * state
 *
 * После возведения в квадрат из результата берется средняя часть битов.
 *
 * Модификация заключается в добавлении последовательности Вейля:  weyl = weyl + WEYL_CONSTANT; state = state * state + weyl;
 *
 * Это помогает уменьшить вероятность быстрого вырождения генератора
 * и появления коротких циклов.
 */
class MiddleSquareWeyl {
private:
    /**
     * @brief Внутреннее состояние генератора.
     */
    uint32_t state;

    /**
     * @brief Дополнительное состояние Weyl-последовательности.
     *
     * На каждом шаге это значение увеличивается на фиксированную константу.
     */
    uint64_t weyl;

    /**
     * @brief Константа для Weyl-последовательности.
     *
     * Константа нечетная, что важно для хорошего перебора значений
     * при работе по модулю 2^64.
     */
    static const uint64_t WEYL_CONSTANT = 0xB5AD4ECEDA1CE2A9ULL;

public:
    /**
     * @brief Создает генератор с заданным начальным значением.
     *
     * @param seed начальное значение генератора.
     */
    MiddleSquareWeyl(uint32_t seed) {
        if (seed == 0) {
            state = 1u;
        } else {
            state = seed;
        }

        weyl = static_cast<uint64_t>(state) ^ WEYL_CONSTANT;
    }

    /**
     * @brief Генерирует следующее псевдослучайное число.
     *
     * Сначала состояние возводится в квадрат.
     * Затем добавляется Weyl-последовательность.
     * После этого из 64-битного результата берутся средние 32 бита.
     *
     * @return uint32_t следующее псевдослучайное число.
     */
    uint32_t next() {
        uint64_t square = static_cast<uint64_t>(state) * state;
        weyl += WEYL_CONSTANT;
        square += weyl;

        state = static_cast<uint32_t>(square >> 16);
        return state;
    }

    /**
     * @brief Возвращает название генератора.
     *
     * Название будет использоваться при записи результатов в CSV.
     */
    std::string name() const {
        return "MiddleSquareWeyl";
    }
};

#endif