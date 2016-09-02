import numpy as np
import pandas as pd
import re
from nltk.corpus import stopwords
from nltk.stem.snowball import SnowballStemmer

products  =  pd.read_csv("../input/producto_tabla.csv")

product_id_list = products['Producto_ID'].values
product_name_list = products['NombreProducto'].values

num_product = product_name_list.shape[0]


product_weight_list = np.empty(num_product)
product_weight_list[:] = np.NAN
product_piece_list = np.ones(num_product)
product_brand_list = list()
product_short_name_list = list()

product_info_list = zip(product_id_list, product_name_list)

regex_piece_weight = re.compile(r'(.*?)(\d+)(p|P)(?!ct).*?(\d+)\s*(kg|Kg|g|G|ml)')
regex_weight_piece = re.compile(r'(.*?)(\d+)\s*(kg|Kg|g|G|ml).*?(\d+)(p|P)(?!ct|CT)')
regex_weight = re.compile(r'(.*?)(\d+)\s*(kg|Kg|g|G|ml)')
regex_piece = re.compile(r'(.*?)(\d+)(p|P)(?!ct|CT)')

dont_know_list = ['MTB', 'MTA', 'TNB', 'CU', 'TAB', 'Prom']

for i, (product_id, orig_product_name) in enumerate(product_info_list):
    product_name = orig_product_name.rstrip(str(product_id)).strip()
    
    product_name_words = product_name.split()   
    
    if re.search(regex_piece_weight, product_name):
        regex_result = re.search(regex_piece_weight, product_name)
        product_weight_list[i] = float(regex_result.group(4))
        product_piece_list[i] = int(regex_result.group(2))
        product_short_name = regex_result.group(1)
        if regex_result.group(5) == 'kg' or regex_result.group(5) == 'Kg':
            product_weight_list[i] *= 1000
    elif re.search(regex_weight_piece, product_name):
        regex_result = re.search(regex_weight_piece, product_name)
        product_weight_list[i] = float(regex_result.group(2))
        product_piece_list[i] = int(regex_result.group(4))
        if regex_result.group(3) == 'kg' or regex_result.group(3) == 'Kg':
            product_weight_list[i] *= 1000
        product_short_name = regex_result.group(1)
    elif re.search(regex_weight, product_name):
        regex_result = re.search(regex_weight, product_name)
        product_weight_list[i] = float(regex_result.group(2))
        if regex_result.group(3) == 'kg' or regex_result.group(3) == 'Kg':
            product_weight_list[i] *= 1000
        product_short_name = regex_result.group(1)
    elif re.search(regex_piece, product_name):
        regex_result = re.search(regex_piece, product_name)
        product_piece_list[i] = int(regex_result.group(2))
        product_short_name = regex_result.group(1)        
    else:
        product_short_name = ' '.join([word for word in product_name_words[:-1] if word not in dont_know_list])
        
        
    
    product_brand_list.append(product_name_words[-1])
    product_short_name_list.append(product_short_name)

products['short_name'] = product_short_name_list
products['brand'] = product_brand_list
products['weight'] = product_weight_list
products['pieces'] = product_piece_list    

df_download_products  =  pd.read_csv("../input/download_product_name.csv")

category_lookup_list = map(lambda x: x.lower(), df_download_products['category'].values)
subcategory_lookup_list = map(lambda x: x.lower(), df_download_products['subcategory'].values)

products['short_name_processed'] = (products['short_name']
                                        .map(lambda x: " ".join([i for i in x.lower()
                                                                 .split() if i not in stopwords.words("spanish")])))

stemmer = SnowballStemmer("spanish")
products['short_name_processed'] = (products['short_name_processed']
                                        .map(lambda x: " ".join([stemmer.stem(i) for i in x.lower().split()])))

lower_product_short_name_list = map(lambda x: x.lower(), product_short_name_list)
lower_processed_product_short_name_list = map(lambda x:x.lower(), products['short_name_processed'])

product_name_lookup_list = map(lambda x: x.lower(), df_download_products['product_name'])
processed_product_name_lookup_list = map(lambda x: " ".join([i for i in x.lower()
                                                                 .split() if i not in stopwords.words("spanish")]),
                                        product_name_lookup_list)
processed_product_name_lookup_list = map(lambda x: " ".join([stemmer.stem(i) for i in x.lower().split()]), 
                                        processed_product_name_lookup_list)

processed_product_name_lookup_list = map(lambda x: " ".join([i for i in x.split() if not re.search(r'^\d+', i)]),
                                        processed_product_name_lookup_list)

grain_regex = re.compile(r'(grano|fibr|linaza|cero|cereal)')
tostado_regex = re.compile(r'(tost)')
flake_regex = re.compile(r'(crujiente|molid|empanizador)')
frozen_regex = re.compile(r'congelad')
lonchibon_regex = re.compile(r'(burritos|sand)')
pan_regex = re.compile(r'(pan\s|bread\s|pan$|bread$|dutch country)')
bollo_regex = re.compile(r'(bollo|hot dog|thins)')
tortilla_regex = re.compile(r'(tortilla|nacho)')
barras_regex = re.compile(r'(bran frut|bran.*fresa|bran.*pina|bran.*avena)')
pan_dulce_regex = re.compile(r'(role|panque|roll|chocorol|twinkies|bisquet|orejitas|bigotes|'\
                             'empanaditas|pay|pipucho|submarinos|navidena)')
