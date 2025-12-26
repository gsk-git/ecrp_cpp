"""
Mission Log: Starship Odyssey - Uncharted Terrestrial Planet Survey

A faint, long-dormant beacon signal, fragmented but unmistakably artificial, has shattered the deep space silence. 
After weeks of high-resolution triangulation, we've managed to zero in on its possible origin: a volatile 50x50 kilometer sector 
of an uncharted terrestrial planet. This isn't a simple 'land or water' scenario; our preliminary atmospheric scans indicate 
a chaotic tapestry of geological anomalies, treacherous zones, and unknown energy signatures.

Our objective is not to land. Odyssey is a reconnaissance vessel, and the terrain stability is unknown. 
Instead, our advanced orbital scanners must analyze each square kilometer of this 50x50 grid. 
Every block holds an 'object' – a unique geological formation, an atmospheric phenomenon, or a physical obstacle. We must:

1.  **Determine Navigable Terrain:** Based on each object's detailed geological properties (rock, silicate, water, carbon density, composition) a
nd structural stability, our system must classify each square kilometer as 'traversable' (land for our survey drones) or 'impassable' (water/hazard). 
There's no direct 'is_land' flag; it's a complex scientific analysis.

2.  **Locate the Beacon Source:** Within this complex terrain, somewhere is the source of the beacon. We don't know if it's an ancient base, 
a derelict crew ship, or something entirely alien.

3.  **Uncover the Beacon's Secret:** The moment we pinpoint the Beacon Source, its true nature (its 'encoded discovery data') must be extracted a
nd reported. This information, hidden within the object itself, is the core of our mission.

This is Odyssey's most critical mission yet: to navigate the unknown, analyze the unanalyzable, 
and unveil the secret of the long-lost beacon. Failure is not an option.
"""
import random
import numpy
import sys
import time
import os
import psutil
from collections import deque

numpy.set_printoptions(threshold=sys.maxsize)
numpy.set_printoptions(linewidth=300)

# --- Constants for geological analysis ---
MIN_STABILITY_FOR_TRAVERSAL = 0.5
MAX_WATER_COMPOSITION_FOR_TRAVERSAL = 0.3
MIN_SOLID_COMPOSITION_FOR_TRAVERSAL = 0.4 # Combined rock/silicate
TRAVERSE_LOGIX = deque([[-1,0],[0,1],[1,0],[0,-1]])
PLANET_GRID_SIZE = 50

# --- Operations Handlers ---
toTraverse = deque()
scannedGrid = numpy.zeros((PLANET_GRID_SIZE,PLANET_GRID_SIZE))

# --- Creating Classes
class TerrainObject:
    """Base class for all terrain features, requiring geological analysis for traversability."""
    def __init__(self, name, description,
                 geological_properties=None, stability=0.0, is_obstacle=False):
        self.name = name
        self.description = description
        self.geological_properties = geological_properties if geological_properties is not None else {
            'rock': 0.0, 'silicates': 0.0, 'water': 1.0, 'carbon': 0.0
        }
        self.stability = stability # 0.0 (unstable) to 1.0 (very stable)
        self.is_obstacle = is_obstacle # Direct obstacles are always impassable
        self.details = f"Details: {description}" # Hidden info for scanning
    def is_traversable(self):
        if self.is_obstacle:
            return False
        
        if self.stability < MIN_STABILITY_FOR_TRAVERSAL:
            return False
        
        if self.geological_properties.get('water',0.0) > MAX_WATER_COMPOSITION_FOR_TRAVERSAL:
            return False
        
        if self.geological_properties.get('rock',0.0) + self.geological_properties.get('silicates',0.0) >= MIN_SOLID_COMPOSITION_FOR_TRAVERSAL:
            return False
        return True
    
    def __repr__(self):
        status = "Traversable" if self.is_traversable() else "Impassable require(s) ariel transit"
        return f"{self.name}({status}, Stab:{self.stability:.1f})"

# --- Specific Terrain Types ---
class EmptySpace(TerrainObject):
    """Vast, deep space / unchartable abyss."""
    def __init__(self):
        super().__init__("Empty Space", "A void where nothing can exist.",
                         geological_properties={'rock': 0, 'silicates': 0, 'water': 1, 'carbon': 0},
                         stability=0.0)

class RockyTerrain(TerrainObject):
    """Solid, high-density rock formations."""
    def __init__(self, name="Rocky Terrain"):
        super().__init__(name, "Dense, ancient rock formations.",
                         geological_properties={'rock': random.uniform(0.6, 0.9), 'silicates': random.uniform(0.05, 0.2), 'water': random.uniform(0, 0.1), 'carbon': random.uniform(0, 0.05)},
                         stability=random.uniform(0.7, 1.0))

class VolcanicCrater(TerrainObject):
    """Highly unstable, molten areas."""
    def __init__(self):
        super().__init__("Volcanic Crater", "A bubbling, molten pit.",
                         geological_properties={'rock': 0.1, 'silicates': 0.1, 'water': 0.4, 'carbon': 0.0},
                         stability=random.uniform(0.0, 0.4)) # Low stability

class HydrothermalVent(TerrainObject):
    """Underwater/subsurface hot vents, too watery/unstable."""
    def __init__(self):
        super().__init__("Hydrothermal Vent", "Geothermal activity creating hot, unstable water flows.",
                         geological_properties={'rock': 0.05, 'silicates': 0.05, 'water': random.uniform(0.7, 0.9), 'carbon': 0.1},
                         stability=random.uniform(0.1, 0.3)) # High water, low stability

