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
    ''' Process single rank's original data

        Help function of `sample_pt_diff_all`, process single rank's
        original data according to the command line options, and
        write the result data to the output directory.

        Args:
            idx: controls which rank's data to process.
            options_dict: the dict of parsed commandline options.
            rankmap_dict: the dict of parsed rankmap.csv file.
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
        res_df["nanosec"] = res_df["nanosec"].apply(lambda x: int(x * 1000000 // constant_rates[host_prefix]))
    # store the result data
    res_df.to_csv(dst_file, index=False)


def sample_pt_diff_all(options_dict, rankmap_dict):
    ''' Process the original data of all ranks

        Process all original data according to the command line
        options, and write all result data to the output directory.
        Use multi-process to accelerate.

        Args:
            options_dict: the dict of parsed commandline options.
            rankmap_dict: the dict of parsed rankmap.csv file.
    '''
    idxs = [x for x in range(rankmap_dict["rank"].shape[0])]
    partial_func = partial(sample_pt_diff_single, options_dict=options_dict, rankmap_dict=rankmap_dict)
    # use multi process to accelerate
    with multiprocessing.Pool(processes=ncpus) as pool:
        pool.map(partial_func, idxs)


def get_res_df(core_id, options_dict):
    ''' Given the cpu core id, get the corresponding processed dataframe

        Wrapper function of "parse_rankmap" and "sample_pt_diff_all",
        used to get the result dataframe of specified cpu core.

        Args:
            core_id: specified id of cpu core.
            options_dict: the dict of parsed commandline options.

        Returns:
            res_df: result dataframe of specified cpu core.
    '''
    rankmap_dict = parse_rankmap(options_dict)
    cpu_id = core_id
    idx = np.where(rankmap_dict["cpu"] == core_id)
    rank_id = rankmap_dict["rank"][idx][0]
    hostname = rankmap_dict["hostname"][idx][0]
    res_file = f"{options_dict['output']}/{hostname}/r{rank_id}c{cpu_id}.csv"
    if not os.path.exists(res_file):
        sample_pt_diff_all(options_dict, rankmap_dict)
    res_df = pd.read_csv(res_file)
    return res_df


def IsolationForest_filter(param, data):
    ''' The help function of "rm_noise_single"

        Given the original data and a parameter, use the parameter to run
        IsolationForest and get the data after filtering out noise points.
        Also, we will calculate the point density corresponding to this parameter.
        The point density is (the number of filtered points) / ((max_cycle - min_cycle)*(max_ns - max_ns)).

        Args:
            param: the value for the contamination parameter of IsolationForest.
            data: the original data.

        Returns:
            density: the point density corresponding to the parameter.
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
        cycle_nanosec = np.max(new_data[:, 1]) - np.min(new_data[:, 1])
        density = (new_data.shape[0] / (cycle_width * cycle_nanosec))
    return density


def rm_noise_single(idx, options_dict, rankmap_dict):
    ''' The function to remove the noise points for single rank's dataframe

        Help function of `rm_noise_all`.
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
            idx: controls which rank's data to process.
            options_dict: the dict of parsed commandline options.
            rankmap_dict: the dict of parsed rankmap.csv file.
    '''
    cpu_id = rankmap_dict["cpu"][idx]
    rank_id = rankmap_dict["rank"][idx]
    hostname = rankmap_dict["hostname"][idx]
    # file path for the original result dataframe
    org_df_path = f"{options_dict['output']}/{hostname}/r{rank_id}c{cpu_id}.csv"
    # file path for the result data without noise points
    clean_df_path = f"{options_dict['output']}/{hostname}/clean_r{rank_id}c{cpu_id}.csv"

    # start to remove noise points
    start = 0
    factor = 100
    org_df = pd.read_csv(org_df_path)
    data_cycle = np.array(org_df["cycle"])
    data_nanosec = np.array(org_df["nanosec"])
    data = np.array([data_cycle, data_nanosec]).T
    while factor <= 1000:
        params = [(x / factor) + start for x in range(11)]
        density_results = [IsolationForest_filter(param, data) for param in params]
        start = params[np.argmax(np.diff(density_results))]
        factor *= 10
    best_param = (start + (10 / factor))
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
    # store the result data without noise points
    new_df.to_csv(clean_df_path, index=False)


