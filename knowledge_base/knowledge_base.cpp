
#include <iostream>
#include <string>

#include "../bloom_filter/bloom_filter.hpp"

class KnowledgeBase{
    public:
        int KnowledgeBase()
        {

            // How many elements roughly do we expect to insert?
            parameters.projected_element_count = 1000;

            // Maximum tolerable false positive probability? (0,1)
            parameters.false_positive_probability = 0.0001; // 1 in 10000


            if (!parameters)
            {
                std::cout << "Error - Invalid set of bloom filter parameters!" << std::endl;
                return 1;
            }

            parameters.compute_optimal_parameters();

            if(!init_base())
            {
                std::cout << "Error - Could not possible to initiate the base!" << std::endl;
                return 1;
            }

            return 0;
        }
        bool faultCodeIsPresent(const std::string& key)
        {
            return bloomFilter().contains(key);
        }
        void addFaultCode(const std::string& key)
        {
            bloomFilter().insert(key);
        }
    private:
        bloom_parameters parameters;
        bloom_filter bloomFilter();

        int init_base(){
            std::vector<std::string> fault_list;

            if (!load_fault_from_file(fault_list))
            {
                return 1;
            }
            bloom_filter filter(parameters);
            bloomFilter = filter;
            bloomFilter.insert(fault_list.begin(),fault_list.end());

            return 0;
        }
        int load_fault_from_file(std::vector<std::string>& fault_list)
    {
        static const std::string fl_list[] =
                { "_fault_codes_dir/_fault_codes_list.txt",
                };
        std::size_t index = 0;

        std::cout << "Loading list " << fl_list[index] << ".....";

        if (!read_file(fl_list[index],fault_list))
        {
            return false;
        }

        if (word_list.empty())
        {
            std::cout << "No fault list - Either none requested, or desired word list could not be loaded." << std::endl;
            return false;
        }
        else
            std::cout << " Complete." << std::endl;

        return true;

    }
        template <class T,
            class Allocator,
            template <class,class> class Container>
        bool read_file(const std::string& file_name, Container<T, Allocator>& c)
    {
        std::ifstream stream(file_name.c_str());

        if (!stream)
        {
            std::cout << "Error: Failed to open file '" << file_name << "'" << std::endl;
            return false;
        }

        std::string buffer;

        while (std::getline(stream,buffer))
        {
            c.push_back(buffer);
            c.push_back(uppercase(buffer));
        }

        return true;
    }
        std::string uppercase(std::string str)
    {
        for (std::size_t i = 0; i < str.size(); ++i)
        {
            str[i] = static_cast<char>(toupper(str[i]));
        }

        return str;
    }

};






