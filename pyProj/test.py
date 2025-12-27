class ListNode:
    def __init__(self, value=0, next=None):
        self.value = value
        self.next = next

valList = [10, 15, 20, 25, 20, 30, 35, 35, 35, 40, 20]
head = ListNode(valList[0])

def createLinkedList(valList):
    if not valList:
        return None    
    currentNode = head
    for val in valList[1:]:
        currentNode.next = ListNode(val)
        currentNode = currentNode.next
    return head

def printLinkedList(head):
    if not head:
        return   
    currentNode = head
    nodes_str = [] # List to store string representations of node values
    while currentNode: # Walk through the list
        nodes_str.append(str(currentNode.value)) # Add current node's value (as string)
        currentNode = currentNode.next
    print(" -> ".join(nodes_str) + " -> None")

def removeDuplicates(head):
    if not head:
        return None    

    currentNode = head
    seenValues = set()
    previousNode = None

    while currentNode is not None:
        if currentNode.value in seenValues:
            previousNode.next = currentNode.next
            currentNode = currentNode.next
        else:
            seenValues.add(currentNode.value)
            previousNode = currentNode
            currentNode = currentNode.next
    return head

            


createLinkedList(valList)
printLinkedList(head)
head = removeDuplicates(head)
printLinkedList(head)  # Print the linked list after removing duplicates  
