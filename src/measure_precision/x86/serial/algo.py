import os
import sys
import copy
import getopt
import numpy as np
import pandas as pd
import multiprocessing
from sklearn import cluster
from functools import partial
from collections import Counter
from sklearn.ensemble import IsolationForest
from scipy.stats import wasserstein_distance


# frequency = 2.5
error_level = 0.01
confidence_level = 0.05
ncpus = multiprocessing.cpu_count()
constant_rates = {"cas114": 2494103}
# ins_lat = {"ADD": 1, "MUL": 3, "FMA": 4, "LD": 0.5, "ST": 1}


def usage():
    ''' Print some info about how to use this script. '''
    print("This script is used to process the result csv files")
    print("-h or --help     print this message")
    print("-s or --start    <start gid>,<start pid>")
    print("-e or --end      <end gid>,<end_pid>")
    print("-i or --input    <run data directory>")
    print("-b or --backend  <pfhd or papi>")
    print("-f or --function <apply data process functions>")
    print("-a or --fargs <arguments of functions, split with comma>")
    print("if you choose cost_measure, the farg <result file of cost_measure> is needed")
    print("if you choose least_interval, the fargs are <current test round>,\
          <result file of cost_measure>, <result file of least_interval>")
    print("if you choose variation_resolution, the fargs are <current delta round>,\
          <least measurable round>, <result file of cost_measure>, <result file of variation_resolution>")


def parse_option():
    ''' Parse the command line options

        Parse the command line options that are needed to run this script.
        The details of the command line options are shown in the content of 'usage' function.

        Returns:
            options_dict: A dict that contains key-value pairs of command line options
    '''
    try:
        # parse the command line options
        opts, args = getopt.getopt(sys.argv[1:], "hs:e:i:b:f:a:", ["start=", "end=", "input=", "backend=", "function=", "args="])
    except getopt.GetoptError:
        print("parameter format error")
        usage()
        sys.exit(-1)
    # store the parsed command line options in the dict
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
    options_dict["output_suffix"] = f'diff_{sgid}-{spid}_{egid}-{epid}'
    options_dict["output"] = f'{options_dict["input"]}/{options_dict["output_suffix"]}'

    # print(f'process data in {options_dict["input"]}')
    # print(f'backend use {options_dict["backend"]}')
    # print(f'start gid {options_dict["start_gid"]}, start pid {options_dict["start_pid"]}')
    # print(f'end gid {options_dict["end_gid"]}, end pid {options_dict["end_pid"]}')
    if not os.path.exists(options_dict["input"]):
        print(f"The path {options_dict['input']} does not exist")
        sys.exit(-1)
    return options_dict


def parse_rankmap(options_dict):
    ''' Parse the file rankmap.csv

        Parse the rankmap.csv and get the info of rank id, cpu id and hostname.
        Then, create the output directory to store result data.

        Args:
            options_dict: the dict of parsed commandline options.

        Returns:
            rankmap_dict: A dict that contains the value of rank id, cpu id and hostname.
    '''
    rankmap_path = f"{options_dict['input']}/rankmap.csv"
    rankmap_df = pd.read_csv(rankmap_path)
    rankmap_dict = dict()
    for key in ["rank", "cpu", "hostname"]:
        rankmap_dict[key] = rankmap_df[key].values[0]
    dir_path = f"{options_dict['output']}/{rankmap_dict['hostname']}"
    # create the output directory
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)
    return rankmap_dict


