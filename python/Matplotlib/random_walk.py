from random import choice

class RandomWalk:
    """一个生成随机漫步数据的类"""

    def __init__(self, num_points=5000):
        """初始化随机漫步的属性"""
        self.num_points = num_points

        # 所有随机漫步都始于（0,0）
        self.x_values = [0]
        self.y_values = [0]

    def file_walk(self):
        """计算随机漫步包含的所有点"""

        # 直到满足num_points
        while len(self.x_values) < self.num_points:

            # 决定前进方向以及沿着个方向前进的距离
            x_dicection = choice([1, -1]) # choice在给定的列表中随机返回一项

            x_distance = choice([0, 1, 2, 3, 4])
            x_step = x_dicection * x_distance

            y_dicection = choice([1, -1])
            y_distance = choice([0, 1, 2, 3, 4])
            y_step = y_dicection * y_distance

            # 拒绝原地踏步
            if x_step == 0 and y_step == 0:
                continue

            # 计算下一个点的x值和y值
            x = self.x_values[-1] + x_step
            y = self.y_values[-1] + y_step

            self.x_values.append(x)
            self.y_values.append(y)