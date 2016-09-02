#include "entry_week_sketcher.h"
#include "my_utils.h"
#include <cmath>
#include <unordered_set>
using namespace std;

void EntryWeekSketcher::generate_group_info(vector<BimboEntry> &train_entries, unordered_map<vector<bool>, vector<int> > &key_entry_map){
    count_demand_by_semana_map.clear();
    sum_demand_by_semana_map.clear();
    logsum_demand_by_semana_map.clear();

    for (auto it = key_entry_map.begin(); it != key_entry_map.end(); it++){
        const vector<bool> &cur_key = it->first;
        vector<int> &group_ids = it->second;

        vector<int> count_demand_by_semana(7, 0);
        vector<double> sum_demand_by_semana(7, 0);
        vector<double> logsum_demand_by_semana(7, 0);

        for (int i = 0; i < group_ids.size(); i++){
            int k = group_ids[i];

            int i_semana = int(train_entries[k].semana)-3;

            sum_demand_by_semana[i_semana] += train_entries[k].demanda_uni_equil;
            logsum_demand_by_semana[i_semana] += log1p(double(train_entries[k].demanda_uni_equil));
            count_demand_by_semana[i_semana]++;
        }

        count_demand_by_semana_map[cur_key] = count_demand_by_semana;
        sum_demand_by_semana_map[cur_key] = sum_demand_by_semana;
        logsum_demand_by_semana_map[cur_key] = logsum_demand_by_semana;
    }
}

string get_lag25_extra_from_sketcher(EntryWeekSketcher &entry_week_info, vector<bool> &cur_hash, const BimboEntry &target_entry){
    int lag_start, lag_end;
    lag_start = 2; lag_end = 5;

    string line;

    if ((entry_week_info.count_demand_by_semana_map).count(cur_hash)){
        vector<int> count_demand_by_semana = (entry_week_info.count_demand_by_semana_map)[cur_hash];
        vector<double> sum_demand_by_semana = (entry_week_info.sum_demand_by_semana_map)[cur_hash];
        vector<double> logsum_demand_by_semana = (entry_week_info.logsum_demand_by_semana_map)[cur_hash];

        int target_i_semana = int(target_entry.semana) - 3;

        double total_sum_demand, total_logsum_demand;
        int total_count_demand;

        total_sum_demand = 0; total_logsum_demand = 0; total_count_demand = 0;        
        for (int i = 0; i + lag_start <= target_i_semana; i++){
            total_count_demand += count_demand_by_semana[i];
            total_sum_demand += sum_demand_by_semana[i];
            total_logsum_demand += logsum_demand_by_semana[i];
        }


        double mean_sum_demand, mean_logsum_demand, mean_count_demand;
        mean_sum_demand = 0; mean_logsum_demand = 0; mean_count_demand = 0;
        int semana_count = target_i_semana - lag_start + 1;

        if (total_count_demand > 0){
            mean_sum_demand = total_sum_demand / semana_count;
            mean_logsum_demand = total_logsum_demand / semana_count;
            mean_count_demand = double(total_count_demand) / semana_count;
        }

        double total_lag_sum = 0;
        for (int j = lag_start; j <= lag_end; j++){
            if (j > lag_start){
                line += ",";
            }                
            if (target_i_semana - j >= 0 && target_i_semana - j < 7 && count_demand_by_semana[target_i_semana - j] != 0){
                line += to_string(sum_demand_by_semana[target_i_semana - j] / count_demand_by_semana[target_i_semana - j]);
                total_lag_sum += sum_demand_by_semana[target_i_semana - j] / count_demand_by_semana[target_i_semana - j];
            }
        }

        if (total_count_demand > 0){
            line += "," + to_string(mean_count_demand) + "," + to_string(mean_sum_demand) + "," + to_string(mean_logsum_demand) 
                    + "," + to_string(total_lag_sum);    
        }else{
            line += ",0,,," + to_string(total_lag_sum);
        }
    }else{
        for (int j = lag_start; j <= lag_end; j++){
            if (j > lag_start){
                line += ",";
            }                
        }
        line += ",0,,,0";
    }

    return line;
}


