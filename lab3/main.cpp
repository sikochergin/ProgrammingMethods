#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <utility>
#include <chrono>
#include <random>

#include "LCGMix.h"
#include "MiddleSquareWeyl.h"
#include "RotateShiftMix.h"

const double TWO32 = 4294967296.0;

/**
 * @brief Структура для хранения одной выборки.
 *
 * rawValues хранят исходные 32-битные числа генератора.
 * normalizedValues хранят эти же числа, но приведенные к диапазону [0, 1).
 */
struct Sample {
    std::vector<uint32_t> rawValues;
    std::vector<double> normalizedValues;
};

/**
 * @brief Структура для хранения базовых статистических характеристик выборки.
 *
 * mean — среднее значение.
 * stddev — стандартное отклонение.
 * coefficientVariation — коэффициент вариации.
 */
struct BasicStats {
    double mean;
    double stddev;
    double coefficientVariation;
};

/**
 * @brief Структура для хранения результата одного теста.
 *
 * testName — название теста.
 * statistic — рассчитанное значение статистики.
 * passed — прошел тест или нет.
 */
struct TestResult {
    std::string testName;
    double statistic;
    bool passed;
};

/**
 * @brief Структура для хранения результата Хи-квадрат проверки равномерности.
 *
 * statistic — рассчитанное значение Хи-квадрат.
 * criticalValue — критическое значение.
 * passed — прошла выборка проверку или нет.
 */
struct ChiSquareResult {
    double statistic;
    double criticalValue;
    bool passed;
};

/**
 * @brief Генерирует одну выборку для переданного генератора.
 *
 * @tparam Generator тип генератора
 * @param generator объект генератора
 * @param sampleSize количество элементов в выборке
 * @return Sample выборка с исходными и нормированными значениями
 */
template <typename Generator>
Sample generateSample(Generator& generator, int sampleSize) {
    Sample sample;

    sample.rawValues.reserve(sampleSize);
    sample.normalizedValues.reserve(sampleSize);

    for (int i = 0; i < sampleSize; i++) {
        uint32_t value = generator.next();

        sample.rawValues.push_back(value);
        sample.normalizedValues.push_back((static_cast<double>(value) + 0.5) / TWO32);
    }

    return sample;
}

//МАТЕМАТИКА

/**
 * @brief Считает среднее значение выборки.
 *
 * @param values выборка нормированных значений
 * @return double среднее значение
 */
double calculateMean(const std::vector<double>& values) {
    if (values.empty()){
        return 0.0;
    }
    
    double sum = 0.0;

    for (double value : values) {
        sum += value;
    }

    return sum / values.size();
}

/**
 * @brief Считает стандартное отклонение выборки.
 *
 * Стандартное отклонение показывает, насколько сильно значения
 * в среднем отклоняются от среднего значения.
 *
 * @param values выборка нормированных значений
 * @param mean заранее посчитанное среднее значение
 * @return double стандартное отклонение
 */
double calculateStddev(const std::vector<double>& values, double mean) {
    if (values.size() < 2){
        return 0.0;
    }

    double sumSquares = 0.0;

    for (double value : values) {
        double diff = value - mean;
        sumSquares += diff * diff;
    }

    return std::sqrt(sumSquares / (values.size() - 1));
}

/**
 * @brief Считает коэффициент вариации.
 *
 * Коэффициент вариации равен стандартному отклонению,
 * деленному на среднее значение.
 *
 * @param mean среднее значение
 * @param stddev стандартное отклонение
 * @return double коэффициент вариации
 */
double calculateCoefficientVariation(double mean, double stddev) {
    if (mean == 0.0) {
        return 0.0;
    }

    return stddev / mean;
}

/**
 * @brief Считает все базовые статистические характеристики выборки.
 *
 * @param values выборка нормированных значений
 * @return BasicStats структура со средним, отклонением и коэффициентом вариации
 */
BasicStats calculateBasicStats(const std::vector<double>& values) {
    BasicStats stats;

    stats.mean = calculateMean(values);
    stats.stddev = calculateStddev(values, stats.mean);
    stats.coefficientVariation = calculateCoefficientVariation(stats.mean, stats.stddev);

    return stats;
}

