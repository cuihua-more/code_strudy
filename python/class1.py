class Dog : # 定义一个类
    """一个模拟小狗的类"""

    def __init__(self, name, age) : # __init__表示类的构造函数， self类似cpp的this指针 name age是类初始化的参数
        """初始化类属性name age"""
        self.name = name # 为类属性赋值
        self.age = age
    
    def sit(self) : 
        """模拟小狗蹲下"""
        print(f"{self.name} is now setting.")
    
    def roll_over(self):
        """模拟小狗翻滚"""
        print(f"{self.name} rolled over.")

my_dog = Dog("Willie", 6)
print(f"My dog's name is {my_dog.name}") # 访问类属性
print(f"My dog's age is {my_dog.age}")

my_dog.sit() # 调用类方法
my_dog.roll_over()