def sample_pt_diff(options_dict, rankmap_dict):
    ''' Process the original data

        Process the original data according to the command line options,
        and write the result data to the output directory.

        Args:
            options_dict: the dict of parsed commandline options.
            rankmap_dict: the dict of parsed rankmap.csv file.

        Returns:
            res_df: the pandas.DataFrame of the result data.
    '''
    sgid = options_dict["start_gid"]
    spid = options_dict["start_pid"]
    egid = options_dict["end_gid"]
    epid = options_dict["end_pid"]
    cpu_id = rankmap_dict["cpu"]
    rank_id = rankmap_dict["rank"]
    hostname = rankmap_dict["hostname"]
    src_file = f"{options_dict['input']}/{hostname}/r{rank_id}c{cpu_id}.csv"
    # file path for the result data
    dst_file = f"{options_dict['output']}/{hostname}/r{rank_id}c{cpu_id}.csv"
    if os.path.exists(dst_file):
        res_df = pd.read_csv(dst_file)
    else:
        host_prefix = hostname.split(".")[0]
        # read the original data
        src_df = pd.read_csv(src_file)
        col_names = list(src_df.columns.values)
        col_names.remove("gid")
        col_names.remove("pid")
        src_df_sp = src_df[(src_df["gid"] == sgid) & (src_df["pid"] == spid)][col_names]
        src_df_ep = src_df[(src_df["gid"] == egid) & (src_df["pid"] == epid)][col_names]
        src_df_sp.reset_index(inplace=True)
        src_df_ep.reset_index(inplace=True)
        # use the data of (end_gid, end_pid) to minus the data of (start_gid, start_pid)
        res_df = (src_df_ep - src_df_sp)[col_names]
        if options_dict["backend"] == "pfhd":
            res_df["nanosec"] = res_df["nanosec"].apply(lambda x: int(x * 1000000 // constant_rates[host_prefix]))
        # store the result data
        res_df.to_csv(dst_file, index=False)
    return res_df


def get_res_df(options_dict):
    ''' Wrapper function of "parse_rankmap" and "sample_pt_diff"

        Wrapper function of "parse_rankmap" and "sample_pt_diff",
        used to get the dataframe of result data

        Args:
            options_dict: the dict of parsed commandline options.

        Returns:
            res_df: the pandas.DataFrame of the result data
    '''
    rankmap_dict = parse_rankmap(options_dict)
    res_df = sample_pt_diff(options_dict, rankmap_dict)
    return res_df


def cost_measure(options_dict):
    ''' Get the cost of measure function

        Get the cost of measure function and write the result to the output file.
        The path of output file is specified by command line options.

        Args:
            options_dict: the dict of parsed commandline options.
    '''
    cost_res_file = options_dict["fargs"][0]
    res_df = get_res_df(options_dict)
    cost_cycle = res_df["cycle"].min()
    cost_nanosec = res_df["nanosec"].min()
    with open(cost_res_file, "w") as fp:
        fp.write(f"{cost_cycle}\n{cost_nanosec}\n")


def iforest_rm_noise(org_df):
    ''' This function is used to remove noise points from original data

        Use IsolationForest to remove abnormal data points.

        Args:
            org_df: the dataframe contains the orginal data.

        Returns:
            new_df: the dataframe after filtering out noise points.
    '''
    # prepare data and fit the model
    data_cycle = np.array(org_df["cycle"])
    data_nanosec = np.array(org_df["nanosec"])
    data = np.array([data_cycle, data_nanosec]).T
    clf_model = IsolationForest(random_state=47)
    clf_model.fit(data)
    # find the best param
    scores = np.array(clf_model.score_samples(data))
    offset_options = np.arange(0.6, -np.min(scores), 0.01)
    clean_datas = [data[np.where((scores + offset) >= 0)] for offset in offset_options]
    clean_max_cy = [np.max(clean_data[:, 0]) for clean_data in clean_datas]
    clean_max_ns = [np.max(clean_data[:, 1]) for clean_data in clean_datas]
    diff_clean_max_cy = list(np.diff(clean_max_cy))
    diff_clean_max_ns = list(np.diff(clean_max_ns))
    mean_diff_cy = np.mean(diff_clean_max_cy)
    mean_diff_ns = np.mean(diff_clean_max_ns)
    cy_idx = diff_clean_max_cy.index(list(filter(lambda x: x >= mean_diff_cy, diff_clean_max_cy))[0])
    ns_idx = diff_clean_max_ns.index(list(filter(lambda x: x >= mean_diff_ns, diff_clean_max_ns))[0])
    best_clean_data = clean_datas[min(cy_idx, ns_idx)]
    new_df = pd.DataFrame(best_clean_data, columns=["cycle", "nanosec"])
    return new_df


def get_clean_res_df(options_dict):
    ''' Wrapper function of "parse_rankmap", "sample_pt_diff" and "iforest_rm_noise"

        Wrapper function of "parse_rankmap", "sample_pt_diff" and "iforest_rm_noise"
        used to get the dataframe of result data without noise points

        Args:
            options_dict: the dict of parsed commandline options.

        Returns:
            new_res_df: the pandas.DataFrame of the result data without noise points
    '''
    rankmap_dict = parse_rankmap(options_dict)
    cpu_id = rankmap_dict["cpu"]
    rank_id = rankmap_dict["rank"]
    hostname = rankmap_dict["hostname"]
    # file path for the result data without noise points
    clean_dst_file = f"{options_dict['output']}/{hostname}/clean_r{rank_id}c{cpu_id}.csv"
    if not os.path.exists(clean_dst_file):
        res_df = sample_pt_diff(options_dict, rankmap_dict)
        new_res_df = iforest_rm_noise(res_df)
        new_res_df.to_csv(clean_dst_file, index=False)
    else:
        new_res_df = pd.read_csv(clean_dst_file)
    return new_res_df


def least_interval(options_dict):
    ''' The function to find the least measurable interval

        The least measurable interval means that the fluctuations introduced by
        the measurement itself are kept below a certain range. Take the cycle data
        as an example, if the confidence level is 0.95 and error level is 0.02, as long as
        (np.quantile(cycle_data, 0.95) - np.min(cycle_data)) / np.min(cycle_data) < 0.02,
        the condition can be considered satisfied. The final result will be written to
        the output file. The path of output file is specified by command line options.

        Args:
            options_dict: the dict of parsed commandline options.
    '''
    test_round = int(options_dict["fargs"][0])
    cost_res_file = options_dict["fargs"][1]
    interval_res_file = options_dict["fargs"][2]
    # get the measure cost
    with open(cost_res_file, 'r') as fp:
        lines = fp.readlines()
        cost_cycle = int(lines[0].strip())
        cost_nanosec = int(lines[1].strip())
    # get the data after filtering out noise points
    new_res_df = get_clean_res_df(options_dict)
    all_cycles = np.array(new_res_df['cycle'], dtype=np.int64) - cost_cycle
    all_nanosecs = np.array(new_res_df['nanosec'], dtype=np.int64) - cost_nanosec
    mean_cycles = np.mean(all_cycles)
    mean_nanosecs = np.mean(all_nanosecs)
    std_cycles = np.std(all_cycles)
    std_nanosecs = np.std(all_nanosecs)
    # judge whether the conditions are satisfied
    cond_1 = ((std_cycles / mean_cycles) <= error_level)
    cond_2 = ((std_nanosecs / mean_nanosecs) <= error_level)
    with open(interval_res_file, 'w') as fp:
        if cond_1 and cond_2:
            fp.write(f"yes\n{test_round}\n{np.min(all_cycles)}\n{np.min(all_nanosecs)}\n")
            print(f"Round {test_round} passed cycle: ({round(std_cycles, 2)}) / {round(mean_cycles, 2)} <= {error_level}")
            print(f"Round {test_round} passed nanosec: ({round(std_nanosecs, 2)}) / {round(mean_nanosecs, 2)} <= {error_level}")
        else:
            fp.write("no\n")
            if not cond_1:
                print(f"Round {test_round} failed cycle: ({round(std_cycles, 2)}) / {round(mean_cycles, 2)} > {error_level}")
            else:
                print(f"Round {test_round} passed cycle: ({round(std_cycles, 2)}) / {round(mean_cycles, 2)} <= {error_level}")
            if not cond_2:
                print(f"Round {test_round} failed nanosec: ({round(std_nanosecs, 2)}) / {round(mean_nanosecs, 2)} > {error_level}")
            else:
                print(f"Round {test_round} passed nanosec: ({round(std_nanosecs, 2)}) / {round(mean_nanosecs, 2)} <= {error_level}")


def variation_judge(round_1_df, round_2_df, cost, indicator):
    ''' The help function of "variation resolution"

        Given two measurement data, determine whether they are distinguishable,
        and calculate the wasserstein distance of the two data distributions.

        Args:
            round_1_df: the dataframe of the first measurement data
            round_2_df: the dataframe of the second measurement data
            cost: the cost of measurement
            indicator: Controls whether to use cycle data or nanosec data

        Returns:
            cond: whether the two measurement data are distinguishable
            dist_w: the wasserstein distance of the two data distributions
    '''
    round_1_data = np.array(round_1_df[indicator], dtype=np.int64) - cost
    round_2_data = np.array(round_2_df[indicator], dtype=np.int64) - cost
    # calculate the overlap area
    count_data1 = sorted(Counter(round_1_data).items(), key=lambda x: x[0])
    count_data2 = sorted(Counter(round_2_data).items(), key=lambda x: x[0])
    data1_key = np.array([x[0] for x in count_data1])
    data1_val = np.array([x[1] for x in count_data1])
    data2_key = np.array([x[0] for x in count_data2])
    data2_val = np.array([x[1] for x in count_data2])
    data1_val = data1_val / np.sum(data1_val)
    data2_val = data2_val / np.sum(data2_val)
    dict_data1 = dict(zip(data1_key, data1_val))
    dict_data2 = dict(zip(data2_key, data2_val))
    overlap = 0
    for key2, value2 in dict_data2.items():
        if key2 in data1_key:
            overlap += min(dict_data1[key2], value2)
    # the condition that the two data can be distinguished
    cond = (overlap <= confidence_level)
    dist_w = wasserstein_distance(round_1_data, round_2_data)
    return cond, dist_w


def variation_resolution(options_dict):
    ''' The function to get the resolution

        Given the least measurable round (r) and the delta round (k),
        this function will process the data of round [r, r + k, r + 2k, ..., r + 30k],
        and then check whether the data of round r + i * k and round r + (i + 1) * k is
        distinguishable (i = 0, 1, ..., 29). If this condition is satisfied, the result
        of resolution will be written to the output file. The path of output file is
        specified by command line options.

        Args:
            options_dict: the dict of parsed commandline options.
    '''
    k = int(options_dict["fargs"][0])
    least_round = int(options_dict["fargs"][1])
    cost_res_file = options_dict["fargs"][2]
    resolution_res_file = options_dict["fargs"][3]
    if "cycle" in resolution_res_file:
        indicator = "cycle"
    elif "nanosec" in resolution_res_file:
        indicator = "nanosec"
    else:
        print(f"invalid resolution file name {resolution_res_file}")
        sys.exit(-1)
    # get the measure cost
    with open(cost_res_file, 'r') as fp:
        lines = fp.readlines()
        if indicator == "cycle":
            # cost_cycle
            cost = int(lines[0].strip())
        else:
            # cost_nanosec
            cost = int(lines[1].strip())

    cond_res = list()
    dist_w_res = list()
    input_split = options_dict["input"].split('/')
    end_round = int(input_split[-2].split('_')[-1])
    rounds = [x for x in range(least_round, end_round + 1, k)]
    test_num = len(rounds) - 1
    copy_options_dict_list = [copy.deepcopy(options_dict) for i in range(test_num + 1)]
    for i, my_options_dict in enumerate(copy_options_dict_list):
        input_split[-2] = f"cydelay_{rounds[i]}"
        my_options_dict["input"] = '/'.join(input_split)
        my_options_dict["output"] = f'{my_options_dict["input"]}/{my_options_dict["output_suffix"]}'
    with multiprocessing.Pool(processes=ncpus) as pool:
        round_dfs = pool.map(get_clean_res_df, copy_options_dict_list)
    for i in range(test_num):
        round_1_df = round_dfs[i]
        round_2_df = round_dfs[i + 1]
        cond, dist_w = variation_judge(round_1_df, round_2_df, cost, indicator)
        cond_res.append(cond)
        dist_w_res.append(dist_w)
    with open(resolution_res_file, 'w') as fp:
        if np.sum(cond_res) == test_num:
            indicator_resolution = int(np.median(dist_w_res))
            indicator_resolution_std = int(np.std(dist_w_res))
            fp.write(f"yes\n{indicator_resolution}\n{indicator_resolution_std}\n")
            print(f"resolution {k} round passed, {indicator_resolution} {indicator}s")
        else:
            fp.write("no\n")
            print(f"resolution {k} round failed, {np.sum(cond_res)} of {test_num}")


def main():
    ''' The main function '''
    options_dict = parse_option()
    if options_dict["function"] == "cost_measure":
        cost_measure(options_dict)
    elif options_dict["function"] == "least_interval":
        least_interval(options_dict)
    elif options_dict["function"] == "variation_resolution":
        variation_resolution(options_dict)


if __name__ == '__main__':
    main()
