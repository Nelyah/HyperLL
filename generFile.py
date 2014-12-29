import random

# Generates a file with random numbers

f = open("card.txt", "w")

for i in range(0,1000000):
    a = random.randint(1,15)
    f.write(str(a)+'\n')


