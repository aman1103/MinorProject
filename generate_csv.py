import os
import csv
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

folderpath = './Datasets/'

execs = ["huffman", "adaptive-huffman", "rle", "efficientrle", "lz77", "lz78"]

for ex in execs:
    with open("./csvs/" + ex + ".csv", 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["Dataset", "Dataset_Path", "Original_Size", "Compressed_Size", "Compression_Ratio"])

        for dirname, subdirs, files in os.walk(folderpath):
            for fname in files:
                full_path = os.path.join(dirname, fname)

                if ex == "lz78":
                    os.system("./Execs/" + ex + " -n " + full_path + " ./Outputs/" + ex + "/" + "outfile-" + fname + ".txt")
                else:
                    os.system("./Execs/" + ex + " " + full_path + " ./Outputs/" + ex + "/" + "outfile-" + fname + ".txt")
                
                original_size = os.path.getsize(str(full_path))
                compressed_size = os.path.getsize("./Outputs/" + ex + "/" + "outfile-" + fname + ".txt")
                
                print(f"Writing {fname} stats to CSV:")
                writer.writerow([fname, full_path, original_size, compressed_size, (1 - compressed_size/original_size)])