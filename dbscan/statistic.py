import numpy as np
import matplotlib.pyplot as plt


def visualize_results(means, stds, xpos, name):
    # with each numprocess, draw a line chart.
    fix, ax = plt.subplots()
    ax.bar(xpos, means, yerr=stds, align='center', alpha=0.8, ecolor="black", capsize=5, width=0.3)
    ax.set_ylabel("Time (micorseconds)")
    ax.set_xticks(xpos)
    ax.set_xlabel("Number of processors")
    ax.grid(True)
    plt.tight_layout()
    plt.savefig("plots/Cirle_{}.png".format(name))
    plt.show()
    plt.close()



def visualize_results_linechart(means, stds, xpos, name):
    # with each numprocess, draw a line chart.
    fix, ax = plt.subplots()
    # ax.bar(xpos, means, yerr=stds, align='center', alpha=0.8, ecolor="black", capsize=5, width=0.3)
    ax.errorbar(xpos, means, yerr=stds)
    ax.set_ylabel("Time (micorseconds)")
    ax.set_xticks(xpos)
    ax.set_xlabel("Number of data-point")
    ax.grid(True)
    plt.tight_layout()
    plt.savefig("plots/Cirle_numpros{}_data.png".format(name))
    plt.show()
    plt.close()



if __name__ == "__main__":

    statistic = {}
    for dataset in [1, 5, 10, 15, 20]:
        for lol in [1, 2, 3, 4]:
            data = []
            if lol == 1:
                for iter in range(1, 7):
                    data_path = "output_ori/Iter{}_ori_datafile{}.txt".format(iter, dataset)
                    with open(data_path, "r", encoding="utf-8") as file:
                        last_line = ""
                        for line in file:
                            last_line = line
                    file.close()
                    data_line = last_line.split()
                    for dt in data_line:
                        if dt.isdigit():
                            data.append(int(dt))
            else:
                for iter in range(1, 7):
                    data_path = "output/Iter{}_np{}_datafile{}.txt".format(iter, lol, dataset)
                    with open(data_path, "r", encoding="utf-8") as file:
                        last_line = ""
                        for line in file:
                            last_line = line
                    file.close()
                    data_line = last_line.split()
                    for dt in data_line:
                        if dt.isdigit():
                            data.append(int(dt))
            mean = np.mean(data)
            std = np.std(data)
            if dataset not in statistic.keys():
                statistic[dataset] = {lol: [mean, std]}
            else:
                statistic[dataset][lol] = [mean, std]
    
    for dataset in [1, 5, 10, 15, 20]:
        data_i = statistic[dataset]
        means = []
        stds = []
        xpos = list(data_i.keys())
        for key, value in data_i.items():
            means.append(value[0])
            stds.append(value[1])
        visualize_results(means, stds, xpos, dataset)


    for lol in [1, 2, 3, 4]:
        means = []
        stds = []
        xpos = ["1k", "5k", "10k", "15k", "20k"]
        for dataset in statistic:
            means.append(statistic[dataset][lol][0])
            stds.append(statistic[dataset][lol][1])
        visualize_results_linechart(means, stds, xpos, lol)
        
        
    # for dataset in [1, 5, 10, 15, 20]:
    #     data_i = statistic[dataset]
    #     means = []
    #     stds = []
    #     xpos = list(data_i.keys())
    #     for key, value in data_i.items():
    #         means.append(value[0])
    #         stds.append(value[1])
    #     visualize_results(means, stds, xpos, dataset)

            
            
                    

            