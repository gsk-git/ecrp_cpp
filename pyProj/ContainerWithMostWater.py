import random

def generateFences () -> list[int]:
    return random.sample(range(1,10), k=random.randrange(5,10))

def highestPossibleWaterAmount ():
    fence = generateFences()
    leftPointer = 0
    rightPointer = len(fence)-1
    maxWater = 0
    currentWater = 0
    distance = 0
    bestWaterSet ={}

    while rightPointer > leftPointer:
        
        #Loops to identify 
        if fence[rightPointer]>fence[leftPointer]:
            #leftPoiner is the least point
            distance = rightPointer - leftPointer
            currentWater = (fence[leftPointer] * distance)
            if maxWater < currentWater:
                maxWater = currentWater 
                bestWaterSet.clear()
                bestWaterSet[maxWater] = [fence[rightPointer], fence[leftPointer]]
            leftPointer+=1
        elif fence[rightPointer]<=fence[leftPointer]:
            #rightPoiner is the least point
            distance = rightPointer - leftPointer
            currentWater = (fence[rightPointer] * distance)
            if maxWater < currentWater:
                maxWater = currentWater 
                bestWaterSet.clear()
                bestWaterSet[maxWater] = [fence[rightPointer], fence[leftPointer]]
            rightPointer-=1

    result = "For the Fence Set : "+str(fence)+" \nFence combination of :"+ str(bestWaterSet[maxWater])+" have the highest capacity of "+str(maxWater)
    print(result)
    return

highestPossibleWaterAmount()