pasteles_regex = re.compile(r'(pastel|muffin|beso|orejas|napolitano|rosca|tarta)')
galletas_regex = re.compile(r'(gallet|barr|suavicrema|kc$|deliciosas|palitrigos|saladas|chocochips|'\
                            'chocochispas|canapinas|decanelas|chispa|gansi premio|'\
                            'surtido|pastisetas|raztachoc|magnas|animalitos|pastiseta|rocko|sponch|rielito)')
barcel_regex = re.compile(r'(karameladas|takis|churritos|chicharron|valentones|triki|chip|snacks)')
mantecadas_regex = re.compile(r'mantecadas')
tuinky_regex = re.compile(r'tuinky')
gum_regex = re.compile(r'(gum|yerbabuena|menta|mentho)')
other_party_drink_regex = re.compile(r'(coca cola|sprite|fresca|fanta|manzana lift|agua ciel|'\
                                     'powerade|joya|sidral mundet|sonrisas|capuccino|moka|cafe)')
non_food_regex = re.compile(r'(camioncito)')
leche_regex = re.compile(r'leche')

product_category_list = list()

for i, (product_short_name, processed_product_short_name, product_weight) in enumerate(zip(lower_product_short_name_list, 
                                                            lower_processed_product_short_name_list, product_weight_list)):
    product_short_name = product_short_name.strip()
    
    if re.search(pan_regex, product_short_name):
        if re.search(tostado_regex, product_short_name):
            product_category_list.append('pan tostado')
        elif re.search(grain_regex, product_short_name):
            product_category_list.append('pan grano')
        else:
            product_category_list.append('pan')
            
    elif re.search(bollo_regex, product_short_name):
        product_category_list.append('bollo')
    elif re.search(tortilla_regex, product_short_name):
        if re.search(tostado_regex, product_short_name):
            product_category_list.append('tortilla tostado')
        else:
            product_category_list.append('tortilla')
    else:
        match = False
        for k, processed_product_name_lookup in enumerate(product_name_lookup_list):
            words = processed_product_name_lookup.split()
            match = True
            for word in words:
                if not re.search(word, product_short_name):
                    if word == 'pan' or word == 'bimb':
                        continue
                    match = False
                    break
            
            if match:
                if category_lookup_list[k] == 'panes':
                    if subcategory_lookup_list[k] == 'panes':
                        product_category_list.append('pan')
                    elif subcategory_lookup_list[k] == 'bolleria':
                        product_category_list.append('bollo')
                    elif re.search(tostado_regex, product_short_name):
                        product_category_list.append('pan tostado')
                    else:
                        product_category_list.append('bread flake')
                elif category_lookup_list[k] == 'tortillas y tostadas':
                    if subcategory_lookup_list[k] == 'tortillas':
                        product_category_list.append('tortilla')
                    else:
                        product_category_list.append('tortilla tostado')
                else:
                    product_category_list.append(category_lookup_list[k])
                break
                
        if match:
            continue
        
        for k, processed_product_name_lookup in enumerate(processed_product_name_lookup_list):
            words = processed_product_name_lookup.split()
            match = True
            for word in words:
                if not re.search(word, product_short_name):
                    if word == 'pan' or word == 'bimb':
                        continue
                    match = False
                    break
            
            if match:
                if category_lookup_list[k] == 'panes':
                    if subcategory_lookup_list[k] == 'panes':
                        product_category_list.append('pan')
                    elif subcategory_lookup_list[k] == 'bolleria':
                        product_category_list.append('bollo')
                    elif re.search(tostado_regex, product_short_name):
                        product_category_list.append('pan tostado')
                    else:
                        product_category_list.append('bread flake')
                elif category_lookup_list[k] == 'tortillas y tostadas':
                    if subcategory_lookup_list[k] == 'tortillas':
                        product_category_list.append('tortilla')
                    else:
                        product_category_list.append('tortilla tostado')                        
                else:
                    product_category_list.append(category_lookup_list[k])
                break
                
        
        if not match:           
            if re.search(pan_dulce_regex, product_short_name):
                product_category_list.append('pan dulce')
            elif re.search(barras_regex, product_short_name):
                product_category_list.append('barras')
            elif re.search(galletas_regex, product_short_name):
                product_category_list.append('galletas')
            elif re.search(galletas_kc_regex, product_short_name):
                product_category_list.append('galletas')
            elif re.search(pasteles_regex, product_short_name):
                product_category_list.append('pasteles')
            elif re.search(mantecadas_regex, product_short_name):
                product_category_list.append('pan dulce')
            elif re.search(tuinky_regex, product_short_name):
                product_category_list.append('galletas')
            elif re.search(barcel_regex, product_short_name):
                product_category_list.append('botanas')
            elif re.search(other_party_drink_regex, product_short_name):
                product_category_list.append('drink')
            elif re.search(tostado_regex, product_short_name):
                product_category_list.append('pan tostado')
            elif re.search(gum_regex, product_short_name):
                product_category_list.append('gum')
            elif re.search(frozen_regex, product_short_name):
                product_category_list.append('frozen')
            elif re.search(lonchibon_regex, product_short_name) and product_brand_list[i] == 'LON':
                product_category_list.append('alimentos preparados')
            elif re.search(flake_regex, product_short_name):
                product_category_list.append('bread flake')
            elif re.search(leche_regex, product_short_name):
                product_category_list.append('lacteos')
            elif re.search(non_food_regex, product_short_name):
                product_category_list.append('non food')
            elif product_weight == 680 or product_weight == 475 or product_weight == 567:
                if re.search(grain_regex, product_short_name):
                    product_category_list.append('pan grano')
                else:
                    product_category_list.append('pan')
            else:
                product_category_list.append('other')

products['tentative_category'] = product_category_list
products.to_csv('../input/producto_tabla.mod.csv', index = False)                
