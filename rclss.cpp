
#include "entry.h"

#include <dlib/clustering.h>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace dlib;

int main(int argc, char* argv[])
{
    std::string filename;
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: rclss <modelfname>\n";
            return 1;
        }

        filename = argv[1];
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
    sample_type m;
    std::vector<sample_type> values_range;

    deserialize(filename + ".mdl") >> test >> values_range;

    std::string input;
    char buf[64];

    while (getline(std::cin, input))
    {
        // Parse input
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
                buf[counter] = '\0';
                m(idx) = std::atof(buf);
                counter = 0;
                idx++;
            }
        }
        buf[counter] = '\0';
        m(idx) = std::atof(buf);

        // Normalize
        for (int i = 0; i < 7; i++)
        {
            auto val = (m(i) - values_range[0](i)) / (values_range[1](i) - values_range[0](i));
            m(i) = val > 0 ? (val < 1 ? val : 1) : 0;
        }

        // Open and sort by range entries in same cluster
        int bucket = test(m);
        std::stringstream ss;
        ss << filename << ".dat" << bucket;

        std::vector<Entry> close_entries;
        deserialize(ss.str()) >> close_entries;

        std::sort(close_entries.begin(), close_entries.end(),
                  [&m, &values_range](const auto& lhs, const auto& rhs)
            {
                return lhs.SqaredDist(m, values_range) < rhs.SqaredDist(m, values_range);
            });
        for (const auto& ent : close_entries)
        {
            std::cout << ent << std::endl;
        }
    }
}
