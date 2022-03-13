from re import fullmatch
import re


def greet_func(username):
    """定义一个函数。"""
    print(f"Hello {username.title()}")

greet_func("job")

def describe_pet(pet_name, animal_type = "dog") : # 函数定义时，形参可以给默认值
    print(f"I have a {animal_type}")
    print(f"My {animal_type}'s name is {pet_name.title()}")

describe_pet("peter")
describe_pet("tom", "cat")

def get_formatted_name(first_name, last_name) :
    """返回整洁的姓名"""
    full_name = f"{first_name} {last_name}"
    return full_name.title()

print(get_formatted_name("jimi", "hendrix"))

def get_formatted_name(first_name, last_name, middle_name="") : # middle_name这样写表示这个参数时可选的
    if middle_name :
        full_name = f"{first_name} {middle_name} {last_name}"
    else :
        full_name = f"{first_name} {last_name}"
    return full_name.title()

print(get_formatted_name("himi", "hendix"))
print(get_formatted_name("john", "hooker", "lee"))

def greet_users(names) : # 函数参数可以接受任何类型的参数，包括列表、字典
    if names :
        for name in names:
            print(f"Hello {name.title()}")
    else :
        print(f"names is NULL")

usernames = ["hannah", "ty", "margot"]
greet_users(usernames)

def print_models(unprint_designs, complete_models) : # 此函数可以看出，函数参数，既可以输出，又可以输出，相当于C++的引用一样，默认就是引用，如果不想引用的话，可以使用赋值的方法传递副本，即后面加[:]
    """
    模拟打印每一个设计，直到没有未打印的设计为止
    """
    while unprint_designs:
        current_design = unprint_designs.pop()
        print(f"Printing model: {current_design}")
        complete_models.append(current_design)

def show_completed_models(completed_models):
    """显示打印好的所有模型"""
    print("\nThe following models have been printed")
    for completed_model in completed_models:
        print(completed_model)

unprinted_designs = ["phone case", "rebot pendant", "dodecachedron"]
completed_models = []
print_models(unprinted_designs, completed_models)
show_completed_models(completed_models)

def make_pizza(*toppings): # *表示Python创建一个空元组，接受到的所有参数都存放在这个toppings中，不是C语言的指针
    print(toppings)
make_pizza("pepperoni")
make_pizza("mushrooms", "green peppers", "extra cheese")

def make_pizza(size, *toppings): # 如果定义空元组，必须将其放在最后一个位置
    print(f"\n Making a {size}-inch pizza wich the following toppings")
    for topping in toppings:
        print(f"- {topping}")

make_pizza(16, "mushrooms", "green peppers", "extra cheese")

def build_profile(first, last, **user_info):    # **表示接受任意数量长度的键值对
    user_info["first"] = first
    user_info["last"] = last
    return user_info

print(build_profile("albert", "einstein",
                    location = "princetion",
                    filed = "physics",
                    firsta = "A",
                    lasta = "A"))