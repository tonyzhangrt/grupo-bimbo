#ifndef __BIMBO_RED_ENTRY_H__
#define __BIMBO_RED_ENTRY_H__

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <fstream>
#include <set>

class BimboReducedEntry{
public:
    unsigned int semana, depot_id, channel_id, route_id;
    int demanda_uni_equil;

    BimboReducedEntry (unsigned int, unsigned int, unsigned int, unsigned int, int);
};

struct reduced_entry_cmp{
    bool operator()(const BimboReducedEntry &, const BimboReducedEntry &);
};

class BimboClientProductInfo{
public:
    unsigned int client_id, product_id;
    std::vector<BimboReducedEntry> entries;

    BimboClientProductInfo(){};

    BimboClientProductInfo(unsigned int, unsigned int);
};

void generate_client_product_count(std::ifstream &, std::unordered_map<unsigned long, std::vector<int> > &, bool require_positive = false);

void process_client_product_count(std::unordered_map<unsigned long, std::vector<int> > &);

void check_client_product_pair(std::ifstream &, std::pair<int, int>, bool is_train = true,
    bool require_positive = false);

void generate_client_count(std::ifstream &, std::unordered_map<unsigned int, std::unordered_set<int> > &, std::unordered_set<int> &, bool is_train = true);

void process_client_count(std::unordered_map<unsigned int, std::unordered_set<int> > &);

void generate_train_client_product_info(std::ifstream &, std::unordered_map<unsigned long, BimboClientProductInfo> &, bool require_positive = false);

void generate_basic_train_lag(std::ofstream &, std::unordered_map<unsigned long, BimboClientProductInfo> &, std::pair<int, int> &, std::set<unsigned int>&);

void generate_basic_test_lag(std::ofstream &, std::ifstream &, std::unordered_map<unsigned long, BimboClientProductInfo> &, std::pair<int, int> &);

#endif