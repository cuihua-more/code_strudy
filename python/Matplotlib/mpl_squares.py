import matplotlib.pyplot as plt

squares = [1, 4, 9, 16, 25]
input_value = [1, 2, 3, 4, 5]

plt.style.use("seaborn") # 设置表格样式
fig, ax = plt.subplots() # 在一张图片中绘制一个或多个表；fig表示这张图，ax表示图标
ax.plot(input_value, squares, linewidth=3) # linewidth 表示绘制的线的宽度 

# 设置图标标题并给坐标轴加上标签
ax.set_title("squares", fontsize=24) # fontsize表示字的大小
ax.set_xlabel("value", fontsize=14)
ax.set_ylabel("value's squares", fontsize=14)

# 设置刻度标记的大小
ax.tick_params(axis="both", labelsize=14) # 设置刻度样式

plt.show()