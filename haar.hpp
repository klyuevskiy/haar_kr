#if !defined(__HAAR_LIB_H_)
#define __HAAR_LIB_H_

#include <iostream>
#include <print>
#include <vector>
#include <cassert>
#include <bit>
#include <algorithm>
#include <ranges>
#include <execution>
#include <type_traits>
#include <cmath>
#include <tuple>

template <typename T>
void print_vector(std::vector<T> const& v)
{
    std::ranges::copy(v, std::ostream_iterator<T>(std::cout, " "));
    std::cout << std::endl;
}

template <typename T>
void print_matrix(std::vector<std::vector<T>> const& v)
{
    std::ranges::for_each(v, [](auto const &v) { print_vector(v); });
}

/* одна итерация усреднения и детализации ВП Хаара для вектора */
template <typename T>
requires std::is_arithmetic_v<T>
std::vector<T> haar_iteration(std::vector<T> const& input, int count)
{
    assert(std::has_single_bit<unsigned>(count) && "count must be power of 2");

    std::vector<T> output(input);
    for (int i = 0; i < count / 2; i++)
    {
        T const& left = input[i * 2];
        T const& right = input[i * 2 + 1];
        T avg = (left + right) / 2;
        T det = (left - right) / 2;
        
        output[i] = avg;
        output[i + count / 2] = det;
    }
    return output;
}

/* Обратная операций 1 итерации ВП Хаара вектора */
template <typename T>
requires std::is_arithmetic_v<T>
std::vector<T> haar_inverse_iteration(std::vector<T> const& input, int count)
{
    assert(std::has_single_bit<unsigned>(count) && "count must be power of 2");

    std::vector<T> output(input);
    for (int i = 0; i < count / 2; i++)
    {
        T const& avg = input[i];
        T const& det = input[i + count / 2];

        output[i * 2] = avg + det;
        output[i * 2 + 1] = avg - det;
    }
    return output;
}

/* ВП Хаара для вектора */
template <typename T>
requires std::is_arithmetic_v<T>
std::vector<T> haar(std::vector<T> const& input)
{
    assert(std::has_single_bit(input.size()) && "vector size must be power of 2");

    std::vector<T> output(input);
    for (auto size = input.size(); size > 1; size /= 2)
    {
        output = haar_iteration(std::move(output), size);
    }
    return output;
}

/* Обратное преобразование к ВП Хаара */
template <typename T>
requires std::is_arithmetic_v<T>
std::vector<T> haar_inverse(std::vector<T> const& input)
{
    assert(std::has_single_bit(input.size()) && "vector size must be power of 2");

    std::vector output(input);
    for(int size = 2; size <= input.size(); size *= 2)
    {
        output = haar_inverse_iteration(std::move(output), size);
    }
    return output;
}

/* Извелечение колонки из матрицы */
template <typename T>
requires std::is_arithmetic_v<T>
std::vector<T> extract_column(
    std::vector<std::vector<T>> const &input,
    int column_num,
    int count)
{
    /*
        Надо было делать матрицу как непрывный массив (std::mdspan) а не vector<vector>
        В таком случае можно было бы избежать копирования
    */
    std::vector<T> column(count);
    for(int row_num = 0; row_num < count; row_num++)
        column[row_num] = input[row_num][column_num];
    return column;
}

/* установить значения колонки матрицы */
template <typename T>
requires std::is_arithmetic_v<T>
void matrix_set_column(
    std::vector<std::vector<T>>& matrix,
    std::vector<T> const& column,
    int column_num,
    int count)
{
    for (int row_num = 0; row_num < count; row_num++)
    {
        matrix[row_num][column_num] = column[row_num];
    }
}

/* стандратное разложение для двумерного преобразования Хаара */
template <typename T>
requires std::is_arithmetic_v<T>
std::vector<std::vector<T>> haar2_std(std::vector<std::vector<T>> const& input)
{
    assert(std::has_single_bit(input.size()) && "matrix dims size must be power of 2");

    auto size = input.size();
    std::remove_cvref_t<decltype(input)> output(size);

    /* haar for every rows */
    for (int i = 0; i < size; i++)
        output[i] = haar(input[i]);

    /* haar for every column */
    for(int column_num = 0; column_num < size; column_num++)
    {
        std::vector<T> column = extract_column(output, column_num, size);
        matrix_set_column(output, haar(std::move(column)), column_num, size);
    }
    return output;
}

/* обратное преобразование к стандратному разложению */
template <typename T>
requires std::is_arithmetic_v<T>
std::vector<std::vector<T>> haar2_inverse_std(std::vector<std::vector<T>> const& input)
{
    assert(std::has_single_bit(input.size()) && "matrix dims size must be power of 2");

    auto size = input.size();
    std::remove_cvref_t<decltype(input)> output(input);

    /* haar inverse for every column */
    for(int column_num = 0; column_num < size; column_num++)
    {
        std::vector<T> column = extract_column(output, column_num, size);
        matrix_set_column(output, haar_inverse(std::move(column)), column_num, size);
    }

    /* haar inverse for every rows */
    for (int i = 0; i < size; i++)
        output[i] = haar_inverse(std::move(output[i]));

    return output;
}

