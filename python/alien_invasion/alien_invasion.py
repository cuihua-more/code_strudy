import sys
from time import sleep
import pygame
from settings import Settings
from ship import Ship
from bullet import Bullet

class AlienInvasion:
    """管理游戏资源和行为的类"""

    def __init__(self):
        """初始化并创建游戏资源"""
        pygame.init() # 初始化背景设置
        self.settings = Settings()

        """进入全屏模式"""
#        self.screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
#        self.settings.screen_width = self.screen.get_rect().width
#        self.settings.screen_height = self.screen.get_rect().height

        self.screen = pygame.display.set_mode((self.settings.screen_width, # 创建一个显示窗口，游戏的所有图形元素都在此绘制 1200 800指定窗口的宽和高像素
                                                self.settings.screen_height)) 
        pygame.display.set_caption("Alien Invision")
        self.ship = Ship(self)
        self.buttles = pygame.sprite.Group() # 创建储存子弹的编组

    def run_game(self):
        """开始游戏的主循环"""
        while True:
            # 监视键盘和鼠标的状态
            self._check_event()
            # 更新飞船的位置
            self.ship.update() 
            self._update_bullets()
             # 每次循环时都重新绘制屏幕
            self._update_screen()
           

        # 屏幕显示更新代码
    def _update_screen(self):
        """更新屏幕上的图案，并切换到新屏幕"""
        self.screen.fill(self.settings.bg_color) # 设置屏幕背景色
        self.ship.blitme() # 调用飞船绘制
        for bullet in self.buttles.sprites():
            bullet.draw_bullet() 
        # 让最近绘制的屏幕可见
        pygame.display.flip()

    def _check_event(self):
        """响应按键和鼠标事件"""
        for event in pygame.event.get(): # pygame.event.get()返回上一次被调用后发生事件的列表
            if event.type == pygame.QUIT: # 窗口退出事件
                sys.exit()
            elif event.type == pygame.KEYDOWN: # 案件按下事件
                self._check_keydown_event(event)
            elif event.type == pygame.KEYUP: # 松开按键
                self._check_keyup_event(event)
    
    def _check_keydown_event(self, event):
        """响应按下按键事件"""
        if event.key == pygame.K_RIGHT: # 按键时右方向键
            self.ship.moving_right = True
        elif event.key == pygame.K_LEFT:
            self.ship.moving_left = True
        elif event.key == pygame.K_q:
            sys.exit() 
        elif event.key == pygame.K_f:
            self._set_full_screen()
        elif event.key == pygame.K_SPACE:
            self._fire_bullet()

    def _check_keyup_event(self, event):
        """响应松下按键事件"""
        if event.key == pygame.K_RIGHT:
            self.ship.moving_right = False
        elif event.key == pygame.K_LEFT:
            self.ship.moving_left = False
        
    # 全屏游戏
    def _set_full_screen(self):
        self.screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
        self.settings.screen_width = self.screen.get_rect().width
        self.settings.screen_height = self.screen.get_rect().height

    def _fire_bullet(self):
        """创建一颗子弹，并将其加入编组bullets中"""
        if len(self.buttles) < self.settings.bullet_allowed:
            new_bullet = Bullet(self)
            self.buttles.add(new_bullet)
    
    def _update_bullets(self):
        """更新子弹的位置，并删除消失的子弹"""
        self.buttles.update()   # 对于编组，这会自动调用编组里每个成员的update

        #删除消失的子弹
        for bullet in self.buttles.copy():# 用copy是因为for循环时要保证所循环的长度不变
            if bullet.rect.bottom <= 0:
                self.buttles.remove(bullet)

if __name__ == "__main__":
    # 创建游戏实例 并运行游戏
    ai = AlienInvasion()
    ai.run_game()