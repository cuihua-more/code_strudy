import json

numbers = [2, 3, 5, 7, 11, 13, 15]

filenames = "number.json"
with open(filenames, "w") as file_obj:
    json.dump(numbers, file_obj)    # 将数组数据写道文件中

with open(filenames) as f:
    number = json.load(f)   # 从文件总读取文件
print(number)