/* пирамидальное разложение для двумерного преобразования Хаара */
template <typename T>
requires std::is_arithmetic_v<T>
std::vector<std::vector<T>> haar2_pyramid(std::vector<std::vector<T>> const& input)
{
    assert(std::has_single_bit(input.size()) && "matrix dims size must be power of 2");

    std::remove_cvref_t<decltype(input)> output(input);

    for(auto size = output.size(); size > 1; size /= 2)
    {
        /* haar 1 iteration for every row */
        std::ranges::for_each(output, [size](auto &v) {
            v = haar_iteration(std::move(v), size);
            });

        /* haar 1 iteration for every column */
        for (int column_num = 0; column_num < size; column_num++)
        {
            auto column = extract_column(output, column_num, size);
            column = haar_iteration(std::move(column), size);
            matrix_set_column(output, std::move(column), column_num, size);
        }
    }

    return output;
}

/* обратное преобразование к пирамидальному разложению */
template <typename T>
requires std::is_arithmetic_v<T>
std::vector<std::vector<T>> haar2_inverse_pyramid(std::vector<std::vector<T>> const& input)
{
    assert(std::has_single_bit(input.size()) && "matrix dims size must be power of 2");

    std::remove_cvref_t<decltype(input)> output(input);

    for(auto size = 2; size <= output.size(); size *= 2)
    {
        /* haar inverse 1 iteration for every column */
        for (int column_num = 0; column_num < size; column_num++)
        {
            auto column = extract_column(output, column_num, size);
            column = haar_inverse_iteration(std::move(column), size);
            matrix_set_column(output, std::move(column), column_num, size);
        }

        /* haar inverse 1 iteration for every row */
        std::ranges::for_each(output, [size](auto &v) {
            v = haar_inverse_iteration(std::move(v), size);
        });
    }

    return output;
}

template <typename T>
requires std::is_arithmetic_v<T>
std::vector<T> compress_image(std::vector<T> const& img, double q)
{
    assert(std::has_single_bit(img.size()) && "img size must be power of 2");
    assert(0 < q && q <= 1 && "q must be in (0; 1]");

    auto modified = haar(img);
    std::ranges::sort(modified);
    int unmodified_count = static_cast<double>(modified.size()) * q;
    std::fill(modified.begin() + unmodified_count, modified.end(), 0);
    return modified;
}

template <typename T>
requires std::is_arithmetic_v<T>
T d1_metric(std::vector<T> const& input, std::vector<T> const& output)
{
    T result{};
    for(int i = 0; i < input.size(); i++)
        result += std::abs(input[i] - output[i]);
    return result / input.size();
}

template <typename T>
requires std::is_arithmetic_v<T>
T d2_metric(std::vector<T> const& input, std::vector<T> const& output)
{
    T result{};
    for(int i = 0; i < input.size(); i++)
        result += (input[i] - output[i]) * (input[i] - output[i]);
    return result / input.size();
}

template <typename T>
requires std::is_arithmetic_v<T>
T dpsnr_metric(std::vector<T> const& input, std::vector<T> const& output, int max_limit /* aka M */ = 255)
{
    T result{};
    for(int i = 0; i < input.size(); i++)
        result += (input[i] - output[i]) * (input[i] - output[i]);
    result /= input.size();
    return 10. * std::log10(max_limit * max_limit / result);
}

template <typename T>
auto fill_smallest_by_zero(
    std::vector<T> const &img,
    std::vector<std::tuple<T, int>>
    const &sorted_with_indexes, int for_zero)
{   
    
    /* берем значения для обнуления */
    auto output = img;
    /* обнуляем по соответсвующим индексам */
    for(auto [el, index] : std::ranges::views::take(sorted_with_indexes, for_zero))
        output[index] = 0;
    return output;
}

/* найти оптимальное сжатие с заданным качеством */
template <typename T, typename MetricFunc>
requires requires(MetricFunc f, std::vector<T> const& img1, std::vector<T> const& img2)
{
    requires std::is_arithmetic_v<T>;
    f(img1, img2);
}
std::tuple<int, std::vector<T>>
get_optimal_n(
    std::vector<T> const& img,
    double target_metric_val,
    double eps,
    MetricFunc metric_func)
{
    /* после ВП Хаара*/
    auto modified = haar(img);
    std::vector<std::tuple<T, int>> sorted_with_indexes;
    std::ranges::transform(modified, std::back_inserter(sorted_with_indexes),
    [i = 0] (auto element) mutable
    {
        return std::make_tuple(std::abs(element), i++);
    });
    std::ranges::sort(sorted_with_indexes);

    std::print("afrer haar img:\n");
    print_vector(modified);

    /* начинаем с самого лучшего сжатия */
    for(int i = img.size(); i >= 0; i--)
    {
        /* обнуляем наименьшие n */
        auto truncated = fill_smallest_by_zero(modified, sorted_with_indexes, i);

        /* восстанваливаем и считаем метрику */
        auto inversed = haar_inverse(truncated);
        auto metric_val = metric_func(img, inversed);

        /* печатаем промежуточные результаты */
        std::print("n = {}, metric = {}\n", i, metric_val);

        if (std::abs(metric_val) <= target_metric_val)
            return {i, inversed};
    }
    return {-1, {}};
}

#endif