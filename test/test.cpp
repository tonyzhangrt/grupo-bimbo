#include "my_utils.h"
#include "bimbo_entry.h"
#include "entry_week_sketcher.h"
#include "bimbo_red_entry.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

void check_train_test_file();
void generate_train_test_file();

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
    ifstream f_train ("../input/train.csv");
    ifstream f_test ("../input/test.csv");
    
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