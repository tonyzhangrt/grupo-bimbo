#include "bimbo_red_entry.h"
#include "my_utils.h"
#include <iostream>
#include <string>
using namespace std;

BimboReducedEntry::BimboReducedEntry (unsigned int input_semana, unsigned int input_depot_id, unsigned int input_channel_id, 
    unsigned int input_route_id, int input_demanda_uni_equil){
    semana = input_semana;
    depot_id = input_depot_id;
    channel_id = input_channel_id;
    route_id = input_route_id;
    demanda_uni_equil = input_demanda_uni_equil;
}

bool reduced_entry_cmp::operator()(const BimboReducedEntry &a, const BimboReducedEntry &b ){
    return a.semana < b.semana;
}

BimboClientProductInfo::BimboClientProductInfo(unsigned int input_client_id, unsigned int input_product_id){
    client_id = input_client_id;
    product_id = input_product_id;
}


void generate_client_product_count(ifstream &f_train, unordered_map<unsigned long, vector<int> > &client_product_count, 
    bool require_positive){
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

void check_client_product_pair(ifstream &f_train, pair<int, int> target_semana, bool is_train,
    bool require_positive){
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
    unordered_set<int> &target_semana, bool is_train){
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

void generate_train_client_product_info(ifstream &f_train, unordered_map<unsigned long, BimboClientProductInfo> &client_product_info, 
    bool require_positive){
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