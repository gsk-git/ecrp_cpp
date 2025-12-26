import random

def generateFences () -> list[int]:
    """
    This function generates random fence list[int].
    Geerated list will be always within 5 - 10 fence count and will never contain 0
    """
    return random.sample(range(1,10), k=random.randrange(5,10))

def highestPossibleWaterAmount () -> dict:
    """
    Calculates the maximum water that can be contained between two fences
    in a given set, and returns the details.

    Uses a two-pointer approach for optimal O(N) time and O(1) space complexity.

    Returns:
        dict: A dictionary containing:
            - 'fence' (list[int]): The generated fence heights.
            - 'bestWaterSet' (list[int]): The heights of the two fences that
                                          form the container with the maximum capacity.
            - 'maxWater' (int): The calculated maximum water capacity.
    """
    fence = generateFences()
    leftPointer = 0
    rightPointer = len(fence)-1
    maxWater = 0
    currentWater = 0
    distance = 0
    bestWaterSet: list[int] = []

    if not fence or len(fence) < 2:
        return {
            "fence": fence,
            "bestWaterSet": bestWaterSet,
            "maxWater": maxWater
        }

    while rightPointer > leftPointer:
        distance = rightPointer - leftPointer
        currentWater = (min(fence[rightPointer], fence[leftPointer]) * distance)
        
        if maxWater < currentWater:
            maxWater = currentWater
            bestWaterSet.clear()
            bestWaterSet.append(fence[rightPointer]) 
            bestWaterSet.append(fence[leftPointer])
         
        if fence[rightPointer]>fence[leftPointer]:
            leftPointer+=1
        else:
            rightPointer-=1
            
    return {
        "fence": fence,
        "bestWaterSet": bestWaterSet,
        "maxWater": maxWater
    }

resultData = highestPossibleWaterAmount()
print(f"For the Fence Set : "+str(resultData['fence'])+" \nFence combination of :"+ str(resultData['bestWaterSet'])+" have the highest capacity of "+str(resultData['maxWater']))