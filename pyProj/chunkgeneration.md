Step 1.1: Define Chunking Constants

These constants are the fundamental building blocks for how your vast world is divided and loaded. Think of them as the "blueprints" for your world grid.

    CHUNK_SIZE = 8
        What it is: This is the side length (in number of blocks) of a single "chunk" of your world.
        Human Term: Imagine your entire planet is divided into countless small squares, and each of these squares is exactly 8 blocks wide and 8 blocks high. Your program will load and generate the content of these small squares on demand.
        Why we need it: It's the atomic unit of world management. We load/unload the world in these 8x8 block packets.

    LOADED_CHUNK_RADIUS = 2
        What it is: This determines how many chunks away from the player's current chunk you want to keep loaded in all directions (horizontal and vertical).
        Human Term: If the player is in the "center" chunk, this means we want to load all chunks that are 2 steps to the left, 2 steps to the right, 2 steps up, and 2 steps down from that center chunk.
        Why we need it: To ensure the player always has a buffer of loaded world around them, so new chunks are generated/loaded before they walk into an empty area. This prevents "pop-in" of terrain.

    LOADED_AREA_DIMENSION_CHUNKS = (LOADED_CHUNK_RADIUS * 2) + 1
        What it is: This calculates the total number of chunks (in a square dimension) that will be active in your memory at any given time.
        Human Term: If LOADED_CHUNK_RADIUS is 2, then (2 * 2) + 1 = 5. This means your active, loaded world will always be a 5x5 grid of chunks. (The player's chunk is the center 1 chunk, plus 2 chunks on each side).
        Why we need it: This defines the "window" of chunks that your WorldManager will be responsible for.

    LOADED_AREA_DIMENSION_BLOCKS = LOADED_AREA_DIMENSION_CHUNKS * CHUNK_SIZE
        What it is: This calculates the total number of individual blocks (in a square dimension) that will be active in your memory at any given time.
        Human Term: If LOADED_AREA_DIMENSION_CHUNKS is 5 and CHUNK_SIZE is 8, then 5 * 8 = 40. This means your main data structure (which will replace the old PLANET_GRID_DIMENSION NumPy array) will be a 40x40 grid of Block objects.
        Why we need it: This tells us the fixed size of the NumPy array that will hold the Block sprites for your currently loaded part of the world. It's the size of your "active chunk cache."

    PIXEL_SIZE = 256
        What it is: The pixel dimension of a single block when drawn on the screen.
        Human Term: Each dirt or rock block will appear as a huge, detailed 256x256 pixel square.
        Why we need it: Your choice for visual detail and scale. This drastically reduces the number of blocks visible on screen, focusing on detail.

    SCREEN_WIDTH, SCREEN_HEIGHT
        What it is: The pixel dimensions of your game window.
        Human Term: How wide and tall your game screen is.
        Why we need it: Defines the canvas for your game.

    PLAYER_MOVE_SPEED_PER_SECOND = 1
        What it is: How many logical grid steps the player takes per second.
        Human Term: Player moves 1 block per second.
        Why we need it: Controls the player's movement pace.

    PLAYER_MOVE_DELAY_MS = 1000 // PLAYER_MOVE_SPEED_PER_SECOND
        What it is: The minimum time (in milliseconds) that must pass between player moves.
        Human Term: If PLAYER_MOVE_SPEED_PER_SECOND is 1, this is 1000ms. This prevents the player from "teleporting" rapidly if a key is held down.
        Why we need it: For controlled, discrete movement on the grid.

    FPS = 60
        What it is: The target frames per second for your game.
        Human Term: How many times per second the game updates its logic and draws a new picture.
        Why we need it: To ensure smooth animation and consistent game speed across different machines.

    LAYER_BACKGROUND_BLOCKS = 0 and LAYER_PLAYER = 1
        What it is: Numeric values for drawing layers in pygame.sprite.LayeredUpdates.
        Human Term: 0 is the "back" layer, 1 is "on top" of 0. This ensures blocks are drawn first, then the player on top of them.
        Why we need it: To control the visual order of sprites when they occupy the same screen space.

        self.image = pygame.Surface((PIXEL_SIZE, PIXEL_SIZE))
        self.image.fill(PYGAME_LIGHT_SAND)
        pygame.draw.rect(self.image,PYGAME_LIGHT_SAND,self.image.get_rect())
        pygame.draw.rect(self.image,BLACK,self.image.get_rect(),1,1)
                
        # Creating texture
        """grainCount = random.randint(0, PIXEL_SIZE)
        colors = [WHITE, PYGAME_DARK_DIRT, PYGAME_GRAY_ROCK]
        size = [0.5,1,2]

        for _ in range(grainCount):
            x = random.randint(0, PIXEL_SIZE)
            y = random.randint(0, PIXEL_SIZE)
            grainColor = random.choice(colors)
            grainSize = random.choice(size)
            pygame.draw.rect(self.image,grainColor,(x,y,grainSize,grainSize))"""