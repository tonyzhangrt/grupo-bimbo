import pandas as pd
import numpy as np
import xgboost as xgb
import ml_metrics as metrics
import datetime
import multiprocessing
from sklearn.cross_validation import train_test_split
import os.path

NUM_CORES = multiprocessing.cpu_count()
SAMPLE_SUB = '../output/sample_submission.csv'


def make_pred(model_file, test_buffer_file):
    bst = xgb.Booster({'nthread':NUM_CORES}) #init model
    bst.load_model(model_file)

    xg_test = xgb.DMatrix(test_buffer_file)

    y_pred = bst.predict(xg_test)

    return y_pred

def make_avg_pred():
    model_folder = '../model/'
    buffer_folder = '../buffer/'

    # insert model file and buffer filename generated during training
    model_file_list = []

    test_buffer_file_list = []

    model_filepath_list = [model_folder+model_file for model_file in model_file_list]
    test_buffer_filepath_list = [buffer_folder+buffer_file for buffer_file in test_buffer_file_list]


    num_model = len(model_file_list)

    log_pred_list = list()

    df_pred = pd.read_csv(SAMPLE_SUB)


    for model_file, test_buffer_file in zip(model_filepath_list, test_buffer_filepath_list):
        df_pred['logpred_'+model_file] = make_pred(model_file, test_buffer_file)
        log_pred_list.append('logpred_'+model_file)


    df_pred['logpred_avg'] = df_pred[log_pred_list].mean(axis=1)

    logpred_avg = df_pred['logpred_avg'].values

    pred_logavg = np.expm1(logpred_avg)

    df_pred_logavg = pd.read_csv(SAMPLE_SUB)

    df_pred_logavg['Demanda_uni_equil'] = pred_logavg

    AVG_FILE_NAME = '../output/final_avg.csv'

    df_pred_logavg.to_csv(AVG_FILE_NAME, index = False)

if __name__ == "__main__":
    make_avg_pred()    






