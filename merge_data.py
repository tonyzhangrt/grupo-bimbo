import os
import numpy as np
import pandas as pd
from sklearn.preprocessing import LabelEncoder
from scipy import stats, integrate
import os.path

ORIG_TRAIN_FILE = '../input/train_lag25_route_extra_89'
ORIG_TEST_FILE = '../input/test_lag25_route_extra_1011'

def create_client_mean_semana():
    df_all_client_info = pd.read_csv('../input/cliente_tabla.mod.extra.csv')

    df_client_demand_count_fields = ['client_demand_semana3', 'client_demand_semana4', 'client_demand_semana5', 'client_demand_semana6', 
    'client_demand_semana7', 'client_demand_semana8', 'client_demand_semana9']
    df_client_dmeand_sum_fields = ['client_sum_demand_semana3', 'client_sum_demand_semana4', 'client_sum_demand_semana5', 'client_sum_demand_semana6',
    'client_sum_demand_semana7', 'client_sum_demand_semana8', 'client_sum_demand_semana9']

    df_client_demand_mean_fields = ['client_mean_demand_semana3', 'client_mean_demand_semana4', 'client_mean_demand_semana5', 'client_mean_demand_semana6',
    'client_mean_demand_semana7', 'client_mean_demand_semana8', 'client_mean_demand_semana9']

    for i in range(7):
        demand_count = df_all_client_info[df_client_demand_count_fields[i]].values
        demand_sum = df_all_client_info[df_client_dmeand_sum_fields[i]].values

        length = demand_count.shape[0]

        demand_mean = np.empty(length)
        demand_mean[:] = np.nan

        for k in range(length):
            if demand_count[k] == 0:
                continue
            demand_mean[k] = float(demand_sum[k]) / demand_count[k]

        df_all_client_info[df_client_demand_mean_fields[i]] = demand_mean
    
    df_all_client_info.to_csv('../input/cliente_tabla.mod.extra.mean.csv', index = False)


def append_partial_df_by_key(filename, outfilename, key_type, file_key_field, df, df_key_field, df_append_fields):
    key_array = df[df_key_field].values
    
    unique_key, unique_key_ind = np.unique(key_array, return_index = True)
    
    key_to_ind_dict = dict(zip(unique_key, unique_key_ind))    
    
    with open (filename) as f:
        line = f.readline().strip('\n')        
        
        file_data_field =  [word.strip() for word in line.split(',')]
        file_data_pos_dict = dict(zip(file_data_field, range(len(file_data_field))))
        
        file_key_data_pos = file_data_pos_dict[file_key_field]
        
        f_out = open(outfilename, 'w')
        output_line = line
        for append_field in df_append_fields:
            output_line += "," + append_field
            
        f_out.write(output_line + "\n")
        
        while True:
            line = f.readline().strip('\n') 
            if not line:
                break
            
            data = [word.strip() for word in line.split(',')]
            key_data = key_type(data[file_key_data_pos])
            
            ind = key_to_ind_dict[key_data]
            
            output_line = line
            for append_field in df_append_fields:
                output_line += ",%s" % (df[append_field].values[ind])
            
            f_out.write(output_line + "\n")



def append_partial_df_lag_by_key_semana(filename, outfilename, key_type, file_key_field, df, df_key_field, df_append_fields_by_semana, lag_range_list, lag_name_list):
    if len(df_append_fields_by_semana) != 7:
        print "semana features to be appended not fit, aborted"
        return

    key_array = df[df_key_field].values
    
    unique_key, unique_key_ind = np.unique(key_array, return_index = True)
    
    key_to_ind_dict = dict(zip(unique_key, unique_key_ind))    
    
    with open (filename) as f:
        line = f.readline().strip('\n')        
        
        file_data_field =  [word.strip() for word in line.split(',')]
        file_data_pos_dict = dict(zip(file_data_field, range(len(file_data_field))))
        
        file_key_data_pos = file_data_pos_dict[file_key_field]
        file_semana_pos = file_data_pos_dict['Semana']

        f_out = open(outfilename, 'w')
        output_line = line
        for lag_name in lag_name_list:
            output_line += "," + lag_name
            
        f_out.write(output_line + "\n")
        
        while True:
            line = f.readline().strip('\n') 
            if not line:
                break
            
            data = [word.strip() for word in line.split(',')]
            key_data = key_type(data[file_key_data_pos])
            semana = int(data[file_semana_pos])

            ind = key_to_ind_dict[key_data]
            
            output_line = line
            for lag in lag_range_list:
                if (semana-lag-3) in range(7):
                    append_field = df_append_fields_by_semana[semana-lag-3]

                    if np.isnan(df[append_field].values[ind]):
                        output_line += ","
                    else:
                        output_line += ",%s" % (df[append_field].values[ind])
                else:
                    output_line += ","
            
            f_out.write(output_line + "\n")


