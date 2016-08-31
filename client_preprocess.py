import numpy as np
import pandas as pd 
import re
from sklearn import preprocessing


df_client = pd.read_csv("./input/cliente_tabla.csv")

np_to_upper = np.vectorize(lambda x: x.upper())
df_client[u'NombreCliente'] = np_to_upper(df_client[u'NombreCliente'].values)

client_names = df_client[u'NombreCliente'].values

client_short_name = client_names.copy()

for i, client_name in enumerate(client_short_name):
    if client_name == 'NO IDENTIFICADO' or client_name == 'SIN NOMBRE' or client_name == 'INACTIVAR NO TOCAR':
        continue
    elif re.search(r'OXXO', client_name):
        client_short_name[i] = 'OXXO STORE'
    elif re.search(r'BIMBO', client_name):
        client_short_name[i] = 'BIMBO STORE'
    elif re.search(r'REMISION', client_name):
        client_short_name[i] = 'REMISION BIMBO SUPPLY'
    elif re.search(r'^COMODIN R', client_name):
        client_short_name[i] = 'COMODIN R BIMBO SUPPLY'
    elif re.search(r'CONASUPO', client_name):
        client_short_name[i] = 'CONASUPO STORE'
    elif re.search(r'7 ELEVEN', client_name) or re.search(r'SEVEN ELEVEN', client_name):
        client_short_name[i] = '7 ELEVEN C-STORE'
    elif re.search(r'F\s*GDL', client_name) or re.search(r'FARMACIA GUADALAJARA', client_name):
        client_short_name[i] = 'F GDL PHARMACY'
    elif re.search(r'SUPERCITY', client_name) or re.search(r'SUPER CITY', client_name):
        client_short_name[i] = 'SUPER CITY C-STORE'
    elif re.search(r'CIRCULO K', client_name):
        client_short_name[i] = 'CIRCULO K C-STORE'
    elif re.search(r'TIENDAS EXTRA', client_name) or re.search(r'TIENDA EXTRA', client_name):
        client_short_name[i] = 'TIENDA EXTRA C-STORE'
    elif re.search(r'TIENDAS TRES', client_name):
        client_short_name[i] = 'TIENDA TRES B STORE'
    elif re.search(r'WALDOS MART', client_name):
        client_short_name[i] = 'WALDOS MART SUPERMARKET'
    elif re.search(r'CHEDRAUI', client_name):
        client_short_name[i] = 'CHEDRAUI SUPERMARKET'
    elif re.search(r'SUPERAMA', client_name):
        client_short_name[i] = 'SUPERAMA SUPERMARKET'
    elif re.search(r'CALIMAX', client_name):
        client_short_name[i] = 'CLIMAX SUPERMARKET'
    elif re.search(r'ALSUPER', client_name):
        client_short_name[i] = 'ALSUPER SUPERMARKET'
    elif re.search(r'SUPER KIOSCO', client_name):
        client_short_name[i] = 'SUPER KIOSCO SUPERMARKET'
    elif re.search(r'SUPER NETO', client_name):
        client_short_name[i] = 'SUPER NETO SUPERMARKET'
    elif re.search(r'^GO MART', client_name) or re.search(r' GO MART', client_name):
        client_short_name[i] = 'GO MART SUPERMARKET'
    elif re.search(r'^WAL\s*MART', client_name):
        client_short_name[i] = 'WALMART'
    elif re.search(r'^HEB ', client_name) and not re.search(r'HEB UNIVERSIDAD', client_name):
        client_short_name[i] = 'HEB SUPERMARKET'
    elif re.search(r'ARTELI', client_name):
        client_short_name[i] = 'ARTELI SUPERMARKET'
    elif re.search(r'EL ASTURIANO', client_name):
        client_short_name[i] = 'EL ASTURIANO SUPERMARKET'
    elif re.search(r'LA GRAN BODEGA', client_name):
        client_short_name[i] = 'LA GRAN BODEGA SUPERMARKET'
    elif re.search(r'SUPER SORIANA', client_name) or re.search(r'HIPER SORIANA', client_name) or \
        re.search(r'SORIANA EXPRESS', client_name):
        client_short_name[i] = 'SORIANA SUPERMARKET'
    elif re.search(r'^COMERCIAL MEXICANA', client_name) or \
        re.search(r'BODEGA COMERCIAL MEXICANA', client_name) or re.search(r'MEGA COMERCIAL MEXICANA', client_name):
        client_short_name[i] = 'COMERCIAL MEXICANA SUPERMARKET'
    elif re.search(r'CASA LEY', client_name):
        client_short_name[i] = 'CASA LEY GROCERY'
    elif re.search(r'MODELORAMA', client_name):
        client_short_name[i] = 'MODELORAMA BEER STORE'
    elif re.search(r'TECATE SIX', client_name) or re.search('TKT SIX', client_name):
        client_short_name[i] = 'TECATE SIX BEER STORE'
    elif re.search(r'EL PALACIO DE HIERRO', client_name):
        client_short_name[i] = 'EL PALACIO DE HIERRO DEPARTMENT STORE'
    elif re.search(r'SUBURBIA', client_name):
        client_short_name[i] = 'SUBURBIA DEPARTMENT STORE'
    elif re.search(r'MERZA', client_name):
        client_short_name[i] = 'MERZA DEPARTMENT STORE'
    elif re.search(r'LIVERPOOL', client_name):
        client_short_name[i] = 'LIVERPOOL DEPARTMENT STORE'
    elif re.search(r'FARMACIAS BENAVIDES', client_name):
        client_short_name[i] = 'FARMACIAS BENAVIDES PHARMACY'
    elif re.search(r'SAMS CLUB', client_name):
        client_short_name[i] = 'SAMS CLUB WAREHOUSE CLUB'
    elif re.search(r'CITY CLUB', client_name):
        client_short_name[i] = 'CITY CLUB WAREHOUSE CLUB'
    elif re.search(r'COSTCO', client_name):
        client_short_name[i] = 'COSTCO WAREHOUSE CLUB'
    elif re.search(r'CINEMEX', client_name):
        client_short_name[i] = 'CINEMEX CINEMA CHAIN'
    elif re.search(r'CINEMARK', client_name):
        client_short_name[i] = 'CINEMARK CINEMA CHAIN'
    elif re.search(r'CINEPOLIS', client_name):
        client_short_name[i] = 'CINEPOLIS CINEMA CHAIN'
    elif re.search(r'MMCINEMAS', client_name) or re.search(r'MM CINEMAS', client_name):
        client_short_name[i] = 'MMCINEMAS CINEMA CHAIN'
    elif re.search(r'CINEMA', client_name):
        client_short_name[i] = 'OTHER CINEMA'
    elif re.search(r'MC\s*DONALDS', client_name):
        client_short_name[i]  = 'MCDONALDS FAST FOOD CHAIN'
    elif re.search(r'CARLS JR', client_name):
        client_short_name[i] = 'CARLS JR FAST FOOD CHAIN'
    elif re.search(r'BURGER KING', client_name):
        client_short_name[i] = 'BURGER KING FAST FOOD CHAIN'
    elif re.search(r'KFC', client_name):
        client_short_name[i] = 'KFC FAST FOOD CHAIN'
    elif re.search(r'CHILIS', client_name):
        client_short_name[i] = 'CHILIS FAST FOOD CHAIN'
    elif re.search(r'APPLEBEES', client_name):
        client_short_name[i] = 'APPLEBEES FAST FOOD CHAIN'
    elif re.search(r'VIPS', client_name):
        client_short_name[i] = 'VIPS RESTAURANT CHAIN'
    elif re.search(r'SANBORNS', client_name):
        client_short_name[i] = 'SANBORNS RESTAURANT CHAIN'
    elif re.search(r'TOKS', client_name):
        client_short_name[i] = 'TOKS RESTAURANT CHAIN'
    elif re.search(r'EL POLLO FELIZ', client_name):
        client_short_name[i] = 'EL POLLO FELIZ RESTAURANT CHAIN'
    elif re.search(r'EL POLLO PEPE', client_name):
        client_short_name[i] = 'EL POLLO PEPE RESTAURANT CHAIN'
    elif re.search(r'PECHUGON', client_name):
        client_short_name[i] = 'EL PECHUGON RESAURANT CHAIN'
    elif re.search(r'GI FRIDAYS', client_name):
        client_short_name[i] = 'TGI FRIDAYS BAR CHAIN'
    elif re.search(r'LA MICHOACANA', client_name):
        client_short_name[i] = 'LA MICHOACANA ICE CREAM STORE'        
    elif re.search(r'SDN', client_name):
        client_short_name[i] = 'SDN CAFE TIENDA'
    elif re.search(r'COLEG', client_name) or re.search(r'UNIV', client_name) or \
        re.search(r'ESCU', client_name) or re.search(r'INSTI', client_name) or \
        re.search(r'TECNICA', client_name) or re.search(r'PREPAR', client_name) or \
        re.search(r'FACULT', client_name):
        client_short_name[i] = 'SCHOOL'
    elif re.search(r'HOSPITAL', client_name) or re.search('CLINI', client_name):
        client_short_name[i] = 'HOSTPITAL CLINIC'
    elif re.search(r'FARMA', client_name):
        client_short_name[i] = 'OTHER PHARMACY'
    elif re.search(r'CREMERIA', client_name) or re.search(r'SALCHICHONERIA', client_name):
        client_short_name[i] = 'OTHER CREMERIA SALCHICHONERIA STORE'
    elif re.search(r'CARNES', client_name) or re.search(r'CARNICERIA', client_name):
        client_short_name[i] = 'OTHER MEAT STORE'
    elif re.search(r'DULCERIA', client_name):
        client_short_name[i] = 'OTHER CANDY STORE'
    elif re.search(r'HELADO', client_name) or re.search(r'NIEVE', client_name):
        client_short_name[i] = 'OTHER ICE CREAM STORE'
    elif re.search(r'BURGER', client_name) or re.search(r'BURGU', client_name):
        client_short_name[i] = 'OTHER BURGER STORE'
    elif re.search(r'HOT DOG', client_name):
        client_short_name[i] = 'OTHER HOT DOG'
    elif re.search(r'SANDWICH', client_name):
        client_short_name[i] = 'OTHER SANDWICH'
    elif re.search(r'PIZZA', client_name):
        client_short_name[i] = 'OTHER PIZZA'
    elif re.search(r'TACO', client_name) or re.search(r'TORTA', client_name) or re.search(r'TAQUER', client_name):
        client_short_name[i] = 'OTHER TACO'
    elif re.search(r'CAFE', client_name):
        client_short_name[i] = 'OTHER CAFETERIA'
    elif re.search(r'REST', client_name) or re.search(r'COMEDOR', client_name)\
        or re.search(r'COCINA', client_name):
        client_short_name[i] = 'OTHER EATERY'
    elif re.search(r'VERDU', client_name) or re.search(r'FRUT', client_name):
        client_short_name[i] = 'OTHER FRESH FOOD'    
    elif re.search(r'ABARROTES', client_name) or re.search(r'ABTS', client_name):
        client_short_name[i] = 'OTHER ABARROTES STORE'
    elif re.search(r'MINI\s*SUPER', client_name) or re.search(r'MINI\s*MARKET', client_name) or \
        re.search(r'MINI\s*MERCADO', client_name) or re.search(r'MINI\s*MART', client_name) or \
        re.search(r'SUPERETTE', client_name):
        client_short_name[i] = 'OTHER MINI SUPERMARKET MARKET STORE'
    elif re.search(r'BODEGA', client_name):
        client_short_name[i] = 'OTHER BODEGA STORE'
    elif re.search(r'TIENDA', client_name):
        client_short_name[i] = 'OTHER TIENDA STORE'
    elif re.search(r'MISC', client_name):
        client_short_name[i] = 'OTHER MISC STORE'
    elif re.search(r'PAPELERIA', client_name):
        client_short_name[i] = 'OTHER PAPELERIA STORE'
    elif re.search(r'EXPENDIO', client_name) or re.search(r'SNACK', client_name):
        client_short_name[i] = 'OTHER EXPENDIO SNACK STORE'
    elif re.search(r'SUPER', client_name):
        client_short_name[i] = 'OTHER SUPER MARKET'
    elif re.search(r'HOTEL', client_name) or re.search(r'MOTEL', client_name) or \
        re.search(r' INN', client_name):
        client_short_name[i] = 'HOTEL MOTEL INN'
    elif re.search(r'MERCADO', client_name) or re.search(r'MARKET', client_name) or \
        re.search(r'MART', client_name):
        client_short_name[i] = 'OTHER MARKET'
    elif re.search(r'COMERCIAL', client_name) or re.search(r'PROVIDENCIA', client_name) or\
        re.search(r'DEPOSITO', client_name) or re.search(r'CAMBIO', client_name) or\
        re.search(r'EXP', client_name) or re.search(r'PLAZA', client_name):
        client_short_name[i] = 'OTHER GENERAL STORE'
    elif re.search(r'^LA ', client_name) or re.search(r' LA ', client_name) or\
        re.search(r'^EL ', client_name) or re.search(r' EL ', client_name) or \
        re.search('^DE ', client_name) or re.search(r' DE ', client_name) or \
        re.search(r'^LOS ', client_name) or re.search(r' LOS ', client_name) or \
        re.search(r'^DEL ', client_name) or re.search(r' DEL ', client_name) or \
        re.search(r' Y ', client_name) or \
        re.search(r'^SAN ', client_name) or re.search(r' SAN ', client_name) or \
        re.search(r'^SANTA ', client_name) or re.search(r' SANTA ', client_name) or \
        re.search(r'^AG ', client_name) or re.search(r' AG ', client_name) or\
        re.search(r'^LAS ', client_name) or re.search(r' LAS ', client_name) or \
        re.search(r'^MI ', client_name) or re.search(r' MI ', client_name) or \
        re.search(r'^MA ', client_name) or re.search(r' MA ', client_name)or \
        re.search(r'^CASA ', client_name) or re.search(r' CASA ', client_name) or \
        re.search(r' II ', client_name) or re.search(r'[0-9]+', client_name):
        client_short_name[i] = 'OTHER BUSINESS'
    else :
        client_short_name[i] = 'INDIVIDUAL OR OTHER'
    

df_client['short_name'] = client_short_name

client_short_name = df_client_mod['short_name'].values

le = preprocessing.LabelEncoder()
le.fit(client_short_name)

df_client_mod['short_name_id'] = le.transform(client_short_name)

df_client_mod.to_csv('./input/cliente_tabla.mod.csv', index = False)