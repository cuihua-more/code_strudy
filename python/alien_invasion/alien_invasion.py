import sys
from time import sleep
import pygame
from settings import Settings
from game_stats import GameStats
from ship import Ship
from bullet import Bullet
from alien import Alien

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
        # 黄建一个用于储存游戏统计信息的实例
        self.stats = GameStats(self)

        self.ship = Ship(self)
        self.buttles = pygame.sprite.Group() # 创建储存子弹的编组
        self.aliens = pygame.sprite.Group()

        self._create_fleet()

    def run_game(self):
        """开始游戏的主循环"""
        while True:
            # 监视键盘和鼠标的状态
            self._check_event()

            if self.stats.game_active:
                # 更新飞船的位置
                self.ship.update() 
                self._update_bullets()
                self._update_aliens()
            
             # 每次循环时都重新绘制屏幕
            self._update_screen()
           

        # 屏幕显示更新代码
    def _update_screen(self):
        """更新屏幕上的图案，并切换到新屏幕"""
        self.screen.fill(self.settings.bg_color) # 设置屏幕背景色
        self.ship.blitme() # 调用飞船绘制
        for bullet in self.buttles.sprites():
            bullet.draw_bullet() 
        self.aliens.draw(self.screen) # 在self.screen中画外星人
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

        self._check_bullet_alien_collisions()

    def _check_bullet_alien_collisions(self):
        """响应子弹和外星人碰撞。"""
        # 检擦是否有子弹击中了外星人
        #   如果是，就删除向右的子弹和外星人
        collisions = pygame.sprite.groupcollide(self.buttles, # 比较两个rect是否重叠，如果重叠的话，两个True表示两个都删掉
                                                self.aliens, True, True) 

        if not self.aliens:
            # 删除现有的子弹并新建一群外星人。
            self.buttles.empty() # 删除子弹
            self._create_fleet()

    def _create_fleet(self):
        """创建外星人群"""
        # 创建一个外星人并计算一行能容纳多少个外星人
        # 外星人的间距为外星人的宽度
        alien = Alien(self)
        alien_width, alien_heigh = alien.rect.size # 注意
        available_space_x = self.settings.screen_width - (2 * alien_width)
        number_aliens_x = available_space_x // (2 * alien_width) # //表示整除，不要小数

        #计算屏幕可容纳多少行外星人
        ship_heigh = self.ship.rect.height
        avaliable_space_y = (self.settings.screen_height - 
                                (3 * alien_heigh) - ship_heigh)
        number_rows = avaliable_space_y // (2 * alien_heigh)

        # 创建外星人群
        for row_numbers in range(number_rows):
            for alien_number in range(number_aliens_x): 
                self._create_alien(alien_number, row_numbers)
            
    
    def _create_alien(self, alien_number, row_number):
        """创建一个外星人并将其放在当前行"""
        alien = Alien(self)
        alien_width, alien_heigh = alien.rect.size
        alien.x = alien_width + 2 * alien_width * alien_number
        alien.rect.x = alien.x
        alien.rect.y = alien.rect.height + 2 * alien_heigh * row_number
        self.aliens.add(alien)

    def _update_aliens(self):
        """
        检测是否有外星人位于屏幕的边缘，
        并更新整群外星人的位置。
        """
        self._check_fleet_deges()
        self.aliens.update()

        # 检测外星人和飞船的碰撞
        if pygame.sprite.spritecollideany(self.ship, self.aliens): # 检测一个矩形是否和一个矩形组产生碰撞，如果有的话返回矩形组中的这个，没有返回空
            self._ship_hit()
        
        #检查是否有外星人到达了屏幕底端
        self._check_aliens_bottom()
    
    def _check_fleet_deges(self):
        """有外星人到达边缘时应采取的相应措施"""
        for alien in self.aliens.sprites():
            if alien.check_edges():
                self._change_fleet_direction()
                break
    
    def _change_fleet_direction(self):
        """将整群外星人下移，并改变他们的方向"""
        for alien in self.aliens.sprites():
            alien.rect.y += self.settings.alien_drop_speed
        self.settings.fleet_direction *= -1

    def _ship_hit(self):
        """响应飞船被外星人撞到。"""

        if self.stats.ships_left > 0:
            # 将ship_left减1
            self.stats.ships_left -= 1

            # 清空剩下的子弹和外星人
            self.aliens.empty()
            self.buttles.empty()

            #创建一群新的外星人，并将飞船放在屏幕底部的中央
            self._create_fleet()
            self.ship.center_ship()

            #暂停 0.5S
            sleep(0.5)
        else:
            self.stats.game_active = False

    def _check_aliens_bottom(self):
        """检查是否有外星人到达了屏幕的底端。"""
        screen_rect = self.screen.get_rect()
        for alien in self.aliens.sprites():
            if alien.rect.bottom >= screen_rect.bottom:
                # 像飞船被撞了一下处理：
                self._ship_hit()
                break


if __name__ == "__main__":
    # 创建游戏实例 并运行游戏
    ai = AlienInvasion()
    ai.run_game()