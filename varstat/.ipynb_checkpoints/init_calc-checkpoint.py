import os
import sys
import pandas as pd

"""
== Description

VtObject dominates all data in a specific VarAPI project. It can be used as a
data driver for loading and performing basic data analysis operations. It provides
a basic data structure for a VarAPI project. One wants to better utilize the
VarAPI data can apply this class for locating one or more tracing points/groups.

User can locate any tracing point following 'data root - project - run ID - rank ID - timestamp' 
structure. Meanwile the hostname, binding cpu, trace point ID, group ID, rank map
can also be accquire with the VtObject.

VarStat is developed depending on several libraries:
- DASK-2.29.0
- Pandas-1.1.2
- Numpy-1.19.0
- Holoviews-1.13.4
- Datashader-0.11.1
We strongly suggest using Jupyter for data analysis (although it still works in other environments).

== Usages

VarStat helps you to write your own python3 scripts for performance variation analysis.
You only need to 

= Create a VtObject object

VtObject builds a data domain based on paticular VarAPI project. One wants to 
start a VarStat analysis should initialize at least one VtObject using:
import vtstat
vtobj = vtstat.VtObject("/some/data/root", "myproj")

= Initialize data space

= Reload data

= Export data structure

= Statistical Analysis

== Attributes

== Methods

"""
class VtObject:
    def __new__(cls, data_root, proj_name):
        """
        Check if project is valid.
        
        Parameters
        ----------
        data_root: Root path of vartect data.
        proj_name: Project name.
        
        Returns
        -------
        Returning VtObject object if path is valid, NoneType if not.
        """
        data_root = os.path.expanduser(data_root)
        full_path = os.path.join(data_root, proj_name)
        if not os.path.isdir(full_path):
            print("Project data path doesn't exist!")
            return None
        else:
            return object.__new__(cls)
    
    def __init__(self, data_root, proj_name):
        """
        Initialize project name and path.
        
        Parameters
        ----------
        data_root: Root path of vartect data.
        proj_name: Project name.
        """

        self.nrun = 0
        self.nranks = list()
        self.rmaps = list()
        self.name = proj_name
        data_root = os.path.expanduser(data_root)
        self.path = os.path.join(data_root, proj_name)
        self.opath = os.path.join(self.path, 'varstat_data')

    def data_init(self, run_list = [], rank_list = []):
        """
        Convert raw counter number to relative readings.
        
        Parameters
        ----------
        rank_list: Only calculate rank in rank_list.
        run_list: Only calculate run in run_list.
        """ 
        # Get number of runs.
        nrun = 0
        while 1:
            rank_fname = 'run' + str(nrun) + '_rankmap.csv'
            rank_fpath = os.path.join(self.path, rank_fname)
            if not os.path.exists(rank_fpath) :
                break
            else:
                nrun += 1
        print("Project Name: %s\nProject Path: %s\n# of Runs: %d" % (self.name, self.path, self.nrun))
        self.nrun = nrun
        if len(run_list) == 0:
            run_list = [i for i in range(0, self.nrun)]
        
        # Backup old VarStat data if exist
        if os.path.isdir(self.opath):
            i = 1
            while 1:
                bak_path = os.path.join(self.path, 'varstat_data_bak_' + str(i))
                if not os.path.isdir(bak_path):
                    os.rename(os.opath, bak_path)
        os.mkdir(self.opath, 0o0755)
        
        # Get # of processes in each run
        rank_fpaths = list()
        for run in run_list:
            print("Processing run %d..." % (run))

            # read rank maps
            rank_fname = 'run' + str(run) + '_rankmap.csv'
            rank_fpath = os.path.join(self.path, rank_fname)
            rank_fpaths.append(rank_fpath)
            df = pd.read_csv(rank_fpath)
            self.rmaps.append(df)
            self.nranks.append(len(df.index))                        
            if len(rank_list) == 0:
                rank_list = [r for r in range(0, self.nranks[run])]
            
            
            for rank in rank_list:
                self.abs_to_rel(run, rank)
    
    def abs_to_rel(self, run, rank):
        """
        Convert raw data file of <rank> in <run> to relative readings.
        """
        cpu = self.rmaps[run].iloc[rank]['cpu']
        host = self.rmaps[run].iloc[rank]['hostname']
        fname = 'run' + str(run) + '_r' + str(rank) + '_c' + str(cpu) + '_all.csv'
        fpath = os.path.join(self.path, host)
        fpath = os.path.join(fpath, fname)
        df = pd.read_csv(fpath)
        # relative_value[x] = raw[x] - raw[0]
        df['cycle'] = df['cycle'] - df.iloc[0]['cycle']
        df['nanosec'] = df['nanosec'] - df.iloc[0]['nanosec']
        # Write to csv
        out_csv_path = os.path.join(self.opath, host)
        if not os.path.isdir(out_csv_path):
            os.mkdir(out_csv_path, 0o755)
        out_csv_path = os.path.join(out_csv_path, fname)
        df.to_csv(path_or_buf=out_csv_path, index=False)
