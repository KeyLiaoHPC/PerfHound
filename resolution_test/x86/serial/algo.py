import os
import sys
import getopt
import numpy as np
import pandas as pd
import scipy.stats as stats
from sklearn import cluster
from collections import Counter


frequency = 2.6
interval_data_percent = 95
interval_error_percent = 2.5
resolution_threshold = 0.05
constant_rates = {"cas114": 2494103, "cas113": 249414}
ins_lat = {"ADD": 1, "MUL": 3, "FMA": 4, "LD": 0.5, "ST": 1}


def usage():
    print("This script is used to process the result csv files")
    print("-h or --help     print this message")
    print("-s or --start    <start gid>,<start pid>")
    print("-e or --end      <end gid>,<end_pid>")
    print("-i or --input    <run data directory>")
    print("-b or --backend  <pfhd or papi>")
    print("-f or --function <apply data process functions>")
    print("-a or --fargs <arguments of functions, split with comma>")
    print("if you choose timing_cost, no fargs are needed")
    print("if you choose least_interval, the fargs are <start_round,cost_cycle,cost_nanosec>")
    print("if you choose variation_resolution, the fargs are <least_round,cost_cycle,cost_nanosec>")


def parse_option():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hs:e:i:b:f:a:", ["start=", "end=", "input=", "backend=", "function=", "args="])
    except getopt.GetoptError:
        print("parameter format error")
        usage()
        sys.exit(-1)

    options_dict = dict()
    for opt, arg in opts:
        if opt in ('-h', '--help'):
            usage()
            sys.exit(-1)
        elif opt in ('-s', '--start'):
            sgid = int(arg.split(',')[0])
            spid = int(arg.split(',')[1])
            options_dict["start_gid"] = sgid
            options_dict["start_pid"] = spid
        elif opt in ('-e', '--end'):
            egid = int(arg.split(',')[0])
            epid = int(arg.split(',')[1])
            options_dict["end_gid"] = egid
            options_dict["end_pid"] = epid
        elif opt in ('-i', '--input'):
            options_dict["input"] = arg
        elif opt in ('-b', '--backend'):
            options_dict["backend"] = arg
        elif opt in ('-f', '--function'):
            options_dict["function"] = arg
        elif opt in ('-a', '--fargs'):
            options_dict["fargs"] = [x for x in arg.split(',') if x]
    options_dict["output"] = f'{options_dict["input"]}/diff_{sgid}-{spid}_{egid}-{epid}'

    # print(f'process data in {options_dict["input"]}')
    # print(f'backend use {options_dict["backend"]}')
    # print(f'start gid {options_dict["start_gid"]}, start pid {options_dict["start_pid"]}')
    # print(f'end gid {options_dict["end_gid"]}, end pid {options_dict["end_pid"]}')
    if not os.path.exists(options_dict["input"]):
        print(f"The path {options_dict['input']} does not exist")
        sys.exit(-1)
    return options_dict


def parse_rankmap(options_dict):
    input_dir = options_dict["input"]
    rankmap_path = f"{input_dir}/rankmap.csv"
    rankmap_df = pd.read_csv(rankmap_path)
    rankmap_dict = dict()
    for key in ["rank", "cpu", "hostname"]:
        rankmap_dict[key] = rankmap_df[key].values[0]
    dir_path = f"{options_dict['output']}/{rankmap_dict['hostname']}"
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)
    return rankmap_dict


