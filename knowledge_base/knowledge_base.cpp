/**
* Gharib Damiche
*for ARSLab Carleton
*
* KnowledgeBase:
* Save fault codes in bloom filter with friendly functions to access
*/

#include <iostream>
#include <string>
#include <cstddef>
#include <cstdlib>
#include "../bloom-filter/bloom_filter.hpp"

#include <boost/algorithm/string.hpp>

class KnowledgeBase {
public:
//////////////////////////////////////PUBLIC SPACE////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

    /**
     * init
     *
     * @return int
     */
    int init() {

        int init = init_base();
        if (init == -1) {
            std::cout << "Error - Could not possible to initiate the base!" << std::endl;
            return 1;
        }
        if (init == 0) {
            std::cout << "No data in the file to initiate the base " << std::endl;
            return 0;
        }

        return 0;
    }

    /**
     *
     * @return
     */
    int save_faults_code() {
        return write_file();
    }

    /**
     * Fault code is present?
     *
     * @param string key
     *
     * @return std::map<char,int>::iterator fault_iterator
     */
    std::map<std::string, int>::iterator get_fault_code(std::string &key) {
        faults_code_it = faults_code.find(key);
        if (faults_code_it != faults_code.end()) {
            return faults_code_it;
        } else {
            return faults_code.end();
        }
    }

    /**
     * Fault code is present?
     *
     * @param string key
     *
     * @return bool isPresent
     */
    bool fault_code_is_present(std::string &key) {
        faults_code_it = faults_code.find(key);
        if (faults_code_it != faults_code.end()) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Add fault code.
     *
     * @param key
     * @param value
     *
     * @return bool response
     */
    bool add_fault_code(std::string &key, int value) {
        if (!fault_code_is_present(key)) {
            faults_code[key] = value;
            return true;
        } else {
            return false;
        }
    }

    /**
     * Update fault code value.
     *
     * @param string key
     * @param string value
     *
     * @return bool response
     */
    bool update_fault_code(std::string &key, int value) {
            faults_code[key] = value;
            return true;
    }

private:
//////////////////////////////////////PRIVATE SPACE///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
    bloom_parameters parameters;
    bloom_filter bloomFilter;
    map<std::string, int> faults_code;
    std::map<std::string, int>::iterator faults_code_it;
    std::string filename = "/home/gharib/Documents/Project/SensorFusion/RT-Cadmium-SensorFusion/knowledge_base/_fault_codes_dir/_fault_codes_list.txt";

    /**
     * Initiate the base. (for Bloom Filter)
     *
     * @return int response
     */
    int init_base() {
        return read_file();
    }

    /**
     * Write a map in file
     *
     * @param fname
     * @param m
     *
     * @return Number of written datas
     */
    int write_file() {
        int count = 0;
        if (faults_code.empty())
            return 0;

        FILE *fp = fopen(filename.c_str(), "w");
        if (!fp)
            return -errno;

        for (faults_code_it = faults_code.begin(); faults_code_it != faults_code.end(); faults_code_it++) {
            fprintf(fp, "%sF%d\n", faults_code_it->first.c_str(), faults_code_it->second);
            count++;
        }

        fclose(fp);
        return count;
    }

/**
 * Read a fault code file and save in the map fault_code
 *
 * @return int count
 */
    int read_file() {
        int count = 0;
        if (access(filename.c_str(), R_OK) < 0)
            return -1;

        FILE *fp = fopen(filename.c_str(), "r");
        if (!fp)
            return -1;

        faults_code.clear();

        char *buf = 0;
        size_t buflen = 0;

        while (getline(&buf, &buflen, fp) > 0) {
            char *nl = strchr(buf, '\n');
            if (nl == NULL)
                continue;
            *nl = 0;

            char *sep = strchr(buf, 'F');
            if (sep == NULL)
                continue;
            *sep = 0;
            sep++;

            std::string s1 = buf;
            int s2 = atoi(sep);
            faults_code[s1] = s2;

            count++;
        }

        if (buf)
            free(buf);

        fclose(fp);
        return count;
    }

    /**
     * Uppercase string.
     *
     * @param string str
     *
     * @return string str
     */
    std::string uppercase(std::string str) {
        for (std::size_t i = 0; i < str.size(); ++i) {
            str[i] = static_cast<char>(toupper(str[i]));
        }

        return str;
    }

};

