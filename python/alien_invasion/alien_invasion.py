import imp
import sys
import pygame
from settings import Settings

class AlienInvasion:
    """管理游戏资源和行为的类"""

    def __init__(self):
        """初始化并创建游戏资源"""
        pygame.init() # 初始化背景设置
        self.settings = Settings()

        self.screen = pygame.display.set_mode((self.settings.screen_width, # 创建一个显示窗口，游戏的所有图形元素都在此绘制 1200 800指定窗口的宽和高像素
                                                self.settings.screen_height)) 
        pygame.display.set_caption("Alien Invision")

    def run_game(self):
        """开始游戏的主循环"""
        while True:
            # 监视键盘和鼠标的状态
            for event in pygame.event.get(): # pygame.event.get()返回上一次被调用后发生事件的列表
                if event.type == pygame.QUIT:
                    sys.exit()

            # 每次循环时都重新绘制屏幕
            self.screen.fill(self.settings.bg_color)
            
            # 让最近绘制的屏幕可见
            pygame.display.flip()

if __name__ == "__main__":
    # 创建游戏实例 并运行游戏
    ai = AlienInvasion()
    ai.run_game()