//ТЕСТЫ

/**
 * @brief Приближенно считает обратную функцию стандартного нормального распределения.
 *
 * То есть по вероятности p возвращает такое значение z,
 * что P(Z <= z) = p для стандартной нормальной случайной величины.
 *
 * Используется аппроксимация Питера Аклама.
 *
 * @param p вероятность из диапазона (0, 1)
 * @return double квантиль стандартного нормального распределения
 */
double inverseNormalCDF(double p) {
    if (p <= 0.0) {
        return -INFINITY;
    }

    if (p >= 1.0) {
        return INFINITY;
    }

    const double a1 = -39.69683028665376;
    const double a2 = 220.9460984245205;
    const double a3 = -275.9285104469687;
    const double a4 = 138.3577518672690;
    const double a5 = -30.66479806614716;
    const double a6 = 2.506628277459239;

    const double b1 = -54.47609879822406;
    const double b2 = 161.5858368580409;
    const double b3 = -155.6989798598866;
    const double b4 = 66.80131188771972;
    const double b5 = -13.28068155288572;

    const double c1 = -0.007784894002430293;
    const double c2 = -0.3223964580411365;
    const double c3 = -2.400758277161838;
    const double c4 = -2.549732539343734;
    const double c5 = 4.374664141464968;
    const double c6 = 2.938163982698783;

    const double d1 = 0.007784695709041462;
    const double d2 = 0.3224671290700398;
    const double d3 = 2.445134137142996;
    const double d4 = 3.754408661907416;

    const double pLow = 0.02425;
    const double pHigh = 1.0 - pLow;

    double q;
    double r;

    if (p < pLow) {
        q = std::sqrt(-2.0 * std::log(p));

        return (((((c1 * q + c2) * q + c3) * q + c4) * q + c5) * q + c6) /
               ((((d1 * q + d2) * q + d3) * q + d4) * q + 1.0);
    }

    if (p <= pHigh) {
        q = p - 0.5;
        r = q * q;

        return (((((a1 * r + a2) * r + a3) * r + a4) * r + a5) * r + a6) * q /
               (((((b1 * r + b2) * r + b3) * r + b4) * r + b5) * r + 1.0);
    }

    q = std::sqrt(-2.0 * std::log(1.0 - p));

    return -(((((c1 * q + c2) * q + c3) * q + c4) * q + c5) * q + c6) /
            ((((d1 * q + d2) * q + d3) * q + d4) * q + 1.0);
}

/**
 * @brief Приближенно считает правое критическое значение Хи-квадрат.
 */
double chiSquareCriticalValue(int degreesOfFreedom, double alpha) {
    if (degreesOfFreedom <= 0) {
        return 0.0;
    }

    double k = static_cast<double>(degreesOfFreedom);
    double z = inverseNormalCDF(1.0 - alpha);

    double value = 1.0 - 2.0 / (9.0 * k) + z * std::sqrt(2.0 / (9.0 * k));

    return k * value * value * value;
}


/**
 * @brief Считает количество единичных битов в числе.
 *
 * @param value 32-битное число
 * @return int количество единичных битов
 */
int countOneBits(uint32_t value) {
    int count = 0;

    while (value != 0) {
        count += static_cast<int>(value & 1u);
        value >>= 1;
    }

    return count;
}

/**
 * @brief Частотный битовый тест.
 *
 * Проверяет, примерно ли одинаково количество нулей и единиц
 * в битовом представлении выборки.
 *
 * Для хорошего генератора доля единиц должна быть близка к 0.5.
 *
 * @param values исходные 32-битные значения генератора
 * @return TestResult результат теста
 */
TestResult frequencyTest(const std::vector<uint32_t>& values) {
    int ones = 0;
    int totalBits = static_cast<int>(values.size()) * 32;

    for (uint32_t value : values) {
        ones += countOneBits(value);
    }

    double expected = totalBits / 2.0;
    double stddev = std::sqrt(totalBits / 4.0);
    double z = (ones - expected) / stddev;

    bool passed = std::fabs(z) <= 1.96;

    return {"Frequency", std::fabs(z), passed};
}

