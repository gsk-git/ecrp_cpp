class TreeNode:
    def __init__(self, value=0, left=None, right=None):
        self.value = value
        self.left = left
        self.right = right


branch5 = TreeNode(value=5)
branch4 = TreeNode(value=4, left=branch5)
branch3 = TreeNode(value=3, left=None, right=branch4)
branch2 = TreeNode(value=2 , left=branch3, right=None)
branch1 = TreeNode(value=1, left=None, right=branch2)

root = branch1

def inorder_traversal(root: TreeNode) -> list:
    result = []
    if root is None:
        return result
    
    def traverseTree(branch: TreeNode, recordList: list[int]):
        if branch is None:
            return
        
        traverseTree(branch.left, recordList)
        recordList.append(branch.value)
        traverseTree(branch.right, recordList)

    traverseTree(root, result)
    return result

print(inorder_traversal(root))