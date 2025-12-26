braces = "[]"

def promise_list(braces):
    promised = []
    matchBook = {"{":"}","[":"]","(":")"}
    for char in braces:
        if char == "{" or char == "[" or char == "(":
            promised.append(char)
        else:
            if promised:
                lastPromise = promised.pop()
                if char == matchBook[lastPromise]:
                    pass
                else:
                    return False
            else:
                return False
    if promised:
        return False
    else:
        return True

print(promise_list(braces))
