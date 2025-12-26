def climbingStairs (a: int) -> int:
    mem = {}
    def climb(b: int):
        if b in mem:
            return mem[b]
        
        if b == 1:
            return 1
        if b == 2:
            return 2
        
        mem[b] = climb(b-1)+climb(b-2)
        return mem[b]
    
    return climb(a)

print(climbingStairs(4))
print(climbingStairs(5))
print(climbingStairs(6))
print(climbingStairs(7))
print(climbingStairs(8))
print(climbingStairs(9))
print(climbingStairs(10))
print(climbingStairs(50))
print(climbingStairs(100))
print(climbingStairs(999))