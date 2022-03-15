def make_pizze(size, *toppings) :
    """概述要制作的披萨"""
    print(f"\nMaking a {size}-inch pizze with the following toppins:")
    for topping in toppings :
        print(f" - {topping}")

