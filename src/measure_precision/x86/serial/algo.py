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


# frequency = 2.6
confidence_level = 0.05
data_level = 1 - confidence_level
ncpus = multiprocessing.cpu_count()
constant_rates = {"cas114": 2494103, "cas113": 249414}
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


def IsolationForest_filter(param, data):
    ''' The help function of "rm_noise"

        Given the original data and a parameter, use the parameter to run
        IsolationForest and get the data after filtering out noise points.
        Also, we will calculate the point density corresponding to this parameter.
        The point density is (the number of filtered points) / ((max_cycle - min_cycle)*(max_ns - max_ns)).

        Args:
            param: the value for the contamination parameter of IsolationForest
            data: the original data

        Returns:
            density: the point density corresponding to the parameter
    '''
    if param == 0:
        org_cycle_width = np.max(data[:, 0]) - np.min(data[:, 0])
        org_nanosec_width = np.max(data[:, 1]) - np.min(data[:, 1])
        density = (data.shape[0] / (org_cycle_width * org_nanosec_width))
    else:
        labels = IsolationForest(random_state=42, contamination=param).fit_predict(data)
        # label == 1 means the point is normal point
        target_idx = np.array([(label == 1) for label in labels])
        new_data = data[target_idx]
        cycle_width = np.max(new_data[:, 0]) - np.min(new_data[:, 0])
        nanosec_width = np.max(new_data[:, 1]) - np.min(new_data[:, 1])
        if (int(cycle_width) == 0) or (int(nanosec_width) == 0):
            density = 1e9
        else:
            density = (new_data.shape[0] / (cycle_width * nanosec_width))
    return density


def rm_noise(org_df, multi_proc):
    ''' This function is used to remove noise points from original data

        Use ten-point search method (similar to a binary search) to search for the
        best parameters of IsolationForest. For example, in the first round, we
        iterate over the parameters [0, 0.01, 0.02, ..., 0.09, 0.1] in steps of 0.01,
        and in the second round, we traverse the parameters [0.03, 0.031, ..., 0.039, 0.4]
        in the steps of 0.001. The step size is controlled by the variable "factor". For
        each parameter, we calculate its corresponding point density (see the description in
        function `IsolationForest_filter`). The best parameter of each round appears where
        there is a sudden change in the point density.

        After finding the best parameter, we then use the IsolationForest to filter the noise
        points in the original data.

        Args:
            org_df: the dataframe contains the orginal data.
            multi_proc: use multiprocessing to accelerate or not.

        Returns:
            new_df: the dataframe after filtering out noise points.
    '''
    start = 0
    factor = 100
    data_cycle = np.array(org_df["cycle"])
    data_nanosec = np.array(org_df["nanosec"])
    data = np.array([data_cycle, data_nanosec]).T
    if multi_proc:
        partial_func = partial(IsolationForest_filter, data=data)
        # use multi process to accelerate
        with multiprocessing.Pool(processes=ncpus) as pool:
            # use ten-point search method to search the best parameters
            while factor <= 1000:
                params = [(x / factor) + start for x in range(11)]
                density_results = pool.map(partial_func, params)
                start = params[np.argmax(np.diff(density_results)) + 1]
                factor *= 10
    else:
        while factor <= 1000:
            params = [(x / factor) + start for x in range(11)]
            density_results = [IsolationForest_filter(param, data) for param in params]
            start = params[np.argmax(np.diff(density_results)) + 1]
            factor *= 10
    best_param = start
    labels = IsolationForest(random_state=42, contamination=best_param).fit_predict(data)
    tmp_idx = np.array([(label == 1) for label in labels])
    tmp_data = data[tmp_idx]
    tmp_max_cycle = np.max(tmp_data[:, 0])
    tmp_max_nanosec = np.max(tmp_data[:, 1])
    noise_idx = [i for i in range(len(labels)) if (labels[i] != 1)]
    # classify some eligible noise points as normal points
    for idx in noise_idx:
        if (data[idx, 0] <= tmp_max_cycle) and (data[idx, 1] <= tmp_max_nanosec):
            tmp_idx[idx] = True
    new_df = pd.DataFrame(data[tmp_idx], columns=["cycle", "nanosec"])
    return new_df


def get_clean_res_df(options_dict, multi_proc):
    ''' Wrapper function of "parse_rankmap", "sample_pt_diff" and "rm_noise"

        Wrapper function of "parse_rankmap", "sample_pt_diff" and "rm_noise"
        used to get the dataframe of result data without noise points

        Args:
            options_dict: the dict of parsed commandline options.
            multi_proc: use multiprocessing to accelerate `rm_noise` function or not.

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
        new_res_df = rm_noise(res_df, multi_proc)
        new_res_df.to_csv(clean_dst_file, index=False)
    else:
        new_res_df = pd.read_csv(clean_dst_file)
    return new_res_df


def least_interval(options_dict):
    ''' The function to find the least measurable interval

        The least measurable interval means that the fluctuations introduced by
        the measurement itself are kept below a certain range. Take the cycle data
        as an example, take the confidence level as 0.05, as long as
        (np.quantile(cycle_data, 0.95) - np.min(cycle_data)) / np.min(cycle_data) < 0.05,
        the condition can be considered satisfied. The final result will be written to the
        output file. The path of output file is specified by command line options.

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
    new_res_df = get_clean_res_df(options_dict, True)
    all_cycles = np.array(new_res_df['cycle'], dtype=np.int64) - cost_cycle
    all_nanosecs = np.array(new_res_df['nanosec'], dtype=np.int64) - cost_nanosec
    min_cycle = np.min(all_cycles)
    min_nanosec = np.min(all_nanosecs)
    percent_cycle = np.quantile(all_cycles, data_level)
    percent_nanosec = np.quantile(all_nanosecs, data_level)
    interval_cycle = percent_cycle - min_cycle
    interval_nanosec = percent_nanosec - min_nanosec
    # judge whether the conditions are satisfied
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
    # the condition that the two data can be distinguished
    cond = (np.min(round_2_data) > np.quantile(round_1_data, 1 - confidence_level))
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
    copy_options_dict = [copy.deepcopy(options_dict) for i in range(test_num + 1)]
    for i in range(test_num + 1):
        input_split[-2] = f"round_{rounds[i]}"
        copy_options_dict[i]["input"] = '/'.join(input_split)
        copy_options_dict[i]["output"] = f'{copy_options_dict[i]["input"]}/{copy_options_dict[i]["output_suffix"]}'
    with multiprocessing.Pool(processes=ncpus) as pool:
        partial_func = partial(get_clean_res_df, multi_proc=False)
        round_dfs = pool.map(partial_func, copy_options_dict)
    for i in range(test_num):
        round_1_df = round_dfs[i]
        round_2_df = round_dfs[i + 1]
        cond, dist_w = variation_judge(round_1_df, round_2_df, cost, indicator)
        cond_res.append(cond)
        dist_w_res.append(dist_w)
    with open(resolution_res_file, 'w') as fp:
        if np.sum(cond_res) == test_num:
            indicator_resolution = int(np.median(dist_w_res))
            fp.write(f"yes\n{indicator_resolution}\n")
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
