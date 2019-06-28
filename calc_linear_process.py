'''
calc_linear.pyにより求めたA,bを用いて
指定の画像に色補正処理を行う。
'''
from PIL import Image

#y=Ax+bを計算する
def calc_linear_process(rgb_im1, A, b, size):
    im2 = Image.new('RGB',size)
    for x in range(size[0]):
        for y in range(size[1]):
            r1, g1, b1 = rgb_im1.getpixel((x, y))
            r2 = int(A[0][0]*r1 + A[0][1]*g1 + A[0][2]*b1 + b[0])
            g2 = int(A[1][0]*r1 + A[1][1]*g1 + A[1][2]*b1 + b[1])
            b2 = int(A[2][0]*r1 + A[2][1]*g1 + A[2][2]*b1 + b[2])

            im2.putpixel((x,y), (r2, g2, b2, 0))
    return im2


if __name__ == '__main__':
    #任意の画像ファイル名を入力
    im1 = Image.open("data1.jpg")
    rgb_im1 = im1.convert('RGB')
    size1 = rgb_im1.size

    #im1についてy=Ax+bによる変換を行う
    A = [[1.4046, -0.0053, 0.0080], [0.0229, 0.9712, -0.0154], [0.0306, 0.0368, 0.8381]]
    b = [0.1533, -0.0222, -0.1289]
    im2 = calc_linear_process(rgb_im1, A, b, size1)
    im2.show()
    im2.save('data1_CCP.jpg', quality=95)
