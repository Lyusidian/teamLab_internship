from PIL import Image
import numpy as np
import pandas as pd
import csv

#任意の画像ファイル名を入力
im1 = Image.open("data1.jpg")
im2 = Image.open("data2.jpg")

rgb_im1 = im1.convert('RGB')
rgb_im2 = im2.convert('RGB')

size = rgb_im1.size #(1600, 1066)
#size_im2 = rgb_im2.size
print(size)

#im1,im2それぞれのpixelデータをcsvファイルに出力
np1 = np.zeros((size[0],size[1]*3))
np2 = np.zeros((size[0],size[1]*3))

for x in range(size[0]):
    for y in range(size[1]):
        r1,g1,b1 = rgb_im1.getpixel((x,y))
        r2,g2,b2 = rgb_im2.getpixel((x,y))
        np1[x][3*y] = r1
        np1[x][3*y+1] = g1
        np1[x][3*y+2] = b1
        np2[x][3*y] = r2
        np2[x][3*y+1] = g2
        np2[x][3*y+2] = b2

df1 = pd.DataFrame(np1)
df2 = pd.DataFrame(np2)

df1.to_csv('data1.csv')
df2.to_csv('data2.csv')