/**
 * @brief Тест серий битов.
 *
 * Проверяет, насколько часто в битовой последовательности происходят
 * переходы между 0 и 1.
 *
 * Если переходов слишком мало, значит есть слишком длинные серии одинаковых битов.
 * Если переходов слишком много, значит биты слишком регулярно чередуются.
 *
 * @param values исходные 32-битные значения генератора
 * @return TestResult результат теста
 */
TestResult runsTest(const std::vector<uint32_t>& values) {
    int totalBits = static_cast<int>(values.size()) * 32;

    int previousBit = -1;
    int runs = 0;

    for (uint32_t value : values) {
        for (int bitIndex = 0; bitIndex < 32; bitIndex++) {
            int currentBit = static_cast<int>((value >> bitIndex) & 1u);

            if (currentBit != previousBit) {
                runs++;
            }

            previousBit = currentBit;
        }
    }

    double expected = totalBits / 2.0;
    double stddev = std::sqrt(totalBits / 4.0);
    double z = (runs - expected) / stddev;

    bool passed = std::fabs(z) <= 1.96;

    return {"Runs", std::fabs(z), passed};
}

/**
 * @brief Serial 2-bit test.
 *
 * Проверяет распределение пар битов:
 *
 * 00, 01, 10, 11
 *
 * У хорошего генератора эти пары должны встречаться примерно одинаково часто.
 *
 * @param values исходные 32-битные значения генератора
 * @return TestResult результат теста
 */
TestResult serial2Test(const std::vector<uint32_t>& values) {
    int counts[] = {0, 0, 0, 0};
    int totalPairs = 0;

    for (uint32_t value : values) {
        for (int bitIndex = 0; bitIndex < 31; bitIndex++) {
            int pair = static_cast<int>((value >> bitIndex) & 3u);
            counts[pair]++;
            totalPairs++;
        }
    }

    double expected = totalPairs / 4.0;
    double chiSquare = 0.0;

    for (int i = 0; i < 4; i++) {
        double diff = counts[i] - expected;
        chiSquare += diff * diff / expected;
    }

    double critical = chiSquareCriticalValue(3, 0.05);
    bool passed = chiSquare <= critical;

    return {"Serial2", chiSquare, passed};
}

/**
 * @brief Poker 4-bit test.
 *
 * Делит битовую последовательность на блоки по 4 бита.
 * Возможных блоков всего 16: от 0000 до 1111.
 *
 * У хорошего генератора все 16 вариантов должны встречаться примерно равномерно.
 *
 * @param values исходные 32-битные значения генератора
 * @return TestResult результат теста
 */
TestResult poker4Test(const std::vector<uint32_t>& values) {
    int counts[16];
    for (int i = 0; i < 16; i++) {
        counts[i] = 0;
    }

    int totalBlocks = 0;

    for (uint32_t value : values) {
        for (int shift = 0; shift < 32; shift += 4) {
            int block = static_cast<int>((value >> shift) & 15u);
            counts[block]++;
            totalBlocks++;
        }
    }

    double expected = totalBlocks / 16.0;
    double chiSquare = 0.0;

    for (int i = 0; i < 16; i++) {
        double diff = counts[i] - expected;
        chiSquare += diff * diff / expected;
    }

    double critical = chiSquareCriticalValue(15, 0.05);
    bool passed = chiSquare <= critical;

    return {"Poker4", chiSquare, passed};
}

/**
 * @brief Overlapping permutations test.
 *
 * Берет группы из пяти подряд идущих нормированных чисел
 * и смотрит порядок их расположения.
 *
 * Для пяти чисел существует 5! = 120 возможных порядков.
 * @param values нормированные значения из диапазона [0, 1)
 * @return TestResult результат теста
 */
