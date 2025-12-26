number = [3,3]
target = 6


def two_sum(number: list[int], target: int)-> list[int]:
    record = {}
    complement = 0
    resultSet  = []
    for num in range(len(number)): # [0,1,2,3]
        complement = target - number[num] # 7 = 9 - 2
        if complement in record:# No
            resultSet.append(record[complement])
            resultSet.append(num)
            return(resultSet)
        else: # Added to records
            record[number[num]] = num

print(two_sum(number,target))


