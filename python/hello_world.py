from http.client import CannotSendRequest
from math import factorial
from re import M


message = "Hello Python Message!"
print(message)

message = "Hello Python Crash Course world!"
print(message)

name = "ada lovalace"
print(name.title()) #首字母大写
print(name.upper()) #全部大写
print(name.lower()) #全部小写

first_name = "ada"
last_name = "lovalace"
full_name = f"{first_name} {last_name}" # f 表示创建并返回字符串
print(f"Hello, {full_name.title()}!")

print(f"Language:\n\tPython\n\tC\n\tCPP\n\tjavaScript") # /n /t等和C语言一样

favorite_labguage = " Python "
print(favorite_labguage)
print(favorite_labguage.rstrip()) # 返回除最后空格的字符串
print(favorite_labguage)
favorite_labguage = favorite_labguage.rstrip() #要保存除最后空格的字符串的话需要重新赋值
print(favorite_labguage)
favorite_labguage = favorite_labguage.lstrip() # 返回除开头空格的字符串
print(favorite_labguage)

num = 1_000_000
print(num)

motorcycles = ["honda", "ymaha", "suzuki"]
print(motorcycles)
motorcycles.append("add") # 列表中末尾增加一项
print(motorcycles)
motorcycles.insert(1, "insert") # 在指定位置后添加一项
print(motorcycles)
del motorcycles[1] #删除列表中指定位置项
print(motorcycles)
print(f"Remove Last item: {motorcycles.pop()}") # 删除最后一项，并返回这一项内容，如果有参数，表示删除的某一项
print(motorcycles)
print(f"Remove the sec item: {motorcycles.pop(2)}")
print(motorcycles)
motorcycles.remove("honda") # 删除指定内容
print(motorcycles)

cars = ["bmw", "audi", "toyoda", "subaru"]
print(f"cses length: {len(cars)}")
print(cars)
cars.reverse() # 位置反转
print(cars)
print(f"cars items sorted: {sorted(cars)}") # 临时排序
cars.sort() # 永久排序
print(f"cars items forever sort: {cars}") 

magicians = ["alice", "david", "carolina"]
for magician in magicians:  # for循环
    print(magician)
    print(f"still is {magician.title()}")