TestResult overlappingPermutationsTest(const std::vector<double>& values) {
    int permutationFrequency[120];

    for (int i = 0; i < 120; i++) {
        permutationFrequency[i] = 0;
    }

    int factorials[5] = {1, 1, 2, 6, 24};

    int totalWindows = 0;

    for (int start = 0; start + 5 <= static_cast<int>(values.size()); start++) {
        std::pair<double, int> window[5];

        for (int position = 0; position < 5; position++) {
            window[position] = std::make_pair(values[start + position], position);
        }

        std::sort(window, window + 5);

        int permutation[5];

        for (int sortedPosition = 0; sortedPosition < 5; sortedPosition++) {
            permutation[sortedPosition] = window[sortedPosition].second;
        }

        int permutationIndex = 0;

        for (int i = 0; i < 5; i++) {
            int smallerOnRight = 0;

            for (int j = i + 1; j < 5; j++) {
                if (permutation[j] < permutation[i]) {
                    smallerOnRight++;
                }
            }

            permutationIndex += smallerOnRight * factorials[4 - i];
        }

        permutationFrequency[permutationIndex]++;
        totalWindows++;
    }

    double expected = totalWindows / 120.0;
    double chiSquare = 0.0;

    for (int i = 0; i < 120; i++) {
        double diff = permutationFrequency[i] - expected;
        chiSquare += diff * diff / expected;
    }

    double criticalValue = chiSquareCriticalValue(119, 0.05);
    bool passed = chiSquare <= criticalValue;

    return {"OverlappingPermutations", chiSquare, passed};
}

/**
 * @brief Запускает все тесты для одной выборки.
 *
 * @param sample выборка генератора
 * @return std::vector<TestResult> результаты всех тестов
 */
std::vector<TestResult> runAllTests(const Sample& sample) {
    std::vector<TestResult> results;

    results.push_back(frequencyTest(sample.rawValues));
    results.push_back(runsTest(sample.rawValues));
    results.push_back(serial2Test(sample.rawValues));
    results.push_back(poker4Test(sample.rawValues));
    results.push_back(overlappingPermutationsTest(sample.normalizedValues));

    return results;
}


/**
 * @brief Проверяет нормированные значения на равномерность распределения.
 *
 * Значения из диапазона [0, 1) делятся на несколько равных интервалов.
 * Если распределение равномерное, то в каждый интервал должно попасть
 * примерно одинаковое количество значений.
 *
 * @param values нормированные значения из диапазона [0, 1)
 * @return ChiSquareResult результат проверки
 */
ChiSquareResult chiSquareUniformTest(const std::vector<double>& values) {
    int binCount = static_cast<int>(std::log2(values.size()));
    if (binCount < 5){
        binCount = 5;
    }
    
    std::vector<int> bins(binCount, 0);

    for (double value : values) {
        int index = static_cast<int>(value * binCount);

        if (index == binCount) {
            index = binCount - 1;
        }

        bins[index]++;
    }

    double expected = values.size() / static_cast<double>(binCount);
    double chiSquare = 0.0;

    for (int count : bins) {
        double diff = count - expected;
        chiSquare += diff * diff / expected;
    }

    double criticalValue = chiSquareCriticalValue(binCount - 1, 0.05);

    bool passed = chiSquare <= criticalValue;

    return {chiSquare, criticalValue, passed};
}

/**
 * @brief Запускает эксперименты для одного генератора.
 *
 * Для генератора создается 20 выборок по 5000 элементов.
 * Для каждой выборки считаются: среднее, стандартное отклонение, коэффициент вариации, 
 * Хи-квадрат проверка равномерности, 5 NIST/Diehard-подобных тестов.
 *
 * Результаты записываются в CSV-файлы.
 *
 * @tparam Generator тип генератора
 * @param summaryFile файл для базовой статистики и Хи-квадрат проверки
 * @param testsFile файл для результатов NIST/Diehard-подобных тестов
 * @param baseSeed базовое начальное значение
 */