# --- Obstacle Types (Always Impassable) ---
class DenseFog(TerrainObject):
    """Thick atmospheric conditions, blocking all sensors and traversal."""
    def __init__(self):
        super().__init__("Dense Fog", "An atmospheric anomaly, impenetrable.",
                         geological_properties={'rock': 0, 'silicates': 0, 'water': 1, 'carbon': 0}, # Irrelevant, but for completeness
                         stability=0.0, is_obstacle=True)

class MagneticAnomaly(TerrainObject):
    """Intense magnetic fields disrupting all systems."""
    def __init__(self):
        super().__init__("Magnetic Anomaly", "A localized magnetic field, disrupts all electronics.",
                         geological_properties={}, stability=0.0, is_obstacle=True)

class EnergyBarrier(TerrainObject):
    """An unknown, static energy field."""
    def __init__(self):
        super().__init__("Energy Barrier", "A shimmering, impassable energy wall.",
                         geological_properties={}, stability=0.0, is_obstacle=True)

# --- The Target Object ---
class BeaconSource(TerrainObject):
    """The mysterious beacon emitting the signal."""
    def __init__(self, encoded_data, name="Beacon Source"):
        # Beacon is always on highly stable, solid ground
        super().__init__(name, "The origin of the signal.",
                         geological_properties={'rock': random.uniform(0.7, 0.9), 'silicates': random.uniform(0.1, 0.2), 'water': random.uniform(0, 0.05), 'carbon': random.uniform(0, 0.05)},
                         stability=random.uniform(0.9, 1.0))
        self.is_obstacle = False # It's a landable/surveyable spot
        self.encoded_discovery_data = encoded_data # The secret to uncover

    def __repr__(self):
        status = "Traversable" if self.is_traversable() else "IMPASSABLE (ERROR)"
        return f"{self.name}({status}, ID:{self.encoded_discovery_data[:5]}...)"
    
# Triangulating baecon's last know possible grid sector
def generateGrid(PLANET_GRID_SIZE):

    gridSector_Delta = numpy.full((PLANET_GRID_SIZE, PLANET_GRID_SIZE), EmptySpace(), dtype=object)

    # --- Populating the grid with diverse terrain objects ---
    # Create some large landmasses (RockyFormations)
    for _ in range(5): # Create 5 large rocky areas
        start_r = random.randint(0, PLANET_GRID_SIZE - 10)
        start_c = random.randint(0, PLANET_GRID_SIZE - 10)
        height = random.randint(5, 15)
        width = random.randint(5, 15)
        
        for r in range(start_r, min(start_r + height, PLANET_GRID_SIZE)):
            for c in range(start_c, min(start_c + width, PLANET_GRID_SIZE)):
                gridSector_Delta[r, c] = RockyTerrain()

    # Scatter various other terrains and obstacles
    for r in range(PLANET_GRID_SIZE):
        for c in range(PLANET_GRID_SIZE):
            if isinstance(gridSector_Delta[r, c], EmptySpace): # Only change empty spots
                choice = random.random()
                if choice < 0.02: # 2% chance of Magnetic Anomaly
                    gridSector_Delta[r, c] = MagneticAnomaly()
                elif choice < 0.04: # 2% chance of Energy Barrier
                    gridSector_Delta[r, c] = EnergyBarrier()
                elif choice < 0.07: # 3% chance of Dense Fog
                    gridSector_Delta[r, c] = DenseFog()
                elif choice < 0.10: # 3% chance of Volcanic Crater
                    gridSector_Delta[r, c] = VolcanicCrater()
                elif choice < 0.12: # 2% chance of Hydrothermal Vent
                    gridSector_Delta[r, c] = HydrothermalVent()

    # --- Place the single "Long Lost Beacon Source" ---
    # Ensure the beacon is placed on a traversable RockyTerrain
    beacon_placed = False
    while not beacon_placed:
        br, bc = random.randint(0, PLANET_GRID_SIZE - 1), random.randint(0, PLANET_GRID_SIZE - 1)
        if isinstance(gridSector_Delta[br, bc], RockyTerrain): # Ensure it's on rock
            gridSector_Delta[br, bc] = BeaconSource("Pattern Sequence 7.2.9 Repeating. What Is Its Purpose?!", name="The Weaver's Loom")
            beacon_placed = True
    return gridSector_Delta

start_time = time.perf_counter()
process = psutil.Process(os.getpid())
before_memory = process.memory_info().rss
gridSector_Delta  = generateGrid(PLANET_GRID_SIZE)
end_time = time.perf_counter()
elapsed_time = end_time - start_time
after_memory = process.memory_info().rss
memory_used = after_memory - before_memory
print(f"Elapsed time: {elapsed_time:.2f} seconds")
print(f"Memory used: {memory_used/(1024*1024):.2f} MB ({process.memory_percent():.2f} %)")

# ------------------------Traverse through the grid chart and record all blocks' properties and unravel hidden beacon's mystery -------------------


def chartingGridSector_Delta (gridSector_Delta):

    for gridLat in range(0,1):
        for gridLong in range(len(gridSector_Delta)):
            if isinstance:
                print(gridSector_Delta[gridLat][gridLong])
    return 

chartingGridSector_Delta(gridSector_Delta)