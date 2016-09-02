#ifndef __ENTRY_WEEK_SKECTHER_H__
#define __ENTRY_WEEK_SKECTHER_H__

#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>
#include "bimbo_entry.h"

class EntryWeekSketcher{
public:
    std::unordered_map<std::vector<bool>, std::vector<int> > count_demand_by_semana_map;
    std::unordered_map<std::vector<bool>, std::vector<double> > sum_demand_by_semana_map;
    std::unordered_map<std::vector<bool>, std::vector<double> > logsum_demand_by_semana_map;
    
    void generate_group_info(std::vector<BimboEntry> &, std::unordered_map<std::vector<bool>, std::vector<int> > &);
};

std::string get_lag25_extra_from_sketcher(EntryWeekSketcher &, std::vector<bool> &, const BimboEntry &);

std::string get_meancount_extra_from_sketcher(EntryWeekSketcher &, std::vector<bool> &, const BimboEntry &);

void append_to_file_train_grouped_entries_from_sketcher(std::ifstream &, std::ofstream &, std::string &, EntryWeekSketcher &, 
    std::string (*)(EntryWeekSketcher &, std::vector<bool> &, const BimboEntry &), int, 
    void (*)(std::vector<bool> &, std::vector<unsigned int> &, const BimboEntry &), bool is_train = false);

void generate_key_data_info(std::ofstream &, std::string &, std::vector<BimboEntry> &, std::unordered_map<std::vector<bool>, std::vector<int> > &,
    std::string (*)(std::vector<BimboEntry> &, std::vector<int> &) );

std::string client_sketch(std::vector<BimboEntry> &, std::vector<int> &);

std::string depot_sketch(std::vector<BimboEntry> &, std::vector<int> &);

std::string route_sketch(std::vector<BimboEntry> &, std::vector<int> &);

std::string product_sketch(std::vector<BimboEntry> &, std::vector<int> &);

#endif