string get_meancount_extra_from_sketcher(EntryWeekSketcher &entry_week_info, vector<bool> &cur_hash, const BimboEntry &target_entry){
    int lag_start, lag_end;
    lag_start = 2; lag_end = 5;

    string line;

    if ((entry_week_info.count_demand_by_semana_map).count(cur_hash)){
        vector<int> count_demand_by_semana = (entry_week_info.count_demand_by_semana_map)[cur_hash];
        vector<double> sum_demand_by_semana = (entry_week_info.sum_demand_by_semana_map)[cur_hash];
        vector<double> logsum_demand_by_semana = (entry_week_info.logsum_demand_by_semana_map)[cur_hash];

        int target_i_semana = int(target_entry.semana) - 3;

        double total_sum_demand, total_logsum_demand;
        int total_count_demand;

        total_sum_demand = 0; total_logsum_demand = 0; total_count_demand = 0;        
        for (int i = 0; i + lag_start <= target_i_semana; i++){
            total_count_demand += count_demand_by_semana[i];
            total_sum_demand += sum_demand_by_semana[i];
            total_logsum_demand += logsum_demand_by_semana[i];
        }


        double mean_sum_demand, mean_logsum_demand, mean_count_demand;
        mean_sum_demand = 0; mean_logsum_demand = 0; mean_count_demand = 0;
        int semana_count = target_i_semana - lag_start + 1;

        if (total_count_demand > 0){
            mean_sum_demand = total_sum_demand / semana_count;
            mean_logsum_demand = total_logsum_demand / semana_count;
            mean_count_demand = double(total_count_demand) / semana_count;
        }

        double total_lag_sum = 0;
        for (int j = lag_start; j <= lag_end; j++){
            if (target_i_semana - j >= 0 && target_i_semana - j < 7 && count_demand_by_semana[target_i_semana - j] != 0){
                total_lag_sum += sum_demand_by_semana[target_i_semana - j] / count_demand_by_semana[target_i_semana - j];
            }
        }

        if (total_count_demand > 0){
            line += to_string(mean_count_demand) + "," + to_string(mean_sum_demand) + "," + to_string(mean_logsum_demand)
                    + "," + to_string(total_lag_sum);    
        }else{
            line += "0,,," + to_string(total_lag_sum);
        }
    }else{
        line += "0,,,0";
    }

    return line;
}



void append_to_file_train_grouped_entries_from_sketcher(ifstream &f_in, ofstream &f_out, string &header, EntryWeekSketcher &entry_week_info, 
    string (*process_func)(EntryWeekSketcher &, vector<bool> &, const BimboEntry &), int num_keys, 
    void (*get_hash_func)(vector<bool> &, vector<unsigned int> &, const BimboEntry &), bool is_train){

    vector<bool> hash_vec(num_keys*32);
    vector<unsigned int> key_vec(num_keys);

    string input_line;
    getline(f_in, input_line);

    string output_line;
    output_line = input_line + "," + header;
    f_out << output_line << endl;
            
    while ( getline(f_in, input_line) ){
        vector<string> tokens;
        split_line(input_line, ',', tokens);

        unsigned int semana, depot_id, channel_id, route_id, client_id, product_id;

        if (is_train){
            semana = stoi(tokens[0]);
            depot_id = stoi(tokens[1]);
            channel_id = stoi(tokens[2]);
            route_id = stoi(tokens[3]);
            client_id = stoi(tokens[4]);
            product_id = stoi(tokens[5]);       
        }else{
            semana = stoi(tokens[1]);
            depot_id = stoi(tokens[2]);
            channel_id = stoi(tokens[3]);
            route_id = stoi(tokens[4]);
            client_id = stoi(tokens[5]);
            product_id = stoi(tokens[6]);
        }

        BimboEntry cur_entry = BimboEntry (semana, depot_id, channel_id, route_id, client_id, product_id, 0, 0, 0, 0, 0);
        get_hash_func(hash_vec, key_vec, cur_entry);

        output_line = input_line + "," + process_func(entry_week_info, hash_vec, cur_entry);

        f_out << output_line << endl;

    }
    f_in.clear();
    f_in.seekg(0, ios_base::beg);
}



