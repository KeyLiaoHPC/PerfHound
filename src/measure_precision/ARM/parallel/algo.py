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


frequency = 2.6
error_level = 0.01
confidence_level = 0.05


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
            options_dict: A dict that contains key-value pairs of command line options.
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
    if not os.path.exists(options_dict["input"]):
        print(f"The path {options_dict['input']} does not exist")
        sys.exit(-1)
    return options_dict


def parse_rankmap(options_dict):
    ''' Parse the file rankmap.csv

        Parse the rankmap.csv and get the info of rank ids, cpu ids and
        hostnames. Then, create the output directory to store result data.

        Args:
            options_dict: the dict of parsed commandline options.

        Returns:
            rankmap_dict: A dict that contains the value of rank ids, cpu ids and hostnames.
    '''
    rankmap_path = f"{options_dict['input']}/rankmap.csv"
    rankmap_df = pd.read_csv(rankmap_path)
    rankmap_dict = dict()
    for key in ["rank", "cpu", "hostname"]:
        rankmap_dict[key] = np.array(rankmap_df[key].values)
    # create the output directory
    unique_hostnames = list(set(rankmap_df["hostname"]))
    for hostname in unique_hostnames:
        dir_path = f"{options_dict['output']}/{hostname}"
        if not os.path.exists(dir_path):
            os.makedirs(dir_path)
    return rankmap_dict


def sample_pt_diff_single(idx, options_dict, rankmap_dict):
    ''' Process single core's original data

        Help function of `get_combine_df`, Process single core's
        original data according to the command line options, and
        write the result data to the output directory.

        Args:
            idx: controls which core's data to process.
            options_dict: the dict of parsed commandline options.
            rankmap_dict: the dict of parsed rankmap.csv file.

        Returns:
            res_df: result dataframe of specified cpu core.
    '''
    egid = options_dict["end_gid"]
    epid = options_dict["end_pid"]
    sgid = options_dict["start_gid"]
    spid = options_dict["start_pid"]
    cpu_id = rankmap_dict["cpu"][idx]
    rank_id = rankmap_dict["rank"][idx]
    hostname = rankmap_dict["hostname"][idx]
    host_prefix = hostname.split(".")[0]
    # file path for the original data
    src_file = f"{options_dict['input']}/{hostname}/r{rank_id}c{cpu_id}.csv"
    # file path for the result data
    dst_file = f"{options_dict['output']}/{hostname}/r{rank_id}c{cpu_id}.csv"
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
        res_df["nanosec"] = res_df["nanosec"].apply(lambda x: int(x * 10))
    # store the result data
    res_df.to_csv(dst_file, index=False)
    return res_df


def get_combine_df(options_dict, pool):
    ''' Get the combined dataframe of all cores

        Process all original data according to the command line
        options, write all result data to the output directory,
        and collect all cores' data in a single dataframe. Use
        multi-process to accelerate.

        Args:
            options_dict: the dict of parsed commandline options.
            pool: the process pool.

        Returns:
            combine_df: result dataframe of specified cpu core.
    '''
    rankmap_dict = parse_rankmap(options_dict)
    # file path for the combined dataframe
    combine_dst_file = f"{options_dict['output']}/combine.csv"
    if os.path.exists(combine_dst_file):
        combine_df = pd.read_csv(combine_dst_file)
    else:
        idxs = [x for x in range(rankmap_dict["rank"].shape[0])]
        partial_func = partial(sample_pt_diff_single, options_dict=options_dict, rankmap_dict=rankmap_dict)
        # use multi process to accelerate
        all_cores_dfs = pool.map(partial_func, idxs)
        combine_df = pd.concat(all_cores_dfs, axis=0)
        combine_df.to_csv(combine_dst_file, index=False)
    return combine_df


def cost_measure(options_dict, pool):
    ''' Get the cost of measure function

        Get the cost of measure function and write the result to the output
        file. The path of output file is specified by command line options.

        Args:
            options_dict: the dict of parsed commandline options.
            pool: the process pool.
    '''
    cost_res_file = options_dict["fargs"][0]
    combine_df = get_combine_df(options_dict, pool)
    cost_cycle = combine_df["cycle"].min()
    cost_nanosec = combine_df["nanosec"].min()
    with open(cost_res_file, "w") as fp:
        fp.write(f"{cost_cycle}\n{cost_nanosec}\n")


def iforest_rm_noise(org_combine_df):
    ''' This function is used to remove noise points from original dataframe

        Use IsolationForest to remove abnormal data points.

        Args:
            org_combine_df: original dataframe.

        Returns:
            clean_combine_df: the dataframe after filtering out noise points.
    '''
    # prepare data and fit the model
    data_cycle = np.array(org_combine_df["cycle"])
    data_nanosec = np.array(org_combine_df["nanosec"])
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
    clean_combine_df = pd.DataFrame(best_clean_data, columns=["cycle", "nanosec"])
    return clean_combine_df


