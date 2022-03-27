import matplotlib.pyplot as plt
from random_walk import RandomWalk

while True:
    # 创建一个RandomWalk实例
    rw = RandomWalk(50_000)
    rw.file_walk()

    # 绘制所有的点
    plt.style.use("classic")
    fig, ax = plt.subplots(figsize=(10, 6), dpi=128) # figsize设置显示的尺寸
    point_numbers = range(rw.num_points)
    ax.scatter(rw.x_values, rw.y_values,  c=point_numbers, cmap=plt.cm.Blues,
                edgecolors="none", s=1) # edgecolors="none" 表示删除每一个点的周围轮廓
    ax.scatter(0, 0, c="green", edgecolors="none", s=100)
    ax.scatter(rw.x_values[-1], rw.y_values[-1], c="red", edgecolors="none", s=100)

    # 隐藏坐标轴
    ax.get_xaxis().set_visible(False)
    ax.get_yaxis().set_visible(False)

    plt.show()

    keep_running = input("Make another walk? (y/n):")
    if keep_running == "n":
        break