void generate_key_data_info(ofstream &f_out, string &header, vector<BimboEntry> &train_entries, unordered_map<vector<bool>, vector<int> > &key_entry_map,
    string (*key_process_func)(vector<BimboEntry> &, vector<int> &) ){
    f_out << header << endl;

    for (auto it = key_entry_map.begin(); it != key_entry_map.end(); ++it){
        vector<int> &local_entries = it->second;

        string line = key_process_func(train_entries, local_entries);

        f_out << line << endl;
    }
}

string client_sketch(vector<BimboEntry> &train_entries, vector<int> &group_ids){
    vector<double> sum_demand(7, 0);
    vector<double> log_sum_demand(7, 0);
    vector<unsigned int> demand_count(7, 0);

    unordered_set<unsigned int> depot_id_set;
    unordered_set<unsigned int> route_id_set;
    unordered_set<unsigned int> product_id_set;

    unsigned int client_id = train_entries[group_ids[0]].client_id;

    for (int i = 0; i < group_ids.size(); i++){
        int k = group_ids[i];
        int cur_semana = train_entries[k].semana;
        
        if (cur_semana <= 9){
            int i_semana = cur_semana - 3;

            sum_demand[i_semana] += train_entries[k].demanda_uni_equil;
            log_sum_demand[i_semana] += log1p(double(train_entries[k].demanda_uni_equil));
            demand_count[i_semana]++;
        }

        depot_id_set.insert(train_entries[k].depot_id);
        route_id_set.insert(train_entries[k].route_id);
        product_id_set.insert(train_entries[k].product_id);     
    }

    int num_depot_id, num_route_id, num_prodcut_id;
    num_depot_id = depot_id_set.size();
    num_route_id = route_id_set.size();
    num_prodcut_id = product_id_set.size();

    string line = to_string(client_id) + "," + to_string(group_ids.size()) + 
        "," + to_string(num_depot_id) + "," + to_string(num_route_id) + "," + to_string(num_prodcut_id);

    for (int i = 0; i < 7; i++){
        line += "," + to_string(demand_count[i]);
    }
    for (int i = 0; i < 7; i++){
        line += "," + to_string(sum_demand[i]);
    }
    for (int i = 0; i < 7; i++){
        line += "," + to_string(log_sum_demand[i]);
    }

    return line;
}

string depot_sketch(vector<BimboEntry> &train_entries, vector<int> &group_ids){
    vector<double> sum_demand(7, 0);
    vector<double> log_sum_demand(7, 0);
    vector<unsigned int> demand_count(7, 0);

    unordered_set<unsigned int> route_id_set;
    unordered_set<unsigned int> client_id_set;
    unordered_set<unsigned int> product_id_set;

    unsigned int depot_id = train_entries[group_ids[0]].depot_id;

    for (int i = 0; i < group_ids.size(); i++){
        int k = group_ids[i];
        
        int cur_semana = train_entries[k].semana;
        
        if (cur_semana <= 9){
            int i_semana = cur_semana - 3;

            sum_demand[i_semana] += train_entries[k].demanda_uni_equil;
            log_sum_demand[i_semana] += log1p(double(train_entries[k].demanda_uni_equil));
            demand_count[i_semana]++;
        }

        route_id_set.insert(train_entries[k].route_id);
        client_id_set.insert(train_entries[k].client_id);
        product_id_set.insert(train_entries[k].product_id);
    }

    int num_route_id, num_client_id, num_product_id;
    num_route_id = route_id_set.size();
    num_client_id = client_id_set.size();
    num_product_id = product_id_set.size();

    string line = to_string(depot_id) + "," + to_string(group_ids.size()) + 
        "," + to_string(num_route_id) + "," + to_string(num_client_id) + "," + to_string(num_product_id);

    for (int i = 0; i < 7; i++){
        line += "," + to_string(demand_count[i]);
    }
    for (int i = 0; i < 7; i++){
        line += "," + to_string(sum_demand[i]);
    }
    for (int i = 0; i < 7; i++){
        line += "," + to_string(log_sum_demand[i]);
    }

    return line;
}

