import os
import sys
import getopt
import numpy as np
import pandas as pd
from sklearn import cluster
from collections import Counter
from sklearn.ensemble import IsolationForest
from scipy.stats import wasserstein_distance


# frequency = 2.6
confidence_level = 0.05
data_level = 1 - confidence_level
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
        fp.write(f"{cost_cycle}\n{cost_nanosec}\n")


def get_best_param(data, start, factor):
    density = list()
    params = [(x / factor) + start for x in range(11)]
    for param in params:
        if param == 0:
            org_cycle_width = np.max(data[:, 0]) - np.min(data[:, 0])
            org_nanosec_width = np.max(data[:, 1]) - np.min(data[:, 1])
            density.append(data.shape[0] / (org_cycle_width * org_nanosec_width))
        else:
            labels = IsolationForest(random_state=42, contamination=param).fit_predict(data)
            target_idx = np.array([(label == 1) for label in labels])
            new_data = data[target_idx]
            cycle_width = np.max(new_data[:, 0]) - np.min(new_data[:, 0])
            cycle_nanosec = np.max(new_data[:, 1]) - np.min(new_data[:, 1])
            density.append(new_data.shape[0] / (cycle_width * cycle_nanosec))
    return params[np.argmax(np.diff(density))]


def rm_noise(org_df):
    start = 0
    factor = 100
    data_cycle = np.array(org_df["cycle"])
    data_nanosec = np.array(org_df["nanosec"])
    data = np.array([data_cycle, data_nanosec]).T
    while factor <= 1000:
        start = get_best_param(data, start, factor)
        factor *= 10
    best_param = (start + (10 / factor))
    labels = IsolationForest(random_state=42, contamination=best_param).fit_predict(data)
    tmp_idx = np.array([(label == 1) for label in labels])
    tmp_data = data[tmp_idx]
    tmp_max_cycle = np.max(tmp_data[:, 0])
    tmp_max_nanosec = np.max(tmp_data[:, 1])
    noise_idx = [i for i in range(len(labels)) if (labels[i] != 1)]
    for idx in noise_idx:
        if (data[idx, 0] <= tmp_max_cycle) and (data[idx, 1] <= tmp_max_nanosec):
            tmp_idx[idx] = True
    new_df = pd.DataFrame(data[tmp_idx], columns=["cycle", "nanosec"])
    return new_df


def least_interval(options_dict):
    test_round = int(options_dict["fargs"][0])
    cost_res_file = options_dict["fargs"][1]
    interval_res_file = options_dict["fargs"][2]
    with open(cost_res_file, 'r') as fp:
        lines = fp.readlines()
        cost_cycle = int(lines[0].strip())
        cost_nanosec = int(lines[1].strip())
    new_res_df = rm_noise(get_res_df(options_dict))
    all_cycles = np.array(new_res_df['cycle'], dtype=np.int64) - cost_cycle
    all_nanosecs = np.array(new_res_df['nanosec'], dtype=np.int64) - cost_nanosec
    # theo_cycle = 2 * test_round * ins_lat["ADD"]
    # theo_nanosec = theo_cycle / frequency
    min_cycle = np.min(all_cycles)
    min_nanosec = np.min(all_nanosecs)
    percent_cycle = np.quantile(all_cycles, data_level)
    percent_nanosec = np.quantile(all_nanosecs, data_level)
    interval_cycle = percent_cycle - min_cycle
    interval_nanosec = percent_nanosec - min_nanosec
    cond_1 = (((interval_cycle / min_cycle)) <= confidence_level)
    cond_2 = (((interval_nanosec / min_nanosec)) <= confidence_level)
    with open(interval_res_file, 'w') as fp:
        if cond_1 and cond_2:
            fp.write(f"yes\n{test_round}\n{np.min(all_cycles)}\n{np.min(all_nanosecs)}\n")
            print(f"Round {test_round} passed cycle: {min_cycle} {percent_cycle}")
            print(f"Round {test_round} passed nanosec: {min_nanosec} {percent_nanosec}")
        else:
            fp.write("no\n")
            print(f"Round {test_round} failed cycle: {min_cycle} {percent_cycle}")
            print(f"Round {test_round} failed nanosec: {min_nanosec} {percent_nanosec}")


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


def variation_judge(round_1_df, round_2_df, cost, indicator):
    round_1_data = np.array(round_1_df[indicator], dtype=np.int64) - cost
    round_2_data = np.array(round_2_df[indicator], dtype=np.int64) - cost
    cond = (np.min(round_2_data) > np.quantile(round_1_data, 1 - confidence_level))
    dist_w = wasserstein_distance(round_1_data, round_2_data)
    return cond, dist_w


def variation_resolution(options_dict):
    k = int(options_dict["fargs"][0])
    least_round = int(options_dict["fargs"][1])
    cost_res_file = options_dict["fargs"][2]
    resolution_res_file = options_dict["fargs"][3]
    with open(cost_res_file, 'r') as fp:
        lines = fp.readlines()
        cost_cycle = int(lines[0].strip())
        cost_nanosec = int(lines[1].strip())
    indicator = resolution_res_file.split('_')[0]
    if indicator == "cycle":
        cost = cost_cycle
    else:
        cost = cost_nanosec

    cond_res = list()
    dist_w_res = list()
    round_dfs = list()
    input_split = options_dict["input"].split('/')
    end_round = int(input_split[-2].split('_')[-1])
    rounds = [x for x in range(least_round, end_round + 1, k)]
    test_num = len(rounds) - 1
    for i in range(test_num + 1):
        input_split[-2] = f"round_{rounds[i]}"
        options_dict["input"] = '/'.join(input_split)
        round_dfs.append(rm_noise(get_res_df(options_dict)))
    for i in range(test_num):
        round_1_df = round_dfs[i]
        round_2_df = round_dfs[i + 1]
        cond, dist_w = variation_judge(round_1_df, round_2_df, cost, indicator)
        cond_res.append(cond)
        dist_w_res.append(dist_w)
    with open(resolution_res_file, 'w') as fp:
        if np.sum(cond_res) == test_num:
            indicator_resolution = int(np.max(dist_w_res))
            fp.write(f"yes\n{indicator_resolution}\n")
            print(f"resolution {k} round passed, {indicator_resolution} {indicator}")
        else:
            fp.write("no\n")
            print(f"resolution {k} round failed, {np.sum(cond_res)} of {test_num}")


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
