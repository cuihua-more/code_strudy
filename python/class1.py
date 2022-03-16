from operator import mod
from statistics import mode


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

class Car:
    """汽车父类"""

    def __init__(self, make, model, year):
        self.make = make
        self.model = model
        self.year = year
        self.odometer_reading = 0   # 可以给类的属性定义默认值

    def get_descriptive_name(self):
        long_name = f"{self.year} {self.make} {self.model}"
        return long_name

    def read_odometer(self):
        print(f"This car has {self.odometer_reading} miles on it.")

    def update_odometer(self, mileage):
        if mileage >= self.odometer_reading:
            self.odometer_reading = mileage
        else:
            print(f"You can't toll back an odometer!")
    
    def increment_odometer(self, miles):
        self.odometer_reading += miles

    def fill_gas_tank(self):
        print(f"Every car has a tank.")

class Battery:
    """一个汽车电瓶类"""

    def __init__(self, battery_size = 75):
        """初始化电瓶值"""
        self.battery_size = battery_size

    def describe_battery(self):
        print(f"This car has a {self.battery_size}-KWh battery.")

class ElectricCar(Car): # 定义ElectricCar类，继承于Car类
    """定义一个电动车子类"""
    def __init__(self, make, model, year):
        super().__init__(make, model, year) # super().__init__表示调用父类构造函数父类
        self.battery_size = 75  # 给子类定义属性
        self.battery = Battery(65) # 将别的类定义为属性

    def describe_bettary(self): #子类定义方法
        print(f"This car has a {self.battery_size}-KWh bettary.")
    
    def fill_gas_tank(self): # 父类也有同名函数，但是在子类中调用，就会执行子类的函数，这叫做重写父类方法
        """电动车没有油箱"""
        print(f"This car doesn't need a gas tank.")

my_tesla = ElectricCar("tesla", "model S", "2019")
print(my_tesla.get_descriptive_name())
my_tesla.describe_bettary()
my_tesla.fill_gas_tank()
my_tesla.battery.describe_battery() #调用定义的属性类的方法