string route_sketch(vector<BimboEntry> &train_entries, vector<int> &group_ids){
    vector<double> sum_demand(7, 0);
    vector<double> log_sum_demand(7, 0);
    vector<unsigned int> demand_count(7, 0);

    unordered_set<unsigned int> depot_id_set;
    unordered_set<unsigned int> client_id_set;
    unordered_set<unsigned int> product_id_set;

    unsigned int route_id = train_entries[group_ids[0]].route_id;

    for (int i = 0; i < group_ids.size(); i++){
        int k = group_ids[i];
        
        int cur_semana = train_entries[k].semana;
        
        if (cur_semana <= 9){
            int i_semana = cur_semana - 3;

            sum_demand[i_semana] += train_entries[k].demanda_uni_equil;
            log_sum_demand[i_semana] += log1p(double(train_entries[k].demanda_uni_equil));
            demand_count[i_semana]++;
        }

        depot_id_set.insert(train_entries[k].depot_id);
        client_id_set.insert(train_entries[k].client_id);
        product_id_set.insert(train_entries[k].product_id);
    }

    int num_depot_id, num_client_id, num_product_id;
    num_depot_id = depot_id_set.size();
    num_client_id = client_id_set.size();
    num_product_id = product_id_set.size();

    string line = to_string(route_id) + "," + to_string(group_ids.size()) + 
        "," + to_string(num_depot_id) + "," + to_string(num_client_id) + "," + to_string(num_product_id);

    for (int i = 0; i < 7; i++){
        line += "," + to_string(demand_count[i]);
    }
    for (int i = 0; i < 7; i++){
        line += "," + to_string(sum_demand[i]);
    }
    for (int i = 0; i < 7; i++){
        line += "," + to_string(log_sum_demand[i]);
    }

    return line;
}

string product_sketch(vector<BimboEntry> &train_entries, vector<int> &group_ids){
    vector<double> sum_demand(7, 0);
    vector<double> log_sum_demand(7, 0);
    vector<unsigned int> demand_count(7, 0);

    unordered_set<unsigned int> depot_id_set;
    unordered_set<unsigned int> route_id_set;
    unordered_set<unsigned int> client_id_set;

    unsigned int product_id = train_entries[group_ids[0]].product_id;

    for (int i = 0; i < group_ids.size(); i++){
        int k = group_ids[i];
        
        int cur_semana = train_entries[k].semana;
        
        if (cur_semana <= 9){
            int i_semana = cur_semana - 3;

            sum_demand[i_semana] += train_entries[k].demanda_uni_equil;
            log_sum_demand[i_semana] += log1p(double(train_entries[k].demanda_uni_equil));
            demand_count[i_semana]++;
        }

        depot_id_set.insert(train_entries[k].depot_id);
        route_id_set.insert(train_entries[k].route_id);
        client_id_set.insert(train_entries[k].client_id);
    }

    int num_depot_id, num_route_id, num_client_id;
    num_depot_id = depot_id_set.size();
    num_route_id = route_id_set.size();
    num_client_id = client_id_set.size();

    string line = to_string(product_id) + "," + to_string(group_ids.size()) + 
        "," + to_string(num_depot_id) + "," + to_string(num_route_id) + "," + to_string(num_client_id);

    for (int i = 0; i < 7; i++){
        line += "," + to_string(demand_count[i]);
    }
    for (int i = 0; i < 7; i++){
        line += "," + to_string(sum_demand[i]);
    }
    for (int i = 0; i < 7; i++){
        line += "," + to_string(log_sum_demand[i]);
    }

    return line;
}