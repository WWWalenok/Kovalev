# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

import math

nu = 38E9
c = 5.7E9
k = 360E6
a = 1
b = 10

r = a

pq1 = k * r*r / (b*b) - c * k * r * r / ((2 * nu + c) * a * a) - 4 * nu * k * math.log(r / a) / (2 * nu + c) - k - c * k / (2 * nu + c)

r = b

pq2 = k * r*r / (b*b) - c * k * r * r / ((2 * nu + c) * a * a) - 4 * nu * k * math.log(r / a) / (2 * nu + c) - k - c * k / (2 * nu + c)

print(pq1)
print(pq2)

p = 0.5E9
q = 2.0E9

r1 = a
r2 = b

def Err(rs) :
    return ((-p + k * (rs * rs / (b * b) - 1)) - (-q + c * k / (2 * nu + c) * (rs * rs / (a * a) - 1) + 4 * k * nu / (2 * nu + c) * math.log(rs / a)))**2

for i in range(0, 200):

    err1 = Err(r1 + (r2 - r1) * 0.25)
    err2 = Err(r1 + (r2 - r1) * 0.75)
    print(str(i) + " : " + str(r1) + " " + str(r2) + " : " + str(err1) + " " + str(err2))
    if err1 < err2 :
        r2 = (r1 + r2) * 0.5
    else :
        r1 = (r1 + r2) * 0.5



    if(err1 == 0 and err2 == 0):
        break

print((r1 + r2) * 0.5)