def interval2csv(options_dict, rankmap_dict):
    sgid = options_dict["start_gid"]
    spid = options_dict["start_pid"]
    egid = options_dict["end_gid"]
    epid = options_dict["end_pid"]
    cpu_id = rankmap_dict["cpu"]
    rank_id = rankmap_dict["rank"]
    hostname = rankmap_dict["hostname"]
    src_file = f"{options_dict['input']}/{hostname}/r{rank_id}c{cpu_id}.csv"
    dst_file = f"{options_dict['output']}/{hostname}/r{rank_id}c{cpu_id}.csv"
    host_prefix = hostname.split(".")[0]
    src_df = pd.read_csv(src_file)
    col_names = list(src_df.columns.values)
    col_names.remove("gid")
    col_names.remove("pid")
    src_df_sp = src_df[(src_df["gid"] == sgid) & (src_df["pid"] == spid)][col_names]
    src_df_ep = src_df[(src_df["gid"] == egid) & (src_df["pid"] == epid)][col_names]
    src_df_sp.reset_index(inplace=True)
    src_df_ep.reset_index(inplace=True)
    res_df = (src_df_ep - src_df_sp)[col_names]
    if options_dict["backend"] == "pfhd":
        res_df["nanosec"] = res_df["nanosec"].apply(lambda x: int(x * 1000000 // constant_rates[host_prefix]))
    res_df.to_csv(dst_file, index=False)
    return res_df


def get_res_df(options_dict):
    rankmap_dict = parse_rankmap(options_dict)
    res_df = interval2csv(options_dict, rankmap_dict)
    return res_df


def timing_cost(options_dict):
    cost_res_file = options_dict["fargs"][0]
    res_df = get_res_df(options_dict)
    cost_nanosec = res_df["nanosec"].min()
    cost_cycle = res_df["cycle"].min()
    with open(cost_res_file, "w") as fp:
        fp.write(f"{cost_cycle}\n{cost_nanosec}")


def rm_os_noise(res_df):
    min_cycle = np.min(res_df["cycle"])
    min_nanosec = np.min(res_df["nanosec"])
    threshold_nanosec = min_nanosec + 1000
    threshold_cycle = min_cycle + (1000 * frequency)
    new_res_df = res_df[(res_df["nanosec"] < threshold_nanosec) & (res_df["cycle"] < threshold_cycle)]
    return new_res_df


def least_interval(options_dict):
    test_round = int(options_dict["fargs"][0])
    cost_res_file = options_dict["fargs"][1]
    interval_res_file = options_dict["fargs"][2]
    with open(cost_res_file, 'r') as fp:
        lines = fp.readlines()
        cost_cycle = int(lines[0].strip())
        cost_nanosec = int(lines[1].strip())
    new_res_df = rm_os_noise(get_res_df(options_dict))
    all_cycles = np.array(new_res_df['cycle'], dtype=np.int64) - cost_cycle
    all_nanosecs = np.array(new_res_df['nanosec'], dtype=np.int64) - cost_nanosec
    # judge
    theo_cycle = 2 * test_round * ins_lat["ADD"]
    theo_nanosec = theo_cycle / frequency
    theo_cycle_up = theo_cycle * (1 + interval_error_percent / 100)
    theo_cycle_down = theo_cycle * (1 - interval_error_percent / 100)
    theo_nanosec_up = theo_nanosec * (1 + interval_error_percent / 100)
    theo_nanosec_down = theo_nanosec * (1 - interval_error_percent / 100)
    up_percent = (100 + interval_data_percent) / 2
    down_percent = (100 - interval_data_percent) / 2
    real_cycle_up = np.percentile(all_cycles, up_percent)
    real_cycle_down = np.percentile(all_cycles, down_percent)
    real_nanosec_up = np.percentile(all_nanosecs, up_percent)
    real_nanosec_down = np.percentile(all_nanosecs, down_percent)
    with open(interval_res_file, 'w') as fp:
        if ((real_cycle_down >= theo_cycle_down) and (real_cycle_up <= theo_cycle_up) and
            (real_nanosec_down >= theo_nanosec_down) and (real_nanosec_up <= theo_nanosec_up)):
        # if ((real_nanosec_down >= theo_nanosec_down) and (real_nanosec_up <= theo_nanosec_up)):
            fp.write(f"yes\n{test_round}\n{np.median(all_cycles)}\n{np.median(all_nanosecs)}")
            print(f"Round {test_round} passed\n")
        else:
            fp.write("no")
            print(f"Round {test_round} cycle failed, actual: [{round(real_cycle_down, 2)}, {round(real_cycle_up, 2)}], theo [{round(theo_cycle_down, 2)}, {round(theo_cycle_up, 2)}]")
            print(f"Round {test_round} nanosec failed, actual: [{round(real_nanosec_down, 2)}, {round(real_nanosec_up, 2)}], theo [{round(theo_nanosec_down, 2)}, {round(theo_nanosec_up, 2)}]")


# def cal_overlap(data1, data2):
#     count_data1 = sorted(Counter(data1).items(), key=lambda x: x[0])
#     count_data2 = sorted(Counter(data2).items(), key=lambda x: x[0])
#     data1_key = np.array([x[0] for x in count_data1])
#     data1_val = np.array([x[1] for x in count_data1])
#     data2_key = np.array([x[0] for x in count_data2])
#     data2_val = np.array([x[1] for x in count_data2])
#     data1_val = data1_val / np.sum(data1_val)
#     data2_val = data2_val / np.sum(data2_val)
#     dict_data1 = dict(zip(data1_key, data1_val))
#     dict_data2 = dict(zip(data2_key, data2_val))
#     overlap = 0
#     for key2, value2 in dict_data2.items():
#         if key2 in data1_key:
#             overlap += min(dict_data1[key2], value2)
#     return overlap


def variation_judge(round_1_df, round_2_df, cost_cycle, cost_nanosec):
    round_1_cycles = np.array(round_1_df["cycle"], dtype=np.int64) - cost_cycle
    round_1_nanosecs = np.array(round_1_df["nanosec"], dtype=np.int64) - cost_nanosec
    round_2_cycles = np.array(round_2_df["cycle"], dtype=np.int64) - cost_cycle
    round_2_nanosecs = np.array(round_2_df["nanosec"], dtype=np.int64) - cost_nanosec
    # cond_1 = (cal_overlap(round_1_cycles, round_2_cycles) <= resolution_threshold)
    # cond_2 = (cal_overlap(round_1_nanosecs, round_2_nanosecs) <= resolution_threshold)
    cond_1 = (np.quantile(round_2_cycles, resolution_threshold) >= np.quantile(round_1_cycles, 1 - resolution_threshold))
    cond_2 = (np.quantile(round_2_nanosecs, resolution_threshold) >= np.quantile(round_1_nanosecs, 1- resolution_threshold))
    cond = (cond_1 and cond_2)
    delta_cycle = np.median(round_2_cycles) - np.median(round_1_cycles)
    delta_nanosec = np.median(round_2_nanosecs) - np.median(round_1_nanosecs)
    return cond, delta_cycle, delta_nanosec


def variation_resolution(options_dict):
    resolution_continue_num = int(options_dict["fargs"][0])
    cost_res_file = options_dict["fargs"][1]
    interval_res_file = options_dict["fargs"][2]
    resolution_res_file = options_dict["fargs"][3]
    with open(cost_res_file, 'r') as fp:
        lines = fp.readlines()
        cost_cycle = int(lines[0].strip())
        cost_nanosec = int(lines[1].strip())
    with open(interval_res_file, 'r') as fp:
        lines = fp.readlines()
        least_round = int(lines[1].strip())

    input_split = options_dict["input"].split('/')
    end_round = int(input_split[-2].split('_')[-1])
    delta_round = (end_round - least_round) // resolution_continue_num
    rounds = [x for x in range(least_round, end_round + 1, delta_round)]
    test_res = list()
    delta_median_cycles = list()
    delta_median_nanosecs = list()
    for i in range(resolution_continue_num):
        round_1 = rounds[i]
        round_2 = rounds[i + 1]
        input_split[-2] = f"round_{round_1}"
        options_dict["input"] = '/'.join(input_split)
        round_1_df = rm_os_noise(get_res_df(options_dict))
        input_split[-2] = f"round_{round_2}"
        options_dict["input"] = '/'.join(input_split)
        round_2_df = rm_os_noise(get_res_df(options_dict))
        cond, delta_cycle, delta_nanosec = variation_judge(round_1_df, round_2_df, cost_cycle, cost_nanosec)
        test_res.append(cond)
        delta_median_cycles.append(delta_cycle)
        delta_median_nanosecs.append(delta_nanosec)
    with open(resolution_res_file, 'w') as fp:
        if np.sum(test_res) == resolution_continue_num:
            fp.write(f"yes\n{int(np.max(delta_median_cycles))}\n{int(np.max(delta_median_nanosecs))}")
            print(f"resolution {delta_round} round passed")
        else:
            fp.write("no")
            print(f"resolution {delta_round} round failed, {np.sum(test_res)} of {resolution_continue_num}")


def main():
    options_dict = parse_option()
    if options_dict["function"] == "timing_cost":
        timing_cost(options_dict)
    elif options_dict["function"] == "least_interval":
        least_interval(options_dict)
    elif options_dict["function"] == "variation_resolution":
        variation_resolution(options_dict)


if __name__ == '__main__':
    main()
