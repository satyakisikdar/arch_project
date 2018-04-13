import csv
import re
import pandas as pd
from sys import argv

def get_power_area_info(st):
    '''
    The first 10 lines contain the power and area info
    '''
    pat = r'(\d+\.?\d+e?(-|\+)?\d*)+'
    names = ['sigma', 'subarr_area', 'neuron_area', 'total_area', 'leak_sa_IH', 'leak_sa_HO',
            'leak_nu_IH', 'leak_nu_HO','total_leak_sa','total_leak_nu']
    return dict((k, v) for k, v in zip(names, map(lambda x: x[0], re.findall(pat, st))))


def get_epoch_info(st):
    '''
     eg:
    Accuracy at 1 epochs is : 78%
     Read latency=2.5479e-04 s
    Write latency=0.0000e+00 s
    Read energy=3.3799e-06 J
    Write energy=0.0000e+00 J
    Time taken= 2.78385 sec
    '''
    # print('string:', st)
    n = int(st[st.find('at ') + len('at'): st.find('epochs')])
    data = {'n': n}#, 'accuracy': accuracy}
    pat = r'((\d+\.?\d+e?(-|\+)?\d+)|inf)'
    names = ['accuracy', 'read_lat', 'write_lat', 'read_en', 'write_en', 'time']
    # print(re.findall(pat, st))
    data.update((k, v) for k, v in zip(names, map(lambda x: x[0], re.findall(pat, st))))
    return data

def get_all_epochs(lines, n):
    i = 0
    while True:
        data = get_epoch_info('\n'.join(lines[i: i + n]))
        print(sorted(data.items()))
        print()
        i = i + n
        if data['n'] == 125:
            break
        if i == 2*n:
            break

if __name__ == '__main__':

    if len(argv) < 2:
        print('Enter the output file as an argument')
        exit(0)
    elif len(argv) == 2:
        n = 5
    else:
        fi = argv[1]
        n = int(argv[2])

    with open(argv[1]) as f:
        st = f.read()

    lines = st.split('\n')

    power_data = get_power_area_info('\n'.join(lines[: 9]))
    #print(power_data)

    get_all_epochs(lines[9: ], n)
    # epoch_info = get_epoch_info('\n'.join(lines[9: 9 + n]))
    # print(epoch_info)
