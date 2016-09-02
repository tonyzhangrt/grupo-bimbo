import pandas as pd 

df_town_state = pd.read_csv("../input/town_state.mod.csv")

towns = df_town_state['Town'].values

le = LabelEncoder()

le.fit(towns)

df_town_state['town_id'] = le.transform(towns)

states = df_town_state['State'].values
le = LabelEncoder()

le.fit(states)

df_town_state['state_id'] = le.transform(states)

df_town_state.to_csv("../input/town_state.mod.csv", index = False)

df_client = pd.read_csv("../input/cliente_tabla.mod.csv")
df_extra_client_info = pd.read_csv("../input/client_sketch.csv")

df_all_client_info = pd.merge(df_client, df_extra_client_info, how='left',on=['Cliente_ID'], 
      left_index=False, right_index=False, sort=True,
      suffixes=('_x', '_y'), copy=True, indicator=False)

df_all_client_info.to_csv("../input/cliente_tabla.mod.extra.csv", index = False)

df_product = pd.read_csv("../input/producto_tabla.mod.csv")
df_product_extra_info = pd.read_csv("../input/product_sketch.csv")

df_product_all = pd.merge(df_product, df_product_extra_info, how='outer',left_on=['Producto_ID'], 
                          right_on = ['product_id'],
      left_index=False, right_index=False, sort=True,
      suffixes=('_x', '_y'), copy=True, indicator=False)

df_product_all = df_product_all.drop(['product_id'], axis = 1)
df_product_all['weight_per_piece']  = df_product_all['weight'] /df_product_all['pieces']

brands = df_product_all['brand'].values
le = LabelEncoder()

le.fit(brands)

df_product_all['brand_id'] = le.transform(brands)

df_product_all.to_csv("../input/producto_tabla.mod.extra.csv", index = False)

df_depot = pd.read_csv("../input/town_state.mod.csv")
df_depot_extra_info = pd.read_csv("../input/depot_sketch.csv")

df_depot_all = pd.merge(df_depot, df_depot_extra_info, how='left',left_on=['Agencia_ID'], 
                          right_on = ['depot_id'],
      left_index=False, right_index=False, sort=True,
      suffixes=('_x', '_y'), copy=True, indicator=False)

df_depot_all = df_depot_all.drop(['depot_id'], axis = 1)

df_town_client_count = df_depot_all.groupby(['town_id'], as_index = False)['depot_client_count'].sum()
df_town_client_count = df_town_client_count.rename(columns = {'depot_client_count' : 'town_client_count'})
df_depot_all_town =  pd.merge(df_depot_all, df_town_client_count, how='left',on=['town_id'], 
      left_index=False, right_index=False, sort=False,
      suffixes=('_x', '_y'), copy=True, indicator=False)

df_depot_all_town.to_csv('../input/town_state.mod.extra.csv', index = False)


