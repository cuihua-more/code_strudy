import pygame

class Ship:
    """管理飞船的类"""

    def __init__(self, ai_game): # ai_game是当前AlienInvasion类的引用
        """初始化飞船并设置其初始化的位置"""
        self.srceen = ai_game.screen # 获取游戏窗口
        self.settings = ai_game.settings # 获取设置
        self.screen_rect = ai_game.screen.get_rect() # 使用get_rect方法访问屏幕属性

        # 加载飞船图像并获取其外接矩形
        self.image = pygame.image.load("images/ship.bmp") # 加载图像 返回一个surface类
        self.rect = self.image.get_rect()

        # 对于每艘新飞船，都将其放在屏幕底部中央
        self.rect.midbottom = self.screen_rect.midbottom # 类rect的属性midbottom是设置rect的显示中下部的位置

        #连续移动标致
        self.moving_right = False # 向右
        self.moving_left = False # 向左

        #在飞船的属性X中存储小数值
        self.x = float(self.rect.x)


    def blitme(self):
        """在指定位置绘制飞船"""
        self.srceen.blit(self.image, self.rect) # 将图像显示在屏幕的指定位置

    def update(self):
        """根据移动的标志位移动飞船"""
        # 限制向右移动的最大值
        if self.moving_right and self.rect.right < self.screen_rect.right:
            self.x += self.settings.ship_speed # ship类的坐标向右加设置中的速度
            
        # 限制向左移动的最小值
        if self.moving_left and self.rect.left > 0:
            self.x -= self.settings.ship_speed # ship类的坐标向右减设置中的速度
        
        # 根据self.x更新rect对象
        self.rect.x = self.x

    def center_ship(self):
        """让飞船在屏幕底部居中"""
        self.rect.midbottom = self.screen_rect.midbottom
        self.x = float(self.rect.x)