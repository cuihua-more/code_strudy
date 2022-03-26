from shutil import which
import matplotlib.pyplot as plt

x_values = [1, 2, 3, 4, 5]
y_values = [1, 4, 9, 16, 25]

plt.style.use("seaborn")
fig, ax = plt.subplots()
ax.scatter(2, 6, s=200) # 在(2, 4)位置绘制一个点 s是点的尺寸
ax.scatter(x_values, y_values, s=200) # 绘制一系列点

x_nums = range(1, 1001)
y_nums = [x**2 for x in x_nums]
#ax.scatter(x_nums, y_nums, s=10, c="red") # c是点的颜色
ax.scatter(x_nums, y_nums, s=10, c=y_nums, cmap=plt.cm.Blues) # cmap表示用哪个颜色映射

#设置图标的标题等
ax.set_title("Square", fontsize=24)
ax.set_xlabel("Value", fontsize=14)
ax.set_ylabel("Value's Square", fontsize=14)

# 设置刻度标记的大小
ax.tick_params(axis="both", which="major", labelsize = 14)

#plt.savefig("squares.png", bbox_inches="tight") # 自动保存图标， 第一个参数表示保存的文件名字，第二个表示自动将多余的空白区域裁掉
plt.show()