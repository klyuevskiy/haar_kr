#include "haar.hpp"

void metric_res(std::vector<double> v, double target_metric, double eps)
{
    int n = get_optimal_n(v, target_metric, eps, d2_metric<double>);
    if (n != -1)
    {
       std::print("Подходящее значение n: {}\n", n);
    }
    else
    {
        std::print("Не удалось найти значение n: {}\n", n);
    }
}

void metrics_search()
{
    std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
        // X - целевая метрика
    double target_metric = 0.5;
        // Допустимая погрешность
    double eps = 0.1;
    metric_res(data, target_metric, eps);
    metric_res(data, 0.8, 0.1);
}

int main()
{
    //metrics_search();

    std::vector v { 419, 411, 419, 399, 434, 384, 410, 404 };
    // std::vector<std::vector<int>> m = {
    //     {4, 6, 10, 12},
    //     {8, 6, 10, 12},
    //     {10, 14, 12, 14},
    //     {12, 14, 14, 14}
    // };

    std::vector<std::vector<double>> m = {
        {20, 12, 13, 11},
        {6, 2, 8, 12},
        {15, 17, 14, 8},
        {10, 6, 4, 10}
    };

    // std::vector<double> img = {249, 247, 243, 241, 180, 184, 235, 237};
    // auto compressed = compress_image(img, 1. - 0.375);
    // auto inversed = haar_inverse(compressed);
    // print_vector(inversed);
    // std::print("d1: {}, dpsnr: {}\n", d1_metric(img, inversed), dpsnr_metric(img, inversed));
    //print_matrix(m);
    assert(haar2_inverse_pyramid(haar2_pyramid(m)) == m);
    assert(haar2_inverse_std(haar2_std(m)) == m);
    print_matrix(haar2_inverse_std(haar2_std(m)));
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