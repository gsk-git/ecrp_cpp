"""
Admiral, the latest satellite scans are in, Lieutenant Commander Eva Rostova announced, her voice crisp in the holographic briefing room 
aboard the UNS Odyssey. The massive starship hummed, a silent giant charting unknown space in the Kepler-186 system.

Report, Commander Admiral Thorne's voice, a low rumble, filled the room. His weathered face, illuminated by the glowing grid projection, was stern.
Sir, gridSector_Delta We've designated it 'The Archipelago of Whispers' due to anomalous energy readings. Initial long-range scans 
show multiple landmasses, but the interference is... significant. 
We can't get an accurate count from orbit. Eva gestured to the projected grid, a complex matrix of '1's and '0's, 
where '1's represented potential land and '0's the turbulent, methane-rich seas of the planet below.
Interference? Thorne raised an eyebrow.

Yes, Admiral. Localized atmospheric distortions, almost like... a deliberate cloaking. 
Our primary objective is to conduct a low-altitude survey and confirm the exact number of islands. 
The science team believes these landmasses could harbor unique geological formations, possibly even exotic life.
And the risks?

The seas are treacherous, Admiral. Unpredictable currents, and the atmospheric conditions will make drone deployment challenging. 
We'll need to send down a manned survey skiff. Lieutenant RagnorakRage and his 'Pathfinder' team are prepped and ready. 
They're the best we have for this kind of close-quarters reconnaissance.

Thorne nodded slowly, his gaze fixed on the flickering '1's in the grid. An archipelago that doesn't want to be found. 
Intriguing. The 'Pathfinders' have their orders. Tell Lieutenant Li to be cautious. We need that island count, but more importantly, 
I want my people back in one piece. This 'adventure,' as some of the younger crew are calling it, needs to be a successful one.

Understood, Admiral, Eva replied. Pathfinder team, deploy at 0600.

The grid pulsed, a silent challenge. The Odyssey's mission was clear: to chart the uncharted, 
to count the islands hidden within the whispers of an alien world. The adventure had begun.
"""

import sys
import numpy
import time
import os
import psutil

from collections import deque

#Control numpy grid output
numpy.set_printoptions(threshold=sys.maxsize)
numpy.set_printoptions(linewidth=300)

# Create a 100x100 grid, initially all water (0s)
gridSector_Delta = numpy.zeros((100, 100), dtype=int)

# --- Populating the grid with distinct and sophisticated islands ---
# We'll explicitly count them as we place them.
expected_islands = 0

# Island 1: The "Serpent's Head" Continent (Top-Left, irregular shape with a bay)
gridSector_Delta[5:25, 5:25] = 1  # Base block
gridSector_Delta[10:30, 20:30] = 1 # Extension to the right
gridSector_Delta[20:35, 10:15] = 1 # Extension downwards
gridSector_Delta[15:20, 0:5] = 1  # Small "finger" reaching left border
gridSector_Delta[10:15, 10:15] = 0 # A small "bay" (hole)
expected_islands += 1

# Island 2: The "Fractured Archipelago" (Central, multiple parts connected by thin bridges)
# Part A (Upper left block)
gridSector_Delta[40:45, 40:45] = 1
# Part B (Lower right block)
gridSector_Delta[55:60, 55:60] = 1
# Bridge 1 (horizontal connection)
gridSector_Delta[44, 45:55] = 1
# Bridge 2 (vertical connection)
gridSector_Delta[45:55, 54] = 1
# A connecting piece to ensure all parts form one island
gridSector_Delta[45, 54] = 1
expected_islands += 1

# Island 3: The "Jagged Peak" Continent (Bottom-Right, irregular, extends upwards)
gridSector_Delta[70:95, 70:95] = 1 # Base massive block
gridSector_Delta[65:70, 75:80] = 1 # Upward "finger"
gridSector_Delta[80:85, 90:98] = 1 # Side extension
gridSector_Delta[75, 75] = 0     # Small internal water body
gridSector_Delta[88, 88] = 0     # Another small internal water body
expected_islands += 1

# Island 4: The "Crescent Atoll" (Mid-Right, non-rectangular shape with a hole)
gridSector_Delta[30:40, 70:80] = 1 # Base rectangle
gridSector_Delta[31:38, 71:78] = 0 # Carve out inner hole
gridSector_Delta[35:40, 75:80] = 0 # Carve out part of lower side to form crescent
expected_islands += 1

# Island 5: The "Twin Eye" Island (Top-Right, a large island with two distinct internal lakes)
gridSector_Delta[5:20, 80:95] = 1 # Main body
gridSector_Delta[8:12, 83:87] = 0 # First lake
gridSector_Delta[13:17, 88:92] = 0 # Second lake
expected_islands += 1