def get_clean_combine_df(options_dict, pool):
    ''' Get the combined dataframe without noise points

        Wrapper function of "get_combine_df" and "iforest_rm_noise",
        used to get the noise free dataframe of all cpu cores.

        Args:
            options_dict: the dict of parsed commandline options.
            pool: the process pool.

        Returns:
            clean_combine_df: pandas.DataFrame of the result data without noise points
    '''
    clean_combine_file = f"{options_dict['output']}/clean_combine.csv"
    if not os.path.exists(clean_combine_file):
        org_combine_df = get_combine_df(options_dict, pool)
        clean_combine_df = iforest_rm_noise(org_combine_df)
        clean_combine_df.to_csv(clean_combine_file, index=False)
    else:
        clean_combine_df = pd.read_csv(clean_combine_file)
    return clean_combine_df


def least_interval(options_dict, pool):
    ''' The function to find the least measurable interval

        The least measurable interval means that the fluctuations introduced by
        the measurement itself are kept below a certain range. Take the cycle data
        as an example, if the confidence level is 0.95 and error level is 0.02, as long as
        (np.quantile(cycle_data, 0.95) - np.min(cycle_data)) / np.min(cycle_data) < 0.02,
        the condition can be considered satisfied. The final result will be written to
        the output file. The path of output file is specified by command line options.

        Args:
            options_dict: the dict of parsed commandline options.
            pool: the process pool.
    '''
    test_round = int(options_dict["fargs"][0])
    cost_res_file = options_dict["fargs"][1]
    interval_res_file = options_dict["fargs"][2]
    # get the measure cost
    with open(cost_res_file, 'r') as fp:
        lines = fp.readlines()
        cost_cycle = int(lines[0].strip())
        cost_nanosec = int(lines[1].strip())
    # get the data without noise points
    clean_combine_df = get_clean_combine_df(options_dict, pool)
    all_cycles = np.array(clean_combine_df['cycle'], dtype=np.int64) - cost_cycle
    all_nanosecs = np.array(clean_combine_df['nanosec'], dtype=np.int64) - cost_nanosec
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
    # cond = (np.quantile(round_1_data, 1 - confidence_level) <= np.min(round_2_data))
    # dist_w = wasserstein_distance(round_1_data, round_2_data)
    return cond


def variation_resolution(options_dict, pool):
    ''' The function to get the measure resolution of a specified cpu core

        Given the least measurable round (r) and the delta round (k),
        this function will process the data of round [r, r + k, r + 2k, ..., r + 30k],
        and then check whether the data of round r + i * k and round r + (i + 1) * k is
        distinguishable (i = 0, 1, ..., 29). If this condition is satisfied, the result
        of resolution will be written to the output file. The path of output file is
        specified by command line options. The path of output file contains the id of
        cpu core.

        Args:
            options_dict: the dict of parsed commandline options.
            pool: the process pool.
    '''
    k = int(options_dict["fargs"][0])
    cost_res_file = options_dict["fargs"][1]
    resolution_res_file = options_dict["fargs"][2]
    if "cycle" in resolution_res_file:
        indicator = "cycle"
        indicator_resolution = k
    elif "nanosec" in resolution_res_file:
        indicator = "nanosec"
        indicator_resolution = int(k / frequency)
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

    round_dfs = list()
    input_split = options_dict["input"].split('/')
    round_2 = int(input_split[-2].split('_')[-1])
    round_1 = round_2 - k
    for myround in [round_1, round_2]:
        input_split[-2] = f"cydelay_{myround}"
        options_dict["input"] = '/'.join(input_split)
        options_dict["output"] = f'{options_dict["input"]}/{options_dict["output_suffix"]}'
        round_dfs.append(get_clean_combine_df(options_dict, pool))
    cond = variation_judge(round_dfs[0], round_dfs[1], cost, indicator)
    with open(resolution_res_file, 'w') as fp:
        if cond == True:
            fp.write(f"yes\n{indicator_resolution}\n")
        else:
            fp.write("no\n")


def main():
    ''' The main function '''
    ncpus = multiprocessing.cpu_count()
    with multiprocessing.Pool(processes=ncpus) as pool:
        options_dict = parse_option()
        if options_dict["function"] == "cost_measure":
            cost_measure(options_dict, pool)
        elif options_dict["function"] == "least_interval":
            least_interval(options_dict, pool)
        elif options_dict["function"] == "variation_resolution":
            variation_resolution(options_dict, pool)


if __name__ == '__main__':
    main()
