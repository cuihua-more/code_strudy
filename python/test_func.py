import unittest

def get_formatted_name(first, last, middle = ""):
    """生成整洁的姓名"""
    if middle:
        full_name = f"{first} {middle} {last}"
    else:
        full_name = f"{first} {last}"

    return full_name.title()

class NamesTestCase(unittest.TestCase): #创建一个测试类，继承于unittest.TestCase 这样才能Python自动测试
    """测试get_formatted_name函数"""

    def test_first_last_name(self): # 具体的测试方法 运行这个测试案例时，所有以test开头的方法都会被自动执行
        """能够正确的处理像Jains Jpolin这样的姓名吗"""
        formatted_name = get_formatted_name("jains", "jpolin") # 测试方法的具体实现
        self.assertEqual(formatted_name, "Jains Jpolin") # 断言，执行结果是否个期望的结果一致
    
    def test_first_last_middle_name(self): # test开头
        """能够正确的处理像Wolfgang Amadeus Mozart这样的姓名吗"""
        formatted_name = get_formatted_name("wolfgang", "mozart", "amadeus")
        self.assertEqual(formatted_name, "Wolfgang Amadeus Mozart")

if __name__ == "__main__": # __name__是一个程序执行时的特殊变量，如果作为主程序执行时，这个值就是__main__
    unittest.main() # 运行测试案例