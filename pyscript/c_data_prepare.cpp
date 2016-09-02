#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <utility>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <bitset>
#include <ctime>
#include <queue>
#include <istream>
#include <algorithm>
#include <cmath>
using namespace std;

string get_timestamp(){
    char buff[15];
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    sprintf(buff, "%4d%02d%02d%02d%02d%02d", 1900 + timeinfo->tm_year, 1+timeinfo->tm_mon, 
        timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    string timestamp(buff);
    return timestamp;
}

void split_line(const string &line, char delim, vector<string> &result){
    istringstream iss(line);
    string word;
    while (getline(iss, word, delim)){
        result.push_back(word);
    }
}

class BimboEntry{
public:
    unsigned int semana, depot_id, channel_id, route_id, client_id, product_id;
    int venta_uni_hoy, dev_uni_proxima, demanda_uni_equil;
    double venta_hoy, dev_proxima;

    BimboEntry (unsigned int input_semana, unsigned int input_depot_id, unsigned int input_channel_id, 
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
};


struct entry_cmp{
    bool operator()(const BimboEntry &a, const BimboEntry &b ){
        return a.semana < b.semana;
    }
};

class BimboReducedEntry{
public:
    unsigned int semana, depot_id, channel_id, route_id;
    int demanda_uni_equil;

    BimboReducedEntry (unsigned int input_semana, unsigned int input_depot_id, unsigned int input_channel_id, 
        unsigned int input_route_id, int input_demanda_uni_equil){
        semana = input_semana;
        depot_id = input_depot_id;
        channel_id = input_channel_id;
        route_id = input_route_id;
        demanda_uni_equil = input_demanda_uni_equil;
    }
};

struct reduced_entry_cmp{
    bool operator()(const BimboReducedEntry &a, const BimboReducedEntry &b ){
        return a.semana < b.semana;
    }
}my_reduced_entry_cmp;

class BimboClientProductInfo{
public:
    unsigned int client_id, product_id;
    vector<BimboReducedEntry> entries;

    BimboClientProductInfo(){

    }

    BimboClientProductInfo(unsigned int input_client_id, unsigned int input_product_id){
        client_id = input_client_id;
        product_id = input_product_id;
    }
};


void generate_client_product_count(ifstream &f_train, unordered_map<unsigned long, vector<int> > &client_product_count, 
    bool require_positive = false){
    string line;
    getline(f_train, line);
    while ( getline(f_train, line) ){
        vector<string> tokens;
        split_line(line, ',', tokens);

        unsigned int semana, depot_id, channel_id, route_id, client_id, product_id;
        int venta_uni_hoy;

        semana = stoi(tokens[0]);
        depot_id = stoi(tokens[1]);
        channel_id = stoi(tokens[2]);
        route_id = stoi(tokens[3]);
        client_id = stoi(tokens[4]);
        product_id = stoi(tokens[5]);       
        venta_uni_hoy = stoi(tokens[6]);

        if (require_positive && venta_uni_hoy <= 0)
            continue;


        unsigned long hash;
        hash = product_id;
        hash = (hash << 32) | client_id; 

        if (client_product_count.count(hash)){
            vector<int> &cur_count = client_product_count[hash];
            cur_count[semana-3]++;
            cur_count[7]++;
        }else{
            client_product_count[hash] = vector<int>(10, 0);
            client_product_count[hash][8] = product_id;
            client_product_count[hash][9] = client_id;
            client_product_count[hash][semana-3]++;
            client_product_count[hash][7]++;
        }
    }
    f_train.clear();
    f_train.seekg(0, ios_base::beg);
}

void process_client_product_count(unordered_map<unsigned long, vector<int> > &client_product_count){
    long num_client_product_pair, regular_client_product_pair, extra_client_product_pair, irregular_client_product_pair;
    num_client_product_pair = client_product_count.size();
    regular_client_product_pair = 0;
    extra_client_product_pair = 0;
    irregular_client_product_pair = 0;

    double avg_total_order = 0;


    for (auto it = client_product_count.begin(); it != client_product_count.end(); it++){
        vector<int> &cur_count = it->second;
        bool is_regular = true;

        for (int i = 0; i < 7; i++){
            if (cur_count[i] == 0){
                is_regular = false;
                break;
            }
        }

        avg_total_order += cur_count[7];

        if (!is_regular){
            irregular_client_product_pair++;
            continue;
        }

        regular_client_product_pair++;

        if (cur_count[7] > 7){
            extra_client_product_pair++;
        }
    }

    avg_total_order /= num_client_product_pair;

    cout << num_client_product_pair << endl;
    cout << irregular_client_product_pair << endl;
    cout << regular_client_product_pair << endl;
    cout << extra_client_product_pair << endl;
    cout << avg_total_order << endl;
}

void check_client_product_pair(ifstream &f_train, pair<int, int> target_semana, bool is_train = true,
    bool require_positive = false){
    string line;
    getline(f_train, line);

    unordered_set<long> client_product_hash;
    unordered_set<long> client_product_hash1;
    unordered_set<long> client_product_hash2;
    while ( getline(f_train, line) ){
        vector<string> tokens;
        split_line(line, ',', tokens);

        unsigned int semana, depot_id, channel_id, route_id, client_id, product_id;
        int venta_uni_hoy;


        if (is_train){
            semana = stoi(tokens[0]);
            depot_id = stoi(tokens[1]);
            channel_id = stoi(tokens[2]);
            route_id = stoi(tokens[3]);
            client_id = stoi(tokens[4]);
            product_id = stoi(tokens[5]);
            venta_uni_hoy = stoi(tokens[6]);        
        }else{
            semana = stoi(tokens[1]);
            depot_id = stoi(tokens[2]);
            channel_id = stoi(tokens[3]);
            route_id = stoi(tokens[4]);
            client_id = stoi(tokens[5]);
            product_id = stoi(tokens[6]);
            venta_uni_hoy = 1;
        }

        if (require_positive && venta_uni_hoy <= 0)
            continue;

        if (semana != target_semana.first && semana != target_semana.second)
            continue;

        unsigned long hash;
        hash = product_id;
        hash = (hash << 32) | client_id;     

        client_product_hash.insert(hash);

        if (target_semana.first == semana){
            client_product_hash1.insert(hash);
        }else if (target_semana.second == semana){
            client_product_hash2.insert(hash);
        }
    }

    cout << target_semana.first << ": " << client_product_hash1.size() << endl;
    cout << target_semana.second << ": " << client_product_hash2.size() << endl;
    cout << client_product_hash.size() << endl;

    f_train.clear();
    f_train.seekg(0, ios_base::beg);


}


void generate_client_count(ifstream &f_train, unordered_map<unsigned int, unordered_set<int> > &client_count, 
    unordered_set<int> &target_semana, bool is_train = true){
    string line;
    getline(f_train, line);
    while ( getline(f_train, line) ){
        vector<string> tokens;
        split_line(line, ',', tokens);

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

        if (!target_semana.count(semana))
            continue;

        client_count[client_id].insert(product_id);
    }
    f_train.clear();
    f_train.seekg(0, ios_base::beg);
}

void process_client_count(unordered_map<unsigned int, unordered_set<int> > &client_count){
    int num_client;
    num_client = client_count.size();

    double avg_product;

    for (auto it = client_count.begin(); it != client_count.end(); it++){
        avg_product += (it->second).size();
    }

    avg_product /= num_client;

    cout << num_client << ": " << avg_product << endl;
}

void generate_train_client_product_info(ifstream &f_train, unordered_map<unsigned long, BimboClientProductInfo> &client_product_info, bool require_positive = false){
    string line;
    getline(f_train, line);
    while ( getline(f_train, line) ){
        vector<string> tokens;
        split_line(line, ',', tokens);

        unsigned int semana, depot_id, channel_id, route_id, client_id, product_id;
        int venta_uni_hoy, dev_uni_proxima, demanda_uni_equil;
        double venta_hoy, dev_proxima;

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

        if (require_positive && venta_uni_hoy <= 0)
            continue;


        unsigned long hash;
        hash = product_id;
        hash = (hash << 32) | client_id; 

        if (client_product_info.count(hash)){
            BimboClientProductInfo &cur_info = client_product_info[hash];
            cur_info.entries.push_back(BimboReducedEntry(semana, depot_id, channel_id, route_id, demanda_uni_equil));
        }else{
            client_product_info[hash] = BimboClientProductInfo(client_id, product_id);
            client_product_info[hash].entries.push_back(BimboReducedEntry(semana, depot_id, channel_id, route_id, demanda_uni_equil));
        }
    }
    f_train.clear();
    f_train.seekg(0, ios_base::beg);
}

void generate_basic_train_lag(ofstream &f_out, unordered_map<unsigned long, BimboClientProductInfo> &client_product_info, pair<int, int> &lag_range,
    set<unsigned int>& target_semana){
    string first_line;
    first_line = "Semana,Agencia_ID,Canal_ID,Ruta_SAK,Cliente_ID,Producto_ID";
    for (int i = lag_range.first; i <= lag_range.second; i++){
        first_line += ",lag" + to_string(i)+ "_demand";
    }
    first_line += ",demanda_uni_equil";
    f_out << first_line << endl;
    for (auto it = client_product_info.begin(); it != client_product_info.end(); it++){
        BimboClientProductInfo &cur_info = it->second;
        unsigned int client_id, product_id;
        client_id = cur_info.client_id;
        product_id = cur_info.product_id;
        vector<BimboReducedEntry> &entries = cur_info.entries;

        // sort(entries.begin(), entries.end(), my_reduced_entry_cmp);

        if (entries.back().semana < *(target_semana.begin()) )
            continue;


        vector<double> demand_by_semana(7, 0);
        vector<int> count_by_semana(7, 0);

        for (int i = 0; i < entries.size(); i++){
            int i_semana = int(entries[i].semana)-3;
            demand_by_semana[i_semana] += entries[i].demanda_uni_equil;
            count_by_semana[i_semana]++;

            if (target_semana.count(entries[i].semana)){
                string line;
                line = to_string(entries[i].semana) + "," + to_string(entries[i].depot_id) 
                        + "," + to_string(entries[i].channel_id) + "," + to_string(entries[i].route_id) 
                        + "," + to_string(client_id) + "," + to_string(product_id); 
                for (int k = lag_range.first; k <= lag_range.second; k++){
                    if (i_semana - k < 0 || count_by_semana[i_semana - k] == 0){
                        line += ",";
                    }else{
                        line += "," + to_string(demand_by_semana[i_semana - k] / count_by_semana[i_semana - k]);
                    }
                }
                line += "," + to_string(entries[i].demanda_uni_equil);
                f_out << line << endl;
            }
        }

    }


}


void generate_basic_test_lag(ofstream &f_out, ifstream &f_test, 
    unordered_map<unsigned long, BimboClientProductInfo> &client_product_info, pair<int, int> &lag_range){
    string first_line;
    first_line = "Semana,Agencia_ID,Canal_ID,Ruta_SAK,Cliente_ID,Producto_ID";
    for (int i = lag_range.first; i <= lag_range.second; i++){
        first_line += ",lag" + to_string(i)+ "_demand";
    }
    f_out << first_line << endl;

    string input_line;
    getline(f_test, input_line);
    while ( getline(f_test, input_line) ){
        vector<string> tokens;
        split_line(input_line, ',', tokens);

        unsigned int semana, depot_id, channel_id, route_id, client_id, product_id, id;

        semana = stoi(tokens[1]);
        depot_id = stoi(tokens[2]);
        channel_id = stoi(tokens[3]);
        route_id = stoi(tokens[4]);
        client_id = stoi(tokens[5]);
        product_id = stoi(tokens[6]);
        id = stoi(tokens[0]);

        unsigned long hash;
        hash = product_id;
        hash = (hash << 32) | client_id;

        BimboClientProductInfo &cur_info = client_product_info[hash];
        vector<BimboReducedEntry> &entries = cur_info.entries;

        vector<double> demand_by_semana(8, 0);
        vector<int> count_by_semana(8, 0);

        for (int i = 0; i < entries.size(); i++){
            int i_semana = int(entries[i].semana)-3;
            demand_by_semana[i_semana] += entries[i].demanda_uni_equil;
            count_by_semana[i_semana]++;
        }

        string line;
        line = to_string(id) + "," + to_string(semana) + "," + to_string(depot_id) 
                + "," + to_string(channel_id) + "," + to_string(route_id) 
                + "," + to_string(client_id) + "," + to_string(product_id);

        int a_semana = int(semana) - 3;                 
        for (int k = lag_range.first; k <= lag_range.second; k++){
            if (a_semana - k < 0 || a_semana - k >= 7 || count_by_semana[a_semana - k] == 0){
                line += ",";
            }else{
                line += "," + to_string(demand_by_semana[a_semana - k] / count_by_semana[a_semana - k]);
            }
        }
        f_out << line << endl;
    }


}

void generate_entries_from_train(ifstream &f_train, vector<BimboEntry> &train_entries, bool is_train = true, bool is_append = false){
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
    void (*get_hash_func)(vector<bool> &, vector<unsigned int> &, const BimboEntry &), bool base_included = false){

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
    void (*get_hash_func)(vector<bool> &, vector<unsigned int> &, const BimboEntry &), bool base_included = false){

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
    void (*get_hash_func)(vector<bool> &, vector<unsigned int> &, const BimboEntry &), bool is_train = false){

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

class EntryWeekSketcher{
public:
    unordered_map<vector<bool>, vector<int> > count_demand_by_semana_map;
    unordered_map<vector<bool>, vector<double> > sum_demand_by_semana_map;
    unordered_map<vector<bool>, vector<double> > logsum_demand_by_semana_map;

    EntryWeekSketcher(){
    
    }
    
    void generate_group_info(vector<BimboEntry> &train_entries, unordered_map<vector<bool>, vector<int> > &key_entry_map){
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

};

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
    void (*get_hash_func)(vector<bool> &, vector<unsigned int> &, const BimboEntry &), bool is_train = false){

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


void select_train_entries_by_semana(vector<BimboEntry> &train_entries, pair<unsigned int, unsigned int> semana_range, vector<int> &target_entries){
    target_entries.clear();
    for (int i = 0; i < train_entries.size(); i++){
        if (train_entries[i].semana >= semana_range.first && train_entries[i].semana <= semana_range.second){
            target_entries.push_back(i);
        }
    }
}

void check_train_test_file(){
    ifstream f_train ("../input/train.csv");
    ifstream f_test ("../input/test.csv");

    unordered_map<unsigned long, vector<int> > client_product_count;

    generate_client_product_count(f_train, client_product_count, true);

    process_client_product_count(client_product_count);

    unordered_map<unsigned int, unordered_set<int> > train_client_count;
    unordered_map<unsigned int, unordered_set<int> > test_client_count;

    unordered_map<unsigned int, unordered_set<int> > train_test_client_count;

    unordered_set<int> train_target_semana({8,9});
    unordered_set<int> test_target_semana({11});

    generate_client_count(f_train, train_test_client_count, train_target_semana, true);

    process_client_count(train_test_client_count);

    generate_client_count(f_test, train_test_client_count, test_target_semana, false);

    process_client_count(train_test_client_count);

    pair<int, int> train_semana_pair = pair<int, int>(8, 9);
    pair<int, int> test_semana_pair = pair<int, int>(10, 11);

    check_client_product_pair(f_train, pair<int, int>(5, 6), true, true);
    check_client_product_pair(f_train, pair<int, int>(6, 7), true, true);
    check_client_product_pair(f_train, pair<int, int>(7, 8), true, true);
    check_client_product_pair(f_train, pair<int, int>(8, 9), true, true);

    check_client_product_pair(f_test, test_semana_pair, false, true);
}


void generate_train_test_file(){
    vector<BimboEntry> train_entries;
    unordered_map<vector<bool>, vector<int> > key_entry_map;
    string header;
    int num_keys;
    bool is_train;

    generate_entries_from_train(f_train, train_entries);


    /* group by client_product begin */
    num_keys = 2;
    group_train_entries_by_key(train_entries, key_entry_map, num_keys, get_client_product_hash);


    vector<int> train_target_entries;
    select_train_entries_by_semana(train_entries, pair<unsigned int, unsigned int>(8, 9), train_target_entries);


    ofstream f_out_lag25_89("../input/train_lag25_extra_89.csv");

    header = "Semana,Agencia_ID,Canal_ID,Ruta_SAK,Cliente_ID,Producto_ID,Demanda_uni_equil";
    for (int j = 2; j <= 5; j++){header += ",lag" + to_string(j) + "_mean_demand";}
    header += ",mean_week_count_prior_demand,mean_week_sum_prior_demand,mean_week_logsum_prior_demand, total_lag_sum";
    
    bool include_base = true;
    write_to_file_train_grouped_entries(f_out_lag25_89, header, train_target_entries, key_entry_map, train_entries, get_lag25_extra, 
        num_keys, get_client_product_hash, include_base);

    f_out_lag25_89.close();


    ofstream f_out_lag25_1011("../input/test_lag25_extra_1011.csv");

    is_train = false;
    header = "";
    for (int j = 2; j <= 5; j++){
        if (j > 2){header += ",";}
        header += "lag" + to_string(j) + "_mean_demand";
    }
    header += ",mean_week_count_prior_demand,mean_week_sum_prior_demand,mean_week_logsum_prior_demand,total_lag_sum";
    append_to_file_train_grouped_entries(f_test, f_out_lag25_1011, header, key_entry_map, train_entries, get_lag25_extra, 
        num_keys, get_client_product_hash, is_train);

    f_out_lag25_1011.close();

    /* group by client_product end */
    
    /* group by route_product start */
    num_keys = 2;
    group_train_entries_by_key(train_entries, key_entry_map, num_keys, get_route_product_hash);

    EntryWeekSketcher entry_week_info;

    entry_week_info.generate_group_info(train_entries, key_entry_map);

    ifstream f_in_lag25_89("../input/train_lag25_extra_89.csv");

    ofstream f_out_lag25_route_89("../input/train_lag25_route_extra_89.csv");

    is_train = true;
    header = "";
    for (int j = 2; j <= 5; j++){
        if (j > 2){header += ",";}
        header += "route_lag" + to_string(j) + "_mean_demand";
    }
    header += ",route_mean_week_count_prior_demand,route_mean_week_sum_prior_demand,route_mean_week_logsum_prior_demand,route_total_lag_sum";
    append_to_file_train_grouped_entries_from_sketcher(f_in_lag25_89, f_out_lag25_route_89, header, entry_week_info, 
        get_lag25_extra_from_sketcher, num_keys, get_route_product_hash, is_train);


    f_out_lag25_route_89.close();

    ofstream f_out_lag25_route_nl_89("../input/train_lag25_route_nl_extra_89.csv");

    is_train = true;
    header = "";
    header += "route_mean_week_count_prior_demand,route_mean_week_sum_prior_demand,route_mean_week_logsum_prior_demand,route_total_lag_sum";
    append_to_file_train_grouped_entries_from_sketcher(f_in_lag25_89, f_out_lag25_route_nl_89, header, entry_week_info, 
        get_meancount_extra_from_sketcher, num_keys, get_route_product_hash, is_train);

    f_out_lag25_route_nl_89.close();    


    ifstream f_in_lag25_1011("../input/test_lag25_extra_1011.csv");

    ofstream f_out_lag25_route_1011("../input/test_lag25_route_extra_1011.csv");

    is_train = false;
    header = "";
    for (int j = 2; j <= 5; j++){
        if (j > 2){header += ",";}
        header += "route_lag" + to_string(j) + "_mean_demand";
    }
    header += ",route_mean_week_count_prior_demand,route_mean_week_sum_prior_demand,route_mean_week_logsum_prior_demand,route_total_lag_sum";
    append_to_file_train_grouped_entries_from_sketcher(f_in_lag25_1011, f_out_lag25_route_1011, header, entry_week_info, 
        get_lag25_extra_from_sketcher, num_keys, get_route_product_hash, is_train);

    f_out_lag25_route_1011.close();


    ofstream f_out_lag25_route_nl_1011("../input/test_lag25_route_nl_extra_1011.csv");

    is_train = false;
    header = "";
    header += "route_mean_week_count_prior_demand,route_mean_week_sum_prior_demand,route_mean_week_logsum_prior_demand,route_total_lag_sum";
    append_to_file_train_grouped_entries_from_sketcher(f_in_lag25_1011, f_out_lag25_route_nl_1011, header, entry_week_info, 
        get_meancount_extra_from_sketcher, num_keys, get_route_product_hash, is_train);

    f_out_lag25_route_nl_1011.close();    

    /* group by route_product end */

    /* generation of information related to only one field */

    cout << "f_train done" << endl;
    cout << train_entries.size() << endl;

    generate_entries_from_train(f_test, train_entries, false, true);

    cout << "f_test done" << endl;
    cout << train_entries.size() << endl;


    num_keys = 1;
    group_train_entries_by_key(train_entries, key_entry_map, num_keys, get_client_hash);

    ofstream f_client_out("../input/client_sketch.csv");
    header =  "client_id,n_client_id,client_depot_count,client_route_count,client_product_count";

    for (int i = 0; i < 7; i++){header += ",client_demand_semana" + to_string(i+3);}
    for (int i = 0; i < 7; i++){header += ",client_sum_demand_semana" + to_string(i+3);}
    for (int i = 0; i < 7; i++){header += ",client_logsum_demand_semana" + to_string(i+3);}

    generate_key_data_info(f_client_out, header, train_entries, key_entry_map, client_sketch);


    num_keys = 1;
    group_train_entries_by_key(train_entries, key_entry_map, num_keys, get_product_hash);

    ofstream f_product_out("../input/product_sketch.csv");
    header =  "product_id,n_product_id,product_depot_count,product_route_count,product_client_count";

    for (int i = 0; i < 7; i++){header += ",product_demand_semana" + to_string(i+3);}
    for (int i = 0; i < 7; i++){header += ",product_sum_demand_semana" + to_string(i+3);}
    for (int i = 0; i < 7; i++){header += ",product_logsum_demand_semana" + to_string(i+3);}

    generate_key_data_info(f_product_out, header, train_entries, key_entry_map, product_sketch);


    num_keys = 1;
    group_train_entries_by_key(train_entries, key_entry_map, num_keys, get_depot_hash);

    ofstream f_depot_out("../input/depot_sketch.csv");
    header =  "depot_id,n_depot_id,depot_route_count,depot_client_count,depot_product_count";

    for (int i = 0; i < 7; i++){header += ",depot_demand_semana" + to_string(i+3);}
    for (int i = 0; i < 7; i++){header += ",depot_sum_demand_semana" + to_string(i+3);}
    for (int i = 0; i < 7; i++){header += ",depot_logsum_demand_semana" + to_string(i+3);}

    generate_key_data_info(f_depot_out, header, train_entries, key_entry_map, depot_sketch);


    num_keys = 1;
    group_train_entries_by_key(train_entries, key_entry_map, num_keys, get_route_hash);

    ofstream f_route_out("../input/route_sketch.csv");
    header =  "route_id,n_route_id,route_depot_count,route_client_count,route_product_count";

    for (int i = 0; i < 7; i++){header += ",route_demand_semana" + to_string(i+3);}
    for (int i = 0; i < 7; i++){header += ",route_sum_demand_semana" + to_string(i+3);}
    for (int i = 0; i < 7; i++){header += ",route_logsum_demand_semana" + to_string(i+3);}

    generate_key_data_info(f_route_out, header, train_entries, key_entry_map, route_sketch);
}


int main(){
    generate_train_test_file();
}
