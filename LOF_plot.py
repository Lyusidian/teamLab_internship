import csv
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def ScatterPlot2( x_1, y_1, x_2, y_2):
    plt.plot(x_1, y_1, '.', color='c', label='other')
    plt.plot(x_2, y_2, '.', color='m', label='top5')
    #plt.xlim( , )   #(xの最小値, xの最大値)
    #plt.ylim( , )
    plt.title("data3.csv:k=5")
    plt.xlabel("x")
    plt.ylabel("y")

    plt.legend(loc='upper left')
    plt.show()

if __name__ == '__main__':
    x_list = []
    y_list = []
    LOF_list = []
    list = []
    #OutlierDetection_LOF.cで生成されるcsvファイルを読み込む
    with open('data3_LOF.csv', 'r') as f:
        reader = reader = csv.reader(f)
        header = next(reader)   #ヘッダを飛ばす

        for row in reader:
            list.append(row)
    list.sort(key=lambda x:x[2])
    x_1 = []
    y_1 = []
    x_2 = []    #上位5
    y_2 = []    #上位5
    for i in range(195):
        x_1.append(float(list[i][0]))
        y_1.append(float(list[i][1]))
    for i in range(5):
        x_2.append(float(list[195+i][0]))
        y_2.append(float(list[195+i][1]))
    ScatterPlot2(x_1, y_1, x_2, y_2)
