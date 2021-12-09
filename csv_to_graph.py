import os
import csv
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

execs = ["huffman", "adaptive-huffman", "rle", "efficientrle", "lz77", "lz78"]

crs = []
datasets = []
for ex in execs:
    with open("./csvs/" + ex + ".csv", 'r', newline='') as file:
        csvLines = csv.reader(file)
        cr = []
        dataset = []
        for line in list(csvLines)[1:26]:
            cr.append(float(line[4]) * 100)
            dataset.append(line[0])
        
        datasets.append(dataset)
        crs.append(cr)

plotdata = {}
for i in range(len(execs)):
    plotdata.update({execs[i] : crs[i]})

plotd = pd.DataFrame(plotdata)
plotd['Dataset'] = datasets[0][:26]

# plotd.plot.bar(x = "Dataset", rot=0)
# plt.show()

for i in range(5):
    plotd_new = plotd.iloc[(i*5):((i+1)*5)]
    ax = plotd_new.plot.bar(x = "Dataset", rot=0, figsize=(20, 15), fontsize=18, color=['#FEC606', '#D33257', '#1DABB8', '#2CC990', '#E22211', '#11156D'])
    ax.legend(fontsize=16)
    ax.set_xlabel('Dataset', fontsize = 24)
    ax.set_ylabel('Compression Ratio (in percentage)', fontsize = 24)
    fig = ax.get_figure()
    fig.savefig("./Results/" + "output" + str(i+1) + ".png")