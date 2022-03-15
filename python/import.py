import imp
import module1    # 使用import导入外部定义的函数

module1.make_pizze(16, "pepperoni") # 外部导入的函数调用 必须以导入的模块开头
module1.make_pizze(12, "mushrooms", "green penper", "extra cheese")

from module1 import make_pizze as pizz # 使用as为make_pizza函数定义一个别名
pizz(12, "mushrooms", "green penper", "extra cheese")

import module1 as p # 也可以为模块作别名
p.make_pizze(12, "mushrooms", "green penper", "extra cheese")
