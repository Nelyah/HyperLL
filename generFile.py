import random

# Generates a file with random numbers

f = open("card2.txt", "w")

for i in range(0,10000000):
    a = random.randint(100,1000000)
    f.write(str(a)+'\n')


