""" RTM data plotter script

This script is a simple plotting tool for RTM data provided to it in form of a
(.csv)-file

This file can also be imported as a module and contains the following functions
and class:
    * get_data_from_scan
    * plot_3d
    * plot_single_scan
    * plotter

Examples of Usage:
    >>> ...

License:
    Copyright (C) 2022 Marten Scheuck

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""

import numpy as np
import warnings
import matplotlib.pyplot as plt

from scipy import interpolate
from pathlib import Path
from matplotlib import cm
from typing import Any, Dict, List, Union, Optional


def get_data_from_scan(csv_file: Path, exclude_extrems: Optional[bool] = True,
                       scan_length_x: Optional[int] = 200) -> List:
    """Gets the data from the (.csv)-file and reforms is so it can be used for
    a 3D contour plot, it then returns a list containing the x, y and z
    component as numpy arrays

    Parameters
    ----------
    csv_file: Path
        The path to the (.csv)-file
    exclude_extrems: bool, optional
        If 'True' replaces extreme values with the median value to make the
        plot look better
    scan_length_x: int, optional
        The index/length after the RTM starts a new x-axis for the next 3D
        measurement (important for the refactoring so the 2D-numpy arrays for
        the plot can be generated from the (.csv)-file

    Returns
    -------
    List
        A list containing 2D-np.ndarrays containing the x, y and z
        information
    """
    break_line = None
    x, y, z = map(lambda n: np.array(n).astype(int),
                     map(list, zip(*np.genfromtxt(file, delimiter=";"))))

    for i, o in enumerate(x):
        if i % scan_length_x == 0:
            if o in [0, scan_length_x-1]:
                break_line = i
            if o not in [0, scan_length_x-1]:
                warnings.warn(f"From line {i} datastructure is not of form [0,"\
                              f" {scan_length_x-1}] in x anymore... "\
                              f" Values from then on will be discarded")
                break

    if break_line != x.shape[0]:
        new_dimensions = x[:break_line].shape[0]//scan_length_x
        x, y, z = map(lambda n: n[:break_line], [x, y, z])
    else:
        if x.shape[0] % scan_length_x != 0:
            raise InputError(f"The input (.csv)-file is not of the right form"\
                             f" for the given scan length: {scan_length_x}")
        else:
            new_dimensions = x.shape[0]//scan_length_x

    x, y, z = map(lambda n: n.reshape(new_dimensions, scan_length_x), [x, y, z])

    if exclude_extrems:
        ind_max = np.where(z.copy()/np.mean(z) > 1.)
        z[ind_max] = np.median(z).astype(int)
        warnings.warn(f"Replaced extreme values at indices {ind_max}, with the"\
                      f" median value {np.median(z).astype(int).astype(int)}",
                      category=Warning)
        warnings.warn("CAUTION: The inserted values may lead to wrong"\
                      " estimations of the surface profile",
                      category=FutureWarning)
    print("Refactoring (.csv)-file into dictionary done!")
    return [x, y, z]


def plot_single_scan(fig, data: List) -> None:
    """Plots a single segment (the middle) of the 3D contour"""
    x, _, z = map(lambda x: x[x.shape[0]//2], data)
    ax = fig.add_subplot(2, 1, 2)
    ax.plot(x, z)
    ax.set_xlabel("x")
    ax.set_ylabel("z")
    ax.set_title("Single centre slice")

def plot_3d(fig, data: List, intp: Optional[bool] = True) -> None:
    """Plots the x, y, z from data contained within a dict

    Parameters
    ----------
    data: List
        The data with the x, y, z as 2D-numpy arrays
    intp: bool, optional
        If 'True' then the contour is interpolated for a more smooth look
    """
    ax = fig.add_subplot(2, 1, 1, projection="3d")

    ax.plot_surface(*data, rstride=1, cstride=1,
                    antialiased=True, cmap=cm.coolwarm)

    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    ax.set_title("3D Height profile (antialised)")

def plotter(data: List, plt_save: Optional[bool] = False) -> None:
    """Makes a two page plot of the (.csv)-file's RTM scan input

    Parameters
    ----------
    data_dict: Dict
        The restructured data from the (.csv)-file
    plt_save: bool, optional
        If 'True' saves the plot in the folder, where this script is located
    """
    fig = plt.figure(figsize=(12, 12))
    fig.suptitle("RTM Scan")
    plot_3d(fig, data)
    plot_single_scan(fig, data)

    if plt_save:
        plt.savefig("RTM_scan.png")
    else:
        plt.show()


if __name__ == "__main__":
    file = "newScan.csv"
    data = get_data_from_scan(file)
    plotter(data, plt_save=False)

