class Settings:
    """储存游戏中的所有设置类"""
    def __init__(self):
        """初始化游戏的设置"""
        # 屏幕设置
        self.screen_width = 1200
        self.screen_height = 800
        self.bg_color = (230, 230, 230)

        # 飞船设置
        self.ship_speed = 1.5

        #子弹设置
        self.bullet_speed = 1.5
        self.bullet_width = 3000
        self.bullet_heigh = 15
        self.bullet_color = (60, 60, 60)
        self.bullet_allowed = 3

        #外星人设置
        self.alien_speed = 1.0
        self.alien_drop_speed = 10
        # fleet_direction为1表示右移 -1表示左移
        self.fleet_direction = 1
        