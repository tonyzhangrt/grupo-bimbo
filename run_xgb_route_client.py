import pandas as pd
import numpy as np
import xgboost as xgb
import ml_metrics as metrics
import datetime
import multiprocessing
from sklearn.cross_validation import train_test_split
import os.path
import gc

NUM_CORES = multiprocessing.cpu_count()

timestamp =  datetime.datetime.now().strftime('%Y%m%d%H%M%S')

TRAIN_FILE = '../input/train_lag25_route_extra_89.clientproductdepotroute.clientlag.csv'
TEST_FILE = '../input/test_lag25_route_extra_1011.clientproductdepotroute.clientlag.csv'

TRAIN_BUFFER = '../buffer/train_lag25_route_extra_89.clientproductdepotroute.clientlag.csv.buffer'
TEST_BUFFER = '../buffer/test_lag25_route_extra_1011.clientproductdepotroute.clientlag.csv.buffer'

SAMPLE_SUB = '../output/submission20160829112145.csv'

INITIAL_MODEL_FILE = '../model/lag25_route_client_extra.model'

def run_xgb():
    if not os.path.isfile(TRAIN_BUFFER):
        df_train = pd.read_csv(TRAIN_FILE)

        print "read train df done"

        print df_train.columns

        y_prep_train = df_train['Demanda_uni_equil'].values

        y_train = np.log1p(y_prep_train)

        X_train = df_train.drop(['Demanda_uni_equil'], axis = 1).values

        del df_train
        gc.collect()

        xg_train = xgb.DMatrix(X_train, label = y_train, missing = np.nan)

        del X_train, y_train

        xg_train.save_binary(TRAIN_BUFFER)

        print "buffer saved to: " + TRAIN_BUFFER

        del xg_train

        gc.collect()


    if not os.path.isfile(TEST_BUFFER):
        df_test = pd.read_csv(TEST_FILE)

        print "read test df done"

        X_test = df_test.drop(['id'], axis = 1).values

        del df_test
        gc.collect()

        xg_test = xgb.DMatrix(X_test, missing = np.nan)

        del X_test

        xg_test.save_binary(TEST_BUFFER)

        print "buffer saved to: " + TEST_BUFFER

        del xg_test

        gc.collect()

    df_pred = pd.read_csv(SAMPLE_SUB)

    param = dict()
    # use softmax multi-class classification
    param['objective'] = 'reg:linear'
    # scale weight of positive examples
    param['eta'] = 0.1
    param['subsample'] = 0.85
    param['colsample_bytree'] = 0.7
    # param['colsample_bylevel'] = 0.9
    param['max_depth'] = 10
    param['silent'] = 1
    param['nthread'] = NUM_CORES

    total_round = 1600

    num_round = 40

    cur_model_file = INITIAL_MODEL_FILE

    cur_round = 0
    while (cur_round < total_round):
        if cur_round >= 800:
            param['eta'] = 0.03
        elif cur_round >= 400:
            param['eta'] = 0.05


        xg_train = xgb.DMatrix(TRAIN_BUFFER)


        if os.path.isfile(cur_model_file):
            bst = xgb.train(param, xg_train, num_round, xgb_model = cur_model_file)

            print "model loaded from: " + cur_model_file
        else:
            bst = xgb.train(param, xg_train, num_round)
    # bst = xgb.train(param, xg_train, num_round, eval_metric='rmse', early_stopping_rounds = 10, verbose_eval = 5)

        cur_round += num_round

        del xg_train
        gc.collect()

        xg_test = xgb.DMatrix(TEST_BUFFER)

        y_pred = bst.predict(xg_test)

        del xg_test
        gc.collect()

        y_prep_pred = np.expm1(y_pred)

        df_pred['Demanda_uni_equil'] = y_prep_pred

        df_pred.to_csv('../output/sub'+timestamp+'.%s'%(cur_round)+'.csv', index = False) 
        
        cur_model_file = INITIAL_MODEL_FILE+'.%s'%(cur_round)

        bst.save_model(cur_model_file)


if __name__ == "__main__":
    run_xgb()    