def rm_noise_all(options_dict, rankmap_dict):
    ''' Remove noise points for all ranks' dataframe

        Remove noise points for all ranks' dataframe, and write all
        result data to the output directory. Use multi-process to accelerate.

        Args:
            options_dict: the dict of parsed commandline options.
            rankmap_dict: the dict of parsed rankmap.csv file.
    '''
    idxs = [x for x in range(rankmap_dict["rank"].shape[0])]
    partial_func = partial(rm_noise_single, options_dict=options_dict, rankmap_dict=rankmap_dict)
    # use multi process to accelerate
    with multiprocessing.Pool(processes=ncpus) as pool:
        pool.map(partial_func, idxs)


def get_res_df_no_noise(core_id, options_dict):
    ''' Given the cpu core id, get the corresponding result dataframe without noise points

        Wrapper function of "parse_rankmap", "sample_pt_diff_all" and "rm_noise_all",
        used to get the noise free dataframe of specified cpu core.

        Args:
            core_id: specified id of cpu core.
            options_dict: the dict of parsed commandline options.

        Returns:
            clean_res_df: pandas.DataFrame of the result data without noise points
    '''
    rankmap_dict = parse_rankmap(options_dict)
    cpu_id = core_id
    idx = np.where(rankmap_dict["cpu"] == core_id)
    rank_id = rankmap_dict["rank"][idx][0]
    hostname = rankmap_dict["hostname"][idx][0]
    clean_res_file = f"{options_dict['output']}/{hostname}/clean_r{rank_id}c{cpu_id}.csv"
    if not os.path.exists(clean_res_file):
        sample_pt_diff_all(options_dict, rankmap_dict)
        rm_noise_all(options_dict, rankmap_dict)
    clean_res_df = pd.read_csv(clean_res_file)
    return clean_res_df


def cost_measure(options_dict):
    ''' Get the cost of measure function

        Get the cost of measure function and write the result to the output file.
        The path of output file is specified by command line options. The path of
        output file contains the id of cpu core.

        Args:
            options_dict: the dict of parsed commandline options.
    '''
    cost_res_file = options_dict["fargs"][0]
    # get the id of cpu core
    core_id = int(cost_res_file.split('.')[0].split('_')[-1])
    res_df = get_res_df(core_id, options_dict)
    cost_cycle = res_df["cycle"].min()
    cost_nanosec = res_df["nanosec"].min()
    with open(cost_res_file, "w") as fp:
        fp.write(f"{cost_cycle}\n{cost_nanosec}\n")


def least_interval(options_dict):
    ''' The function to find the least measurable interval

        The least measurable interval means that the fluctuations introduced by
        the measurement itself are kept below a certain range. Take the cycle data
        as an example, take the confidence level as 0.05, as long as
        (np.quantile(cycle_data, 0.95) - np.min(cycle_data)) / np.min(cycle_data) < 0.05,
        the condition can be considered satisfied. The final result will be written to the
        output file. The path of output file is specified by command line options. The path
        of output file contains the id of cpu core.

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
    # get the id of cpu core
    core_id = int(interval_res_file.split('.')[0].split('_')[-1])
    # get the data without noise points
    clean_res_df = get_res_df_no_noise(core_id, options_dict)
    all_cycles = np.array(clean_res_df['cycle'], dtype=np.int64) - cost_cycle
    all_nanosecs = np.array(clean_res_df['nanosec'], dtype=np.int64) - cost_nanosec
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


def main():
    ''' The main function '''
    options_dict = parse_option()
    if options_dict["function"] == "cost_measure":
        cost_measure(options_dict)
    elif options_dict["function"] == "least_interval":
        least_interval(options_dict)


if __name__ == '__main__':
    main()
