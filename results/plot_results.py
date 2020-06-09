import os
import matplotlib.pyplot as plt
import matplotlib.lines as mlines
from collections import defaultdict
import numpy as np
from scipy.stats import sem

RESULT_DIR = 'data'
SERIAL = 'serial'
OMP = 'omp'
MPI = 'mpi'
HYBRID = 'hybrid'
REG = 'reg'
CLARA = 'clara'
METHOD_MAP = {REG: 'PAM',
              CLARA: 'CLARA'}
SERIAL_COLOR = 'orange'
PARALLEL_COLOR = 'blue'
Y_SEC_LABEL = 'Time (s)'
Y_MS_LABEL = 'Time (ms)'


def parse_times():
    times = defaultdict(list)
    for filename in os.listdir(RESULT_DIR):
        with open(os.path.join(RESULT_DIR, filename), 'r', encoding='windows-1252') as file:
            for line in file.readlines():
                if 'wall' in line:
                    line = line.split('s')  # s for seconds appended to time in seconds
                    time = float(line[0].strip(' '))
                    times[filename].append(time)
    return times


def plot_serial(num_procs, times, method):
    _, y_vals, _ = get_x_and_y(times, SERIAL, method)
    y_vals, _ = scale(y_vals)
    plt.plot([1] + list(num_procs), [y_vals for _ in range(1 + len(num_procs))], color=SERIAL_COLOR)


def scale(times):
    if all([True if time < 1.5 else False for time in times]):
        return [time * 1000 for time in times], 'ms'
    else:
        return times, 's'


def plot_times(times, parallelism, method):
    x_vals, y_vals, errors = get_x_and_y(times, parallelism, method)
    y_vals, time_scale = scale(y_vals)
    if time_scale == 'ms':
        errors, _ = scale(errors)
    labels = [1] + list(x_vals)

    if parallelism == MPI:
        x_vals = list(range(2, len(x_vals) + 2))
        plt.xticks([1] + x_vals, labels=labels)
    else:
        plt.xticks(labels)
    plot_serial(x_vals, times, method)
    result = plt.scatter(x_vals, y_vals, cmap=PARALLEL_COLOR)
    plt.errorbar(x_vals, y_vals, yerr=errors, ecolor=result.get_facecolor()[0])
    plt.title(METHOD_MAP[method] + ' ' + parallelism.upper())
    plt.ylabel(Y_SEC_LABEL if time_scale == 's' else Y_MS_LABEL)
    serial = mlines.Line2D([], [], color=SERIAL_COLOR, linestyle='-',
                           markersize=15, label=SERIAL.upper())
    parallel = mlines.Line2D([], [], color=result.get_facecolor()[0], linestyle='-',
                             markersize=15, label=parallelism.upper())
    if parallelism == MPI:
        plt.legend(handles=[serial, parallel])
    else:
        plt.legend(handles=[serial, parallel], loc='center right')

    if parallelism == OMP:
        plt.xlabel('Threads')
    elif parallelism == MPI:
        plt.xlabel('Procs')
    elif parallelism == HYBRID:
        plt.xlabel('Nodes')

    plt.show()
    # plt.savefig(parallelism + '_' + method + '.png', dpi=1400)


def get_x_and_y(times, parallelism, method):
    x_vals = []
    y_vals = []
    errors = []
    for key, value in times.items():
        if parallelism in key and method in key:
            num_procs = key.split('_')[-1].split('.')[0]
            try:
                x_vals.append(int(num_procs))
            except ValueError:
                x_vals.append(1)
            y_vals.append(np.average(value))
            errors.append(sem(value))

    x_vals, y_vals, errors = zip(*sorted(zip(x_vals, y_vals, errors), key=lambda x: x[0]))
    return x_vals, y_vals, errors


def calc_speedup(times, parallelism, method):
    x_vals, y_vals, errors = get_x_and_y(times, parallelism, method)
    _, serial_y_vals, _ = get_x_and_y(times, SERIAL, method)
    idx = np.argmin(y_vals)
    print(f'Lowest time is {y_vals[idx]} with num threads: {x_vals[idx]}')
    print(f'Speed up compared to serial is: {serial_y_vals[0] / y_vals[idx]}')


def calc_speedup_pam_clara(times):
    _, reg_times, _ = get_x_and_y(times, SERIAL, REG)
    _, clara_times, _ = get_x_and_y(times, SERIAL, CLARA)
    print(f'CLARA speedup compared to PAM: {reg_times[0] / clara_times[0]}')


if __name__ == "__main__":
    times = parse_times()
    calc_speedup(times, MPI, CLARA)
    calc_speedup_pam_clara(times)
    plot_times(times, OMP, REG)
    plot_times(times, OMP, CLARA)
    plot_times(times, MPI, CLARA)
    plot_times(times, HYBRID, CLARA)