def run_file_merge_client_lag():
    ORIG_TEST_FILE = '../input/test_lag25_route_nl_extra_1011.clientproductdepotroute'
    ORIG_TRAIN_FILE = '../input/train_lag25_route_nl_extra_89.clientproductdepotroute'

    test_file = ORIG_TEST_FILE+'.csv'
    train_file = ORIG_TRAIN_FILE+'.csv'

    test_client_lag_file = ORIG_TEST_FILE+'.clientlag.csv'
    train_client_lag_file = ORIG_TRAIN_FILE+'.clientlag.csv'

    if not os.path.isfile('../input/cliente_tabla.mod.extra.mean.csv'):
        create_client_mean_semana()

    df_all_client_info = pd.read_csv('../input/cliente_tabla.mod.extra.mean.csv')

    key_type = int
    file_key_field = 'Cliente_ID'
    df_key_field = 'Cliente_ID'

    df_client_demand_mean_fields = ['client_mean_demand_semana3', 'client_mean_demand_semana4', 'client_mean_demand_semana5', 'client_mean_demand_semana6',
    'client_mean_demand_semana7', 'client_mean_demand_semana8', 'client_mean_demand_semana9']

    lag_range_list = [2, 3, 4, 5]

    lag_name_list = ['client_lag2_mean_demand', 'client_lag3_mean_demand', 'client_lag4_mean_demand', 'client_lag5_mean_demand']

    append_partial_df_lag_by_key_semana(test_file, test_client_lag_file, key_type, file_key_field, df_all_client_info, df_key_field, 
        df_client_demand_mean_fields, lag_range_list, lag_name_list)

    append_partial_df_lag_by_key_semana(train_file, train_client_lag_file, key_type, file_key_field, df_all_client_info, df_key_field, 
    df_client_demand_mean_fields, lag_range_list, lag_name_list)

def run_file_merging():
    train_file = ORIG_TRAIN_FILE+'.csv'
    test_file = ORIG_TEST_FILE+'.csv'

    # merging information from client table
    train_client_file = ORIG_TRAIN_FILE+'.client.csv'
    test_client_file = ORIG_TEST_FILE+'.client.csv'

    df_all_client_info = pd.read_csv("../input/cliente_tabla.mod.extra.csv")

    key_type = int
    file_key_field = 'Cliente_ID'
    df_key_field = 'Cliente_ID'

    df_append_fields = ['short_name_id', 'n_client_id', 'client_depot_count','client_route_count','client_product_count']

    append_partial_df_by_key(train_file, train_client_file, key_type, file_key_field, df_all_client_info,
                             df_key_field, df_append_fields)
    append_partial_df_by_key(test_file, test_client_file, key_type, file_key_field, df_all_client_info,
                             df_key_field, df_append_fields)


    # product info merging
    train_product_file = ORIG_TRAIN_FILE+'.clientproduct.csv'
    test_product_file = ORIG_TEST_FILE+'.clientproduct.csv'

    df_all_product_info = pd.read_csv("../input/producto_tabla.mod.extra.csv")

    key_type = int
    file_key_field = 'Producto_ID'
    df_key_field = 'Producto_ID'

    df_append_fields = ['weight', 'pieces', 'tentative_category_id', 'weight_per_piece', 'brand_id', 'n_product_id']


    append_partial_df_by_key(train_client_file, train_product_file, key_type, file_key_field, df_all_product_info,
                             df_key_field, df_append_fields)
    append_partial_df_by_key(test_client_file, test_product_file, key_type, file_key_field, df_all_product_info,
                             df_key_field, df_append_fields)

    # depot info merging
    train_depot_file = ORIG_TRAIN_FILE + '.clientproductdepot.csv'
    test_depot_file = ORIG_TEST_FILE + '.clientproductdepot.csv'

    df_all_depot_info = pd.read_csv("../input/town_state.mod.extra.csv")

    key_type = int
    file_key_field = 'Agencia_ID'
    df_key_field = 'Agencia_ID'

    df_append_fields = ['n_depot_id','depot_route_count','depot_client_count','depot_product_count','town_client_count']


    append_partial_df_by_key(train_product_file, train_depot_file, key_type, file_key_field, df_all_depot_info,
                             df_key_field, df_append_fields)
    append_partial_df_by_key(test_product_file, test_depot_file, key_type, file_key_field, df_all_depot_info,
                             df_key_field, df_append_fields)

    # route info merging
    train_route_file = ORIG_TRAIN_FILE + '.clientproductdepotroute.csv'
    test_route_file = ORIG_TEST_FILE + '.clientproductdepotroute.csv'

    df_all_route_info = pd.read_csv("../input/route_sketch.csv")


    key_type = int
    file_key_field = 'Ruta_SAK'
    df_key_field = 'route_id'

    df_append_fields = ['n_route_id','route_depot_count','route_client_count','route_product_count']

    append_partial_df_by_key(train_depot_file, train_route_file, key_type, file_key_field, df_all_route_info,
                             df_key_field, df_append_fields)
    append_partial_df_by_key(test_depot_file, test_route_file, key_type, file_key_field, df_all_route_info,
                             df_key_field, df_append_fields)





if __name__ == "__main__":
    run_file_merging()
    run_file_merge_client_lag()