template <typename Generator>
void runGeneratorExperiments(std::ofstream& summaryFile,
                             std::ofstream& testsFile,
                             uint32_t baseSeed) {
    const int sampleCount = 20; //сколько выблрок генерируем
    const int sampleSize = 5000; //сколько элементов в каждой выборке

    for (int sampleIndex = 1; sampleIndex <= sampleCount; sampleIndex++) {
        uint32_t seed = baseSeed + static_cast<uint32_t>(sampleIndex) * 1000u;

        Generator generator(seed);

        Sample sample = generateSample(generator, sampleSize);

        BasicStats stats = calculateBasicStats(sample.normalizedValues);

        ChiSquareResult chiResult = chiSquareUniformTest(sample.normalizedValues);

        summaryFile << generator.name() << ","
                    << sampleIndex << ","
                    << stats.mean << ","
                    << stats.stddev << ","
                    << stats.coefficientVariation << ","
                    << chiResult.statistic << ","
                    << chiResult.criticalValue << ","
                    << chiResult.passed << "\n";

        std::vector<TestResult> testResults = runAllTests(sample);

        for (const TestResult& test : testResults) {
            testsFile << generator.name() << ","
                      << sampleIndex << ","
                      << test.testName << ","
                      << test.statistic << ","
                      << test.passed << "\n";
        }
    }
}

//ПОДСЧЕТ ВРЕМЕНИ

/**
 * @brief Замеряет время генерации чисел для одного из наших генераторов.
 *
 * @tparam Generator тип генератора
 * @param seed начальное значение генератора
 * @param count количество генерируемых чисел
 * @return double время генерации в микросекундах
 */
template <typename Generator>
double measureCustomGeneratorSpeed(uint32_t seed, int count) {
    Generator generator(seed);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < count; i++) {
        generator.next();
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::micro> duration = end - start;

    return duration.count();
}

/**
 * @brief Замеряет время генерации чисел для стандартного генератора std::mt19937.
 *
 * @param seed начальное значение генератора
 * @param count количество генерируемых чисел
 * @return double время генерации в миллисекундах
 */
double measureMt19937Speed(uint32_t seed, int count) {
    std::mt19937 generator(seed);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < count; i++) {
        generator();
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::micro> duration = end - start;

    return duration.count();
}

/**
 * @brief Записывает результаты замера скорости в speed.csv.
 *
 * Проверяются размеры от 1000 до 1000000 элементов.
 *
 * @param speedFile файл для записи результатов скорости
 */
void writeSpeedResults(std::ofstream& speedFile) {
    std::vector<int> sizes = {1000, 5000, 10000, 50000, 100000, 250000, 500000, 1000000};

    for (int size : sizes) {
        double lcgTime = measureCustomGeneratorSpeed<LCGMix>(12345u, size);
        speedFile << "LCGMix," << size << "," << lcgTime << "\n";

        double middleTime = measureCustomGeneratorSpeed<MiddleSquareWeyl>(12345u, size);
        speedFile << "MiddleSquareWeyl," << size << "," << middleTime << "\n";

        double rotateTime = measureCustomGeneratorSpeed<RotateShiftMix>(12345u, size);
        speedFile << "RotateShiftMix," << size << "," << rotateTime << "\n";

        double mtTime = measureMt19937Speed(12345u, size);
        speedFile << "std_mt19937," << size << "," << mtTime << "\n";
    }
}

//MAIN

int main() {
    std::ofstream summaryFile("results_summary.csv");
    std::ofstream testsFile("tests_detail.csv");
    std::ofstream speedFile("speed.csv");

    if (!summaryFile.is_open()) {
        std::cout << "Ошибка открытия results_summary.csv" << std::endl;
        return 1;
    }

    if (!testsFile.is_open()) {
        std::cout << "Ошибка открытия tests_detail.csv" << std::endl;
        return 1;
    }

    if (!speedFile.is_open()) {
        std::cout << "Ошибка открытия speed.csv" << std::endl;
        return 1;
    }

    summaryFile << "method,sample,mean,stddev,coefficient_variation,chi_square,chi_critical,chi_pass\n";
    testsFile << "method,sample,test_name,statistic,passed\n";
    speedFile << "method,size,microseconds\n";

    runGeneratorExperiments<LCGMix>(summaryFile, testsFile, 10000u);
    runGeneratorExperiments<MiddleSquareWeyl>(summaryFile, testsFile, 20000u);
    runGeneratorExperiments<RotateShiftMix>(summaryFile, testsFile, 30000u);

    writeSpeedResults(speedFile);

    summaryFile.close();
    testsFile.close();
    speedFile.close();

    std::cout << "Программа завершена." << std::endl;

    return 0;
}