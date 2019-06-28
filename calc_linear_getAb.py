'''
入力ベクトルをx、出力ベクトルをyとする。
最小二乗法よりy=Ax+bを満たすA,bを求める。
'''
import numpy as np
import pandas as pd
import csv

def calc_substraction(x,a):
    return x-a

def calc_mean(data):
    s = sum(data)
    n = len(data)
    return s/n

def calc_linear_getAb(im1_list,im2_list):
    '''ベクトルx,yを作成'''
    #リストx,yを作成
    im1_r_list, im1_g_list, im1_b_list = [], [], []
    im2_r_list, im2_g_list, im2_b_list = [], [], []
    for x in range(len(im1_list)):      #len(list)=2320
        #進み具合を出力
        if(x<4 or (len(im1_list)-4)<x):
            print("x: {}".format(x))
        for y in range(int((len(im1_list[0])-1)/3)):     #len(list[0])=2320*3+1
            #if(y<3 or 2315<y):
            #    print("y: {}".format(y))
            im1_r_list.append(im1_list[x][1+3*y])
            im1_g_list.append(im1_list[x][1+3*y+1])
            im1_b_list.append(im1_list[x][1+3*y+2])
            im2_r_list.append(im2_list[x][1+3*y])
            im2_g_list.append(im2_list[x][1+3*y+1])
            im2_b_list.append(im2_list[x][1+3*y+2])
    #x,yの平均値を計算する
    im1_r_ave = calc_mean(im1_r_list)
    im1_g_ave = calc_mean(im1_g_list)
    im1_b_ave = calc_mean(im1_b_list)
    im2_r_ave = calc_mean(im2_r_list)
    im2_g_ave = calc_mean(im2_g_list)
    im2_b_ave = calc_mean(im2_b_list)

    x_ave_list = []
    x_ave_list.append(im1_r_ave)
    x_ave_list.append(im1_g_ave)
    x_ave_list.append(im1_b_ave)
    x_ave = np.array(x_ave_list)
    x_ave = x_ave.T
    y_ave_list = []
    y_ave_list.append(im2_r_ave)
    y_ave_list.append(im2_g_ave)
    y_ave_list.append(im2_b_ave)
    y_ave = np.array(y_ave_list)
    y_ave = y_ave.T

    #x_1y_1をリストから削除し取得する
    im1_r_1 = im1_r_list.pop(0)
    im1_g_1 = im1_g_list.pop(0)
    im1_b_1 = im1_b_list.pop(0)
    im2_r_1 = im2_r_list.pop(0)
    im2_g_1 = im2_g_list.pop(0)
    im2_b_1 = im2_b_list.pop(0)

    im1_r_1_list = []
    im1_g_1_list = []
    im1_b_1_list = []
    im2_r_1_list = []
    im2_g_1_list = []
    im2_b_1_list = []
    for i in range(len(im1_r_list)):
         im1_r_1_list.append(im1_r_1)
         im1_g_1_list.append(im1_g_1)
         im1_b_1_list.append(im1_b_1)
         im2_r_1_list.append(im2_r_1)
         im2_g_1_list.append(im2_g_1)
         im2_b_1_list.append(im2_b_1)

    #x_i-x_1,y_i-y_1を先にしておく
    im1_r_list = list(map(calc_substraction, im1_r_list, im1_r_1_list))
    im1_g_list = list(map(calc_substraction, im1_g_list, im1_g_1_list))
    im1_b_list = list(map(calc_substraction, im1_b_list, im1_b_1_list))
    im2_r_list = list(map(calc_substraction, im2_r_list, im2_r_1_list))
    im2_g_list = list(map(calc_substraction, im2_g_list, im2_g_1_list))
    im2_b_list = list(map(calc_substraction, im2_b_list, im2_b_1_list))
    '''行列X,Yを作成'''
    X_list = []
    X_list.append(im1_r_list)
    X_list.append(im1_g_list)
    X_list.append(im1_b_list)
    Y_list = []
    Y_list.append(im2_r_list)
    Y_list.append(im2_g_list)
    Y_list.append(im2_b_list)
    '''係数A,bを求める'''
    X = np.array(X_list)
    Y = np.array(Y_list)
    A = np.dot(np.dot(Y, X.T), np.linalg.inv(np.dot(X, X.T)))
    b = y_ave - np.dot(A, x_ave)

    return A,b

if __name__ == '__main__':
    #任意のcsvファイル名を入力
    csvfile1 = pd.read_csv("data1.csv")
    csvfile2 = pd.read_csv("data2.csv")

    im1_list = csvfile1.values.tolist()     #.values
    im2_list = csvfile2.values.tolist()

    #print(len(im1_list))    #1600
    #print(len(im1_list[0])) #3199 -> (3199-1)/3=1066

    A, b = calc_linear_getAb(im1_list, im2_list)

    print("A: {}".format(A))
    print("b: {}".format(b))
