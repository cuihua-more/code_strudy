
filename = "programing.txt"

with open(filename, 'w') as file_objet: # 以写的方式创建一个文件，filename为文件名 file_object表示这个文件描述符 必须有with
    file_objet.write("I love programing!") # file_object有效范围在with中


try: # 可能会出现异常的代码
    print(5 / 0)
except ZeroDivisionError: # 出现异常后捕获 ZeroDivisionError是可能出现的对象
    print(f"You Can't devide by zero!")

print(f"Give me two numbers, and I'll divide them.")
print(f"Enter 'q' to quit.")

while True:
    first_number = input("\nFisrt number:")
    if first_number == 'q':
        break

    second_number = input("\nSecond number:")
    if second_number == "q":
        break

    try:
        answer = int(first_number) / int(second_number)
    except ZeroDivisionError:
        print(f"You can't divide 0!")
    else :
        print(answer)
