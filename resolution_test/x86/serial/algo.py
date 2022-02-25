import os
import sys
import getopt
import numpy as np
import pandas as pd
import scipy.stats as stats

frequency = 2.6
confidence_level = 0.05
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
        opts, args = getopt.getopt(sys.argv[1:], "hs:e:i:b:f:a:", [
                                   "start=", "end=", "input=", "backend=", "function=", "args="])
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
            options_dict["fargs"] = arg
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

    unique_hostnames = list(rankmap_df["hostname"])
    for hostname in unique_hostnames:
        dir_path = f"{options_dict['output']}/{hostname}"
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
    options_dict["dst_file"] = dst_file
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
        res_df["nanosec"] = res_df["nanosec"].apply(
            lambda x: int(x * 1000000 // constant_rates[host_prefix]))
    res_df.to_csv(dst_file, index=False)
    return res_df


def timing_cost(res_df):
    cost_nanosec = res_df["nanosec"].min()
    cost_cycle = res_df["cycle"].min()
    with open("tmp_cost.txt", "w") as fp:
        fp.write(f"{cost_cycle}\n{cost_nanosec}")


def least_interval(res_df, options_dict):
    fargs = [x for x in options_dict["fargs"].split(',') if x]
    test_round = int(fargs[0])
    cost_cycle = int(fargs[1])
    cost_nanosec = int(fargs[2])
    theo_cycle = 2 * test_round * ins_lat["ADD"]
    theo_nanosec = theo_cycle / frequency
    all_cycles = np.array(res_df['cycle'], dtype=np.int64)
    all_cycles -= cost_cycle
    all_cycles.sort()
    all_nanosecs = np.array(res_df['nanosec'], dtype=np.int64)
    all_nanosecs -= cost_nanosec
    all_nanosecs.sort()

    # judge
    nrecords = all_cycles.shape[0]
    nd = int(0.01 * nrecords)
    nu = int(0.99 * nrecords)
    nmedian = int(0.5 * nrecords)
    down_cycles = 0.975 * theo_cycle
    up_cycles = 1.025 * theo_cycle
    down_nanosec = 0.975 * theo_nanosec
    up_nanosec = 1.025 * theo_nanosec
    with open("tmp_least_interval.txt", 'w') as fp:
        if ((all_cycles[nd] >= down_cycles) and (all_cycles[nu] <= up_cycles) and
            (all_nanosecs[nd] >= down_nanosec) and (all_nanosecs[nu] <= up_nanosec)):
            fp.write(f"yes\n{all_cycles[nmedian]}\n{all_nanosecs[nmedian]}")
            print(f"Round {test_round} passed\n")
        else:
            fp.write("no")
            print(f"Round {test_round} failed, theo: [{down_cycles}, {up_cycles}], actual [{all_cycles[nd]}, {all_cycles[nu]}]\n")


def variation_resolution(res_df, options_dict):
    fargs = [x for x in options_dict["fargs"].split(',') if x]
    least_round = int(fargs[0])
    cost_cycle = int(fargs[1])
    cost_nanosec = int(fargs[2])
    dst_file_split = options_dict["dst_file"].split('/')
    current_round = int(dst_file_split[-5].split('_')[-1])
    dst_file_split[-5] = f"round_{least_round}"
    ref_path = '/'.join(dst_file_split)
    ref_df = pd.read_csv(ref_path)
    ref_cycles = np.array(ref_df["cycle"], dtype=np.int64) - cost_cycle
    ref_nanosecs = np.array(ref_df["nanosec"], dtype=np.int64) - cost_nanosec
    res_cycles = np.array(res_df['cycle'], dtype=np.int64) - cost_cycle
    res_nanosecs = np.array(res_df['nanosec'], dtype=np.int64) - cost_nanosec
    _, p_cycle = stats.mannwhitneyu(ref_cycles, res_cycles, alternative='two-sided')
    _, p_nanosec = stats.mannwhitneyu(ref_nanosecs, res_nanosecs, alternative='two-sided')
    with open("tmp_var_resolution.txt", 'w') as fp:
        if p_cycle < confidence_level and p_nanosec < confidence_level:
            dcycle = np.median(res_cycles) - np.median(ref_cycles)
            dnanosec = np.median(res_nanosecs) - np.median(ref_nanosecs)
            fp.write(f"yes\n{dcycle}\n{dnanosec}")
            print(f"Round {current_round} passed\n")
        else:
            fp.write("no")
            print(f"Round {current_round} failed")


def main():
    options_dict = parse_option()
    rankmap_dict = parse_rankmap(options_dict)
    res_df = interval2csv(options_dict, rankmap_dict)
    if options_dict["function"] == "timing_cost":
        timing_cost(res_df)
    elif options_dict["function"] == "least_interval":
        least_interval(res_df, options_dict)
    elif options_dict["function"] == "variation_resolution":
        variation_resolution(res_df, options_dict)


if __name__ == '__main__':
    main()
