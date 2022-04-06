import os
import sys
import getopt
import numpy as np
import pandas as pd
from mpi4py import MPI
from scipy import stats


def filter_sigma(args_list):
    # df[(np.abs(stats.zscore(df)) < 3).all(axis=1)]
    pass


def usage():
    print("This script is used to process the result csv files of PerfHound")
    print("-h or --help     print this message")
    print("-s or --start    <start gid>,<start pid>")
    print("-e or --end      <end gid>,<end_pid>")
    print("-i or --input    <run data directory>")
    print("-b or --backend  <pfh or papi>")
    print("-f or --function <apply some data process functions (split with comma)>")
    print("-a or --fargs    <arguments of the data process functions (split with comma)>")


def parse_option():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hs:e:i:b:f:a",
                                   ["start=", "end=", "input=", "backend=", "function=", "fargs="])
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
            options_dict["function"] = [x for x in arg.split(',') if x]
        elif opt in ('-a', '--fargs'):
            options_dict["fargs"] = [x for x in arg.split(',') if x]
    options_dict["output"] = f'{options_dict["input"]}/diff_{sgid}-{spid}_{egid}-{epid}'
    
    if myrank == 0:
        print(f'Backend use {options_dict["backend"]}')
        print(f'start gid {options_dict["start_gid"]}, start pid {options_dict["start_pid"]}')
        print(f'end gid {options_dict["end_gid"]}, end pid {options_dict["end_pid"]}')

    if not os.path.exists(options_dict["input"]):
        print(f"The path {options_dict['input']} does not exist")
        sys.exit(-1)
    return options_dict


def parse_rankmap(options_dict):
    input_dir = options_dict["input"]
    rankmap_path = f"{input_dir}/rankmap.csv"
    rankmap_df = pd.read_csv(rankmap_path)
    total_rows = rankmap_df.shape[0]
    
    mod = total_rows % nprocs
    local_nf = total_rows // nprocs
    if myrank < mod:
        local_nf += 1
        local_start = local_nf * myrank
    else:
        local_start = local_nf * myrank + mod
    local_end = local_start + local_nf

    rankmap_dict = dict()
    for key in ["rank", "cpu", "hostname"]:
        rankmap_dict[key] = rankmap_df[key].values[local_start:local_end]

    if myrank == 0:
        print(f"There are {total_rows} rank records.")
        unique_hostnames = list(set(rankmap_df["hostname"]))
        for hostname in unique_hostnames:
            dir_path = f"{options_dict['output']}/{hostname}"
            if not os.path.exists(dir_path):
                os.makedirs(dir_path)
    comm.Barrier()

    return rankmap_dict


def interval2csv(options_dict, rankmap_dict):
    sgid = options_dict["start_gid"]
    spid = options_dict["start_pid"]
    egid = options_dict["end_gid"]
    epid = options_dict["end_pid"]
    cpu_ids = rankmap_dict["cpu"]
    rank_ids = rankmap_dict["rank"]
    hostnames = rankmap_dict["hostname"]
    local_nf = rank_ids.shape[0]
    for i in range(local_nf):
        src_file = f"{options_dict['input']}/{hostnames[i]}/r{rank_ids[i]}c{cpu_ids[i]}.csv"
        dst_file = f"{options_dict['output']}/{hostnames[i]}/r{rank_ids[i]}c{cpu_ids[i]}.csv"
        host_prefix = hostnames[i].split(".")[0]
        src_df = pd.read_csv(src_file)
        col_names = list(src_df.columns.values)
        col_names.remove("gid")
        col_names.remove("pid")
        src_df_sp = src_df[(src_df["gid"] == sgid) & (src_df["pid"] == spid)][col_names]
        src_df_ep = src_df[(src_df["gid"] == egid) & (src_df["pid"] == epid)][col_names]
        src_df_sp.reset_index(inplace=True)
        src_df_ep.reset_index(inplace=True)
        res_df = (src_df_ep - src_df_sp)[col_names]
        if options_dict["backend"] == "pfh":
            res_df["nanosec"] = res_df["nanosec"].apply(lambda x: int(x * 1000000 // constant_rates[host_prefix]))
        res_df.to_csv(dst_file, index=False)


def main():
    if myrank == 0:
        print("Start to parse command line options.")
    options_dict = parse_option()
    if myrank == 0:
        print("Start to parse rankmap.")
    rankmap_dict = parse_rankmap(options_dict)
    if myrank == 0:
        print("Start to calculate the interval.")
    interval2csv(options_dict, rankmap_dict)


if __name__ == '__main__':
    comm = MPI.COMM_WORLD
    myrank = comm.Get_rank()
    nprocs = comm.Get_size()
    constant_rates = {"cas114": 2494103, "cas113": 249414}
    main()
