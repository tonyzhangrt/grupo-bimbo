#ifndef __BIMBO_ENTRY_H__
#define __BIMBO_ENTRY_H__

#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>

class BimboEntry{
public:
    unsigned int semana, depot_id, channel_id, route_id, client_id, product_id;
    int venta_uni_hoy, dev_uni_proxima, demanda_uni_equil;
    double venta_hoy, dev_proxima;

    BimboEntry (unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, int, 
        double, int, double, int);
};


struct entry_cmp{
    bool operator()(const BimboEntry &, const BimboEntry &);
};

void generate_entries_from_train(std::ifstream &, std::vector<BimboEntry> &, bool is_train = true, bool is_append = false);

void get_hash(std::vector<bool> &, std::vector<unsigned int> &);

void get_client_hash(std::vector<bool> &, std::vector<unsigned int> &, const BimboEntry &);

void get_depot_hash(std::vector<bool> &, std::vector<unsigned int> &, const BimboEntry &);

void get_route_hash(std::vector<bool> &, std::vector<unsigned int> &, const BimboEntry &);

void get_product_hash(std::vector<bool> &, std::vector<unsigned int> &, const BimboEntry &);

void get_client_product_hash(std::vector<bool> &, std::vector<unsigned int> &, const BimboEntry &);

void get_route_product_hash(std::vector<bool> &, std::vector<unsigned int> &, const BimboEntry &);

void group_train_entries_by_key(std::vector<BimboEntry> &, std::unordered_map<std::vector<bool>, std::vector<int> > &, int, 
    void (*)(std::vector<bool> &, std::vector<unsigned int> &, const BimboEntry &));

std::string get_mean_priori_demand(std::vector<BimboEntry> &, std::vector<int> &, const BimboEntry &);

std::string get_lag25_extra(std::vector<BimboEntry> &, std::vector<int> &, const BimboEntry &);

void write_to_vec_train_grouped_entries(std::vector<std::string> &, std::vector<int> &, std::unordered_map<std::vector<bool>, std::vector<int> > &, 
    std::vector<BimboEntry> &, std::string (*)(std::vector<BimboEntry> &, std::vector<int> &, const BimboEntry &), int, 
    void (*)(std::vector<bool> &, std::vector<unsigned int> &, const BimboEntry &), bool base_included = false);

void write_to_file_train_grouped_entries(std::ofstream &, std::string &, std::vector<int> &, std::unordered_map<std::vector<bool>, std::vector<int> > &, 
    std::vector<BimboEntry> &, std::string (*)(std::vector<BimboEntry> &, std::vector<int> &, const BimboEntry &), int, 
    void (*)(std::vector<bool> &, std::vector<unsigned int> &, const BimboEntry &), bool base_included = false);

void append_to_file_train_grouped_entries(std::ifstream &, std::ofstream &, std::string &, std::unordered_map<std::vector<bool>, std::vector<int> > &, 
    std::vector<BimboEntry> &, std::string (*)(std::vector<BimboEntry> &, std::vector<int> &, const BimboEntry &), int, 
    void (*)(std::vector<bool> &, std::vector<unsigned int> &, const BimboEntry &), bool is_train = false);

void select_train_entries_by_semana(std::vector<BimboEntry> &, std::pair<unsigned int, unsigned int>, std::vector<int> &);

#endif