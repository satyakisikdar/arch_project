import re
import pandas as pd
import glob
from sys import argv

def get_power_area_info(st, n):
    '''
    The first 10 lines contain the power and area info
    '''
    # print('st:', st)
    pat = r'(\d+\.?\d+e?(-|\+)?\d*)+'
    names = ['subarr_area', 'neuron_area', 'total_area', 'leak_sa_IH', 'leak_sa_HO', 'leak_nu_IH', 'leak_nu_HO',
             'total_leak_sa','total_leak_nu']
    data = dict((k, v) for k, v in zip(names, map(lambda x: float(x[0]), re.findall(pat, st))))

    df = pd.DataFrame(data, columns=data.keys(), index=[0])
    df['name'] = n
    return df


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
    data = {'epoch': n}#, 'accuracy': accuracy}
    pat = r'((\d+\.\d+e?(-|\+)?\d+)|inf)'
    names = ['accuracy', 'read_lat', 'write_lat', 'read_en', 'write_en']
    # print(re.findall(pat, st))
    data.update((k, v) for k, v in zip(names, map(lambda x: float(x[0]), re.findall(pat, st))))
    return data


def get_all_epochs(lines, n):
    data = []
    i = 0
    while True:
        d = get_epoch_info('\n'.join(lines[i: i + n]))
        data.append(d)
        i = i + n
        if d['epoch'] == 125:
            break

    return pd.DataFrame(data, columns=d.keys())


if __name__ == '__main__':
    fi = './famim/online/00.conductance'

    open('./online_conductance', 'w')

    # open('./conductance_offline.xlsx', 'w')

    # writer = pd.ExcelWriter('conductance_online2.xlsx', engine='xlsxwriter')
    for i, fname in enumerate(glob.glob('./famim/online/*')):
        name = fname.split('/')[2] + '_' + fname.split('/')[-1].split('.')[-1]
        n = fname.split('/')[-1].split('.')[0]
        print(name, n)

        with open(fname) as f:
            st = f.read()

        lines = st.split('\n')

        df_power = get_power_area_info('\n'.join(lines[: 9]), n)
        # print(i, df_power.head())
        if i == 0:
            df_power.to_csv(open(name, 'a'), index=False)
        else:
            df_power.to_csv(open(name, 'a'), index=False, header=False)

        # df_epoch = get_all_epochs(lines[9: ], 5)
        # print(df_epoch.tail())
        # df_epoch.to_excel(writer, sheet_name=str(n), index=False)


    # writer.save()
    # writer.close()

    # if len(argv) < 2:
    #     print('Enter the output file as an argument')
    #     exit(0)
    # elif len(argv) == 2:
    #     n = 5
    # else:
    #     n = int(argv[2])

    # fi = argv[1]

    #
    # n = 5
    #
    # with open(fi) as f:
    #     st = f.read()
    #
    # lines = st.split('\n')
    #
    # power_data = get_power_area_info('\n'.join(lines[: 9]))
    # print(power_data)
    #
    # df = get_all_epochs(lines[9: ], n)
    # df.dropna(axis=1, inplace=True)
    # print(df.head())
    # # epoch_info = get_epoch_info('\n'.join(lines[9: 9 + n]))
    # # print(epoch_info)