# Island 6: The "Spine of Triton" (Long vertical island with side offshoots)
gridSector_Delta[0:99, 1] = 1    # Main vertical spine
gridSector_Delta[10, 2:5] = 1   # Offshoot 1
gridSector_Delta[50, 2:7] = 1   # Offshoot 2
gridSector_Delta[90, 2:4] = 1   # Offshoot 3
expected_islands += 1

# Island 7: Archipelago with rivers and lakes
gridSector_Delta[20:40, 50:70] = 1  # Main island body
gridSector_Delta[25:30, 55:60] = 0  # Lake 1
gridSector_Delta[32:37, 62:67] = 0  # Lake 2
gridSector_Delta[22:38, 52] = 1     # River 1
gridSector_Delta[22:38, 68] = 1     # River 2
gridSector_Delta[38, 52:68] = 1     # Connecting river
gridSector_Delta[42:45, 55:60] = 1  # Smaller island nearby
gridSector_Delta[42:45, 62:67] = 1  # Another smaller island
expected_islands += 1  # Main island + 4 smaller islands

# Island 8: A large, winding island
gridSector_Delta[60:80, 20:40] = 1
gridSector_Delta[65:75, 25:35] = 0  # Internal lake
expected_islands += 1

# Island 9: A small, isolated island
gridSector_Delta[5, 50] = 1
expected_islands += 1

# Island 10: Another small, isolated island
gridSector_Delta[95, 5] = 1
expected_islands += 1


# You can print the grid if you wish, but it will be very large
#print(gridSector_Delta)
"""gridSector_Delta = numpy.array([
  [1,1,0,0,1,0,0,1,0,0], # Row 0
  [1,1,0,0,0,1,0,0,0,0], # Row 1
  [0,0,0,0,0,0,0,0,0,1], # Row 2
  [0,0,0,1,0,1,0,0,0,0], # Row 3
  [1,0,0,0,0,0,0,0,0,0], # Row 4
  [1,1,0,0,1,0,1,0,0,0], # Row 5
  [0,0,0,0,1,1,1,0,0,0], # Row 6
  [0,0,0,0,0,1,0,0,0,0], # Row 7
  [1,0,0,1,0,0,0,1,1,0], # Row 8
  [0,0,0,0,0,0,0,1,1,0]  # Row 9
], dtype=int)"""

def numberOfislands (gridSector_Delta):

    #Preparing Expedition Tools
    mappedIslands = 0
    nextCoordinate = deque()
    traverseLogic = [[-1,0],[0,1],[1,0],[0,-1]]

    #Main Row traversing loop
    for gridRow in range(len(gridSector_Delta)):
        #Main Col traversing loop
        for gridCol in range(len(gridSector_Delta)):
            #Checking if its land
            if gridSector_Delta[gridRow][gridCol]:
                #Since its first contact it is a possible island
                mappedIslands += 1
                #Marking this block visited
                gridSector_Delta[gridRow][gridCol] = 0
                #Adding currentBlock coordinates
                nextCoordinate.append([gridRow, gridCol])
                currentRow = gridRow
                currentCol = gridCol
                #Scanning adjasent gridCoords to get nextCoordinate and visit them to scan as long as there are coordinates to visit
                while len(nextCoordinate):
                    currentRow = nextCoordinate[0][0]
                    currentCol = nextCoordinate[0][1]
                    #Traversing from current block to adjasent
                    for row, col in traverseLogic:
                        adjusentRow = currentRow + row
                        adjusentCol = currentCol + col
                        #Checking whether the current adjasent block meets the conditions to be a part of current island
                        if (adjusentRow >= 0 and adjusentRow < gridSector_Delta.shape[0] and adjusentCol >= 0 and adjusentCol < gridSector_Delta.shape[1]) and (gridSector_Delta[adjusentRow][adjusentCol]):
                            nextCoordinate.append([adjusentRow, adjusentCol])
                            gridSector_Delta[adjusentRow][adjusentCol] = 0
                    #Popping visited coordinate
                    nextCoordinate.popleft()
    
    return mappedIslands



start_time = time.perf_counter()
process = psutil.Process(os.getpid())
before_memory = process.memory_info().rss
print(f"Expected Output (Number of Islands): {expected_islands}")
print(f"Actual Output (Number of Islands){numberOfislands(gridSector_Delta)}")
end_time = time.perf_counter()
elapsed_time = end_time - start_time
after_memory = process.memory_info().rss
memory_used = after_memory - before_memory
print(f"Elapsed time: {elapsed_time} seconds")
print(f"Memory used: {memory_used} bytes")
print(f"Memory percent: {process.memory_percent()} %")