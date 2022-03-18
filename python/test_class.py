import unittest


class AnonymousSurvery:
    """收集匿名调查问卷的答案"""

    def __init__(self, question):
        """存储一个问题，并为存储答案做准备"""
        self.question = question
        self.responses = []

    def show_question(self):
        """显示调查问卷"""
        print(self.question)
    
    def store_response(self, new_response):
        """存储调查问卷答案"""
        self.responses.append(new_response)
    
    def show_results(self):
        """显示收集到的所有答案"""
        print("Survey results:")
        for response in self.responses:
            print(f"- {response}")

#question = "What language did you first learn to speak?"
#my_survey = AnonymousSurvery(question)
#
## 显示问题并存储答案
#my_survey.show_question()
#print(f"Enter 'q' at any time to quit.\n")
#while True:
#    response = input("Language:")
#    if response == "q":
#        break
#    else :
#        my_survey.store_response(response)
#
## 显示调查的结果
#print("\n Thank you to everyone who participated in the survey!")
#my_survey.show_results()

class TestAnonymousSurvey(unittest.TestCase):
    """针对AnonymousSurvey类的测试"""
    def test_store_single_response(self): # 测试一个类的方法
        """测试单个答案会被妥善的储存。"""
        question = "Waht language did you first learn to speak?"
        my_survey = AnonymousSurvery(question) # 定义一个类
        my_survey.store_response("Englsih") # 调用类方法添加一个值
        self.assertIn("Englsih", my_survey.responses) # 断言这个值是否已经添加进去

    def test_stroe_three_response(self):
        """测试储存三个答案"""
        question = "Waht language did you first learn to speak?"
        my_survey = AnonymousSurvery(question)
        responses = ["English", "Spanish", "Chinese"]
        for response in responses:
            my_survey.store_response(response)
        
        for response in responses:
            self.assertIn(response, my_survey.responses)

class TestAnonymousSurvey1(unittest.TestCase):

    def setUp(self):
        """
        创建一个调查对象和一组答案，供使用的测试方法使用。
        """
        question = "What language did you first learn to speak?"
        self.my_survey = AnonymousSurvery(question) # setUp()方法定义的属性可供整个类使用
        self.responses = ["Englsih", "Sapnish", "Mandarin"]
    
    def test_store_single_response(self):
        self.my_survey.store_response(self.responses[0]) # 使用setUp方法中定义的类属性
        self.assertIn(self.responses[0], self.my_survey.responses)
    
    def test_store_three_response(self):
        for response in self.responses:
            self.my_survey.store_response(response)
        for response in self.responses:
            self.assertIn(response, self.my_survey.responses)

if __name__ == "__main__":
    unittest.main()