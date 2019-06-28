import csv
import matplotlib.pyplot as plt

if __name__ == '__main__':
    list = []
    #クラスターの数
    k = 3
    #Clustering_Kmeans.cで生成されるcsvファイルを読み込む
    with open('data3_Kmeans.csv', 'r') as f:
        reader = reader = csv.reader(f)
        header = next(reader)   #ヘッダを飛ばす

        for row in reader:
            list.append(row)
    list.sort(key=lambda x:x[2])

    #クラスターの数は６個まで対応可能
    color = ['c','m','y','r','g','b']
    for i in range(k):
        x_1 = []
        y_1 = []
        for j in range(200):
            if int(list[j][2]) == i:
                x_1.append(float(list[j][0]))
                y_1.append(float(list[j][1]))
        plt.plot(x_1, y_1, '.', color=color[i])

    plt.title("data3_Kmeans:k=3")
    plt.show()
