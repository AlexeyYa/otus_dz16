#include "entry.h"

#include <dlib/clustering.h>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace dlib;

int main(int argc, char* argv[])
{
    int nclusters;
    std::string filename;
    try
    {
        if (argc < 3)
        {
            std::cerr << "Usage: rclst <nclusters> <modelfname>\n";
            return 1;
        }

        nclusters = std::atoi(argv[1]);
        filename = argv[2];
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
        return 1;
    }

    // Dlib types and variables
    typedef matrix<double,7,1> sample_type;
    typedef radial_basis_kernel<sample_type> kernel_type;
    kcentroid<kernel_type> kc(kernel_type(0.1),0.01, 8);
    kkmeans<kernel_type> test(kc);

    std::vector<sample_type> samples;
    std::vector<Entry> entries;
    std::vector<sample_type> initial_centers;

    std::vector<sample_type> values_range;
    values_range.resize(2);

    sample_type m;

    // Parse input
    std::string input;
    char buf[64];
    int floor=1, floor_total=1;

    while (getline(std::cin, input))
    {
        int counter = 0, idx = 0;
        for (auto c : input)
        {
            if (c != ';')
            {
                buf[counter] = c;
                counter++;
            }
            else
            {
                if (idx < 6)
                {
                    buf[counter] = '\0';
                    m(idx) = std::atof(buf);
                    counter = 0;
                    idx++;
                }
                else if (idx == 6)
                {
                    buf[counter] = '\0';
                    floor = std::atoi(buf);
                    counter = 0;
                }
            }
        }
        buf[counter] = '\0';
        floor_total = std::atoi(buf);


        m(6) = (floor == 1 || floor == floor_total);

        samples.emplace_back(m);
        entries.emplace_back(m(0), m(1), m(2), m(3), m(4), m(5), floor, floor_total);
    }

    // Normalize and store minmax values
    for (int i = 0; i < 6; i++)
    {
        auto min = (*std::min_element(samples.begin(), samples.end(),
                                    [i](auto lhs, auto rhs){ return lhs(i) < rhs(i);}))(i);
        auto max = (*std::max_element(samples.begin(), samples.end(),
                                    [i](auto lhs, auto rhs){ return lhs(i) < rhs(i);}))(i);
        values_range[0](i) = min;
        values_range[1](i) = max;
        auto diff = max - min;
        for (auto& sample : samples)
        {
            sample(i) = (sample(i) - min) / diff;
        }
    }
    values_range[0](6) = 0;
    values_range[1](6) = 1;

    // Learn
    test.set_number_of_centers(nclusters);
    pick_initial_centers(nclusters, initial_centers, samples, test.get_kernel());
    test.train(samples,initial_centers);

    // Saving model
    serialize(filename + ".mdl") << test << values_range;

    std::vector<std::vector<Entry>> out_vec;
    out_vec.resize(nclusters);
    for (size_t i = 0; i < samples.size(); i++)
    {
        int clust = test(samples[i]);
        out_vec[clust].emplace_back(entries[i]);
    }
    for (int i = 0; i < nclusters; i++)
    {
        std::stringstream ss;
        ss << filename << ".dat" << i;
        serialize(ss.str()) << out_vec[i];
    }
}
