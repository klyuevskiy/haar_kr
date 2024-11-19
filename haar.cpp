#include "haar.hpp"

void task1()
{
    std::vector<double> img = {211, 215, 218, 220, 252, 216, 198, 142};
    auto [n, out_img] = get_optimal_n(img, 3.5, 0.1, d2_metric<double>);
    std::print("\nОтвет:\n", n);
    std::print("n: {}, итоговое изображение:\n", n);
    print_vector(out_img);
}

void task3()
{
    std::vector<std::vector<double>> m = {
        {160, 240, -80, 120},
        {0, 256, 24, 8},
        {-88, 100, 64, -4},
        {30, -18, 220, 16}
    };

    auto transformed = haar2_std(m);
    std::print("Ответ:\n");
    print_matrix(transformed);
    std::print("Самопроверка: {}", haar2_inverse_std(transformed) == m);
}

int main()
{
    task3();
}


/*
haar vector
input:
   419, 411, 419, 399, 434, 384, 410, 404
output:
    410 2 3 1 4 10 25 3
*/

/*
haar direct
input:
{
        {20, 12, 13, 11},
        {6, 2, 8, 12},
        {15, 17, 14, 8},
        {10, 6, 4, 10}
    };

output:
10.5 0.5 1.75 -0.25 
0 -1 1.25 -0.25
3.5 2.5 1 1.5
3 1 -1.5 3
*/

/*
haar pyramid
input:
{
        {20, 12, 13, 11},
        {6, 2, 8, 12},
        {15, 17, 14, 8},
        {10, 6, 4, 10}
    };

output:
10.5 0.5 3 -0.5 
0 -1 0.5 0
3.5 2.5 1 1.5
3 1 -1.5 3
*/