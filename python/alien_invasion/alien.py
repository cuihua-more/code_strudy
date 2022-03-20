import pygame
from pygame.sprite import Sprite

class Alien(Sprite):
    """表示单个外星人的类"""

    def __init__(self, ai_game):
        """初始化外星人，并设置起始位置"""
        super().__init__()
        self.screen = ai_game.screen

        # 加载外星人图像并设置起始位置
        self.image = pygame.image.load("images/alien.bmp")
        self.rect = self.image.get_rect()

        # 储存外星人精确水平位置
        self.x = float(self.rect.x)