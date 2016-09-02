#include "bimbo_entry.h"
#include "my_utils.h"
#include <cmath>
using namespace std;

BimboEntry::BimboEntry (unsigned int input_semana, unsigned int input_depot_id, unsigned int input_channel_id, 
    unsigned int input_route_id, unsigned int input_client_id, unsigned int input_product_id,
    int input_venta_uni_hoy, double input_venta_hoy, int input_dev_uni_proxima, double input_dev_proxima, int input_demanda_uni_equil){
    semana = input_semana;
    depot_id = input_depot_id;
    channel_id = input_channel_id;
    route_id = input_route_id;
    client_id = input_client_id;
    product_id = input_product_id;
    venta_uni_hoy = input_venta_uni_hoy;
    venta_hoy = input_venta_hoy;
    dev_uni_proxima = input_dev_uni_proxima;
    dev_proxima = input_dev_proxima;
    demanda_uni_equil = input_demanda_uni_equil;
}

bool entry_cmp::operator()(const BimboEntry &a, const BimboEntry &b ){
        return a.semana < b.semana;
}

void generate_entries_from_train(ifstream &f_train, vector<BimboEntry> &train_entries, bool is_train, bool is_append){
    string line;
    getline(f_train, line);
    
    if (!is_append){
        train_entries.clear();
    }
        
    while ( getline(f_train, line) ){
        vector<string> tokens;
        split_line(line, ',', tokens);

        unsigned int semana, depot_id, channel_id, route_id, client_id, product_id;
        int venta_uni_hoy, dev_uni_proxima, demanda_uni_equil;
        double venta_hoy, dev_proxima;

        if (is_train){
            semana = stoi(tokens[0]);
            depot_id = stoi(tokens[1]);
            channel_id = stoi(tokens[2]);
            route_id = stoi(tokens[3]);
            client_id = stoi(tokens[4]);
            product_id = stoi(tokens[5]);       
            venta_uni_hoy = stoi(tokens[6]);
            venta_hoy = stod(tokens[7]);
            dev_uni_proxima = stoi(tokens[8]);
            dev_proxima = stod(tokens[9]);
            demanda_uni_equil = stoi(tokens[10]);
        }else{
            semana = stoi(tokens[1]);
            depot_id = stoi(tokens[2]);
            channel_id = stoi(tokens[3]);
            route_id = stoi(tokens[4]);
            client_id = stoi(tokens[5]);
            product_id = stoi(tokens[6]);       
            venta_uni_hoy = 0;
            venta_hoy = 0;
            dev_uni_proxima = 0;
            dev_proxima = 0;
            demanda_uni_equil = 0;
        }

        train_entries.push_back(BimboEntry (semana, depot_id, channel_id, route_id, client_id, product_id, 
            venta_uni_hoy, venta_hoy, dev_uni_proxima, dev_proxima, demanda_uni_equil));
    }
    f_train.clear();
    f_train.seekg(0, ios_base::beg);
}

void get_hash(vector<bool> &hash, vector<unsigned int> &key_entries){
    for (int i = 0; i < key_entries.size(); i++){
        for (int j = 0; j < 32; j++){
            hash[i*32+j] = (key_entries[i] >> j) & 1;
        }
    }
}

void get_client_hash(vector<bool> &hash, vector<unsigned int> &key_entries, const BimboEntry &entry){
    key_entries[0] = entry.client_id;
    get_hash(hash, key_entries);
}

void get_depot_hash(vector<bool> &hash, vector<unsigned int> &key_entries, const BimboEntry &entry){
    key_entries[0] = entry.depot_id;
    get_hash(hash, key_entries);
}

void get_route_hash(vector<bool> &hash, vector<unsigned int> &key_entries, const BimboEntry &entry){
    key_entries[0] = entry.route_id;
    get_hash(hash, key_entries);
}

void get_product_hash(vector<bool> &hash, vector<unsigned int> &key_entries, const BimboEntry &entry){
    key_entries[0] = entry.product_id;
    get_hash(hash, key_entries);
}

void get_client_product_hash(vector<bool> &hash, vector<unsigned int> &key_entries, const BimboEntry &entry){
    key_entries[0] = entry.client_id;
    key_entries[1] = entry.product_id;
    get_hash(hash, key_entries);
}

void get_route_product_hash(vector<bool> &hash, vector<unsigned int> &key_entries, const BimboEntry &entry){
    key_entries[0] = entry.route_id;
    key_entries[1] = entry.product_id;
    get_hash(hash, key_entries);
}

void group_train_entries_by_key(vector<BimboEntry> &train_entries, unordered_map<vector<bool>, vector<int> > &key_entry_map, int num_keys, 
    void (*get_hash_func)(vector<bool> &, vector<unsigned int> &, const BimboEntry &)){

    vector<bool> hash_vec(num_keys*32);
    vector<unsigned int> key_vec(num_keys);

    key_entry_map.clear();
    int num_entries = train_entries.size();

    for (int i = 0; i < num_entries; i++){
        get_hash_func(hash_vec, key_vec, train_entries[i]);

        key_entry_map[hash_vec].push_back(i);
    }
}

string get_mean_priori_demand(vector<BimboEntry> &train_entries, vector<int> &group_ids, const BimboEntry &target_entry){
    double mean_priori_demand = 0;
    int priori_count = 0;

    for (int i = 0; i < group_ids.size(); i++){
        int k = group_ids[i];
        if (train_entries[i].semana >= target_entry.semana)
            break;
        
        mean_priori_demand += train_entries[i].demanda_uni_equil;
        priori_count++;
    }

    if (priori_count == 0)
        return "";

    return to_string(mean_priori_demand / priori_count);
}

string get_lag25_extra(vector<BimboEntry> &train_entries, vector<int> &group_ids, const BimboEntry &target_entry){
    int lag_start, lag_end;
    lag_start = 2; lag_end = 5;


    vector<double> sum_demand_by_semana(7, 0);
    // vector<double> logsum_demand_by_semana(7, 0);
    vector<int> count_demand_by_semana(7, 0);
    double total_sum_demand, total_logsum_demand;
    int total_count_demand;

    total_sum_demand = 0; total_logsum_demand = 0; total_count_demand = 0;

    for (int i = 0; i < group_ids.size(); i++){
        int k = group_ids[i];

        if (train_entries[k].semana + lag_start > target_entry.semana)
            break;

        int i_semana = int(train_entries[k].semana)-3;

        sum_demand_by_semana[i_semana] += train_entries[k].demanda_uni_equil;
        // logsum_demand_by_semana[i_semana] += log1p(double(entries[k].demanda_uni_equil));
        count_demand_by_semana[i_semana]++;

        total_sum_demand += train_entries[k].demanda_uni_equil;
        total_logsum_demand += log1p(double(train_entries[k].demanda_uni_equil));
        total_count_demand++;
    }

    int target_i_semana = int(target_entry.semana) - 3;

    double mean_sum_demand, mean_logsum_demand, mean_count_demand;
    mean_sum_demand = 0; mean_logsum_demand = 0; mean_count_demand = 0;
    int semana_count = target_i_semana - lag_start + 1;

    if (total_count_demand > 0){
        mean_sum_demand = total_sum_demand / semana_count;
        mean_logsum_demand = total_logsum_demand / semana_count;
        mean_count_demand = double(total_count_demand) / semana_count;
    }


    string line;

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

    return line;
}


void write_to_vec_train_grouped_entries(vector<string> &out_put, vector<int> &target_entries, unordered_map<vector<bool>, vector<int> > &key_entry_map, 
    vector<BimboEntry> &train_entries, string (*process_func)(vector<BimboEntry> &, vector<int> &, const BimboEntry &), int num_keys, 
    void (*get_hash_func)(vector<bool> &, vector<unsigned int> &, const BimboEntry &), bool base_included){

    int num_targets = target_entries.size();

    vector<bool> hash_vec(num_keys*32);
    vector<unsigned int> key_vec(num_keys);

    for (int i = 0; i < num_targets; i++){
        int k = target_entries[i];
        BimboEntry &cur_entry = train_entries[k];
        get_hash_func(hash_vec, key_vec, cur_entry);
        vector<int> &local_entries = key_entry_map[hash_vec];

        string line;

        if (base_included){
            line = to_string(cur_entry.semana) + "," + to_string(cur_entry.depot_id) 
                + "," + to_string(cur_entry.channel_id) + "," + to_string(cur_entry.route_id) 
                + "," + to_string(cur_entry.client_id) + "," + to_string(cur_entry.product_id)
                + "," + to_string(cur_entry.demanda_uni_equil) + ",";
        }

        line += process_func(train_entries, local_entries, cur_entry);

        out_put[i] = line;
    }
}

void write_to_file_train_grouped_entries(ofstream &f_out, string &header, vector<int> &target_entries, unordered_map<vector<bool>, vector<int> > &key_entry_map, 
    vector<BimboEntry> &train_entries, string (*process_func)(vector<BimboEntry> &, vector<int> &, const BimboEntry &), int num_keys, 
    void (*get_hash_func)(vector<bool> &, vector<unsigned int> &, const BimboEntry &), bool base_included){

    f_out << header << endl;

    int num_targets = target_entries.size();

    vector<bool> hash_vec(num_keys*32);
    vector<unsigned int> key_vec(num_keys);

    for (int i = 0; i < num_targets; i++){
        int k = target_entries[i];
        BimboEntry &cur_entry = train_entries[k];
        get_hash_func(hash_vec, key_vec, cur_entry);
        vector<int> &local_entries = key_entry_map[hash_vec];

        string line;

        if (base_included){
            line = to_string(cur_entry.semana) + "," + to_string(cur_entry.depot_id) 
                + "," + to_string(cur_entry.channel_id) + "," + to_string(cur_entry.route_id) 
                + "," + to_string(cur_entry.client_id) + "," + to_string(cur_entry.product_id)
                + "," + to_string(cur_entry.demanda_uni_equil) + ",";
        }

        line += process_func(train_entries, local_entries, cur_entry);

        f_out << line << endl;
    }
}

void append_to_file_train_grouped_entries(ifstream &f_in, ofstream &f_out, string &header, unordered_map<vector<bool>, vector<int> > &key_entry_map, 
    vector<BimboEntry> &train_entries, string (*process_func)(vector<BimboEntry> &, vector<int> &, const BimboEntry &), int num_keys, 
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
        vector<int> &local_entries = key_entry_map[hash_vec];

        output_line = input_line + "," +process_func(train_entries, local_entries, cur_entry);

        if (local_entries.size() == 0)
            key_entry_map.erase(hash_vec);

        f_out << output_line << endl;

    }
    f_in.clear();
    f_in.seekg(0, ios_base::beg);
}

void select_train_entries_by_semana(vector<BimboEntry> &train_entries, pair<unsigned int, unsigned int> semana_range, vector<int> &target_entries){
    target_entries.clear();
    for (int i = 0; i < train_entries.size(); i++){
        if (train_entries[i].semana >= semana_range.first && train_entries[i].semana <= semana_range.second){
            target_entries.push_back(i);
        }
    }
}