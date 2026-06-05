#ifndef ROTATE_SHIFT_MIX_H
#define ROTATE_SHIFT_MIX_H

#include <cstdint>
#include <string>

/**
 * @brief Модифицированный генератор на основе метода перемешивания.
 *
 * Базовая идея метода перемешивания:
 * текущее состояние циклически сдвигается влево и вправо,
 * после чего полученные значения складываются.
 *
 * Так как состояние имеет тип uint32_t, длина ячейки равна 32 битам.
 * В качестве сдвига берется 1/4 длины ячейки
 * 
 * Модификация заключается в добавлении изменяющейся переменной add
 * и дополнительного перемешивания через XOR и циклические сдвиги.
 */
class RotateShiftMix {
private:
    /**
     * @brief Внутреннее состояние генератора.
     */
    uint32_t state;

    /**
     * @brief Дополнительное изменяющееся значение.
     *
     * Оно нужно, чтобы генератор не зависел только от циклических сдвигов state.
     */
    uint32_t add;

    /**
     * @brief Шаг изменения дополнительной переменной add.
     *
     * Константа нечетная, поэтому при работе с uint32_t она хорошо проходит
     * по разным значениям по модулю 2^32.
     */
    static const uint32_t STEP = 0x9E3779B9u;

    /**
     * @brief Циклический сдвиг влево для 32-битного числа.
     *
     * @param x число, которое нужно сдвинуть.
     * @param shift количество битов сдвига.
     * @return uint32_t результат циклического сдвига влево.
     */
    static uint32_t rotateLeft(uint32_t x, unsigned int shift) {
        return (x << shift) | (x >> (32 - shift));
    }

    /**
     * @brief Циклический сдвиг вправо для 32-битного числа.
     *
     * @param x число, которое нужно сдвинуть.
     * @param shift количество битов сдвига.
     * @return uint32_t результат циклического сдвига вправо.
     */
    static uint32_t rotateRight(uint32_t x, unsigned int shift) {
        return (x >> shift) | (x << (32 - shift));
    }

public:
    /**
     * @brief Создает генератор с заданным начальным значением.
     *
     * @param seed начальное значение генератора.
     */
    RotateShiftMix(uint32_t seed) {
        if (seed == 0) {
            state = 1u;
        } else {
            state = seed;
        }

        add = state ^ 0xA5A5A5A5u;
    }

    /**
     * @brief Генерирует следующее псевдослучайное число.
     *
     * Сначала состояние циклически сдвигается влево и вправо на 8 бит.
     * Затем результаты складываются вместе с дополнительной переменной add.
     * После этого выполняется дополнительное перемешивание.
     *
     * @return uint32_t следующее псевдослучайное число.
     */
    uint32_t next() {
        uint32_t left = rotateLeft(state, 8);
        uint32_t right = rotateRight(state, 8);

        add += STEP;

        uint32_t result = left + right + add;

        result ^= rotateLeft(result, 13);
        result += rotateRight(result, 7);

        state = result;

        return state;
    }

    /**
     * @brief Возвращает название генератора.
     *
     * Название будет использоваться при записи результатов в CSV.
     */
    std::string name() const {
        return "RotateShiftMix";
    }
};

#endif