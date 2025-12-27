import pygame
import sys

pygame.init()

# --- Screen Setup ---
SCREEN_WIDTH = pygame.display.Info().current_w
SCREEN_HEIGHT = pygame.display.Info().current_w
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Scrollable Grid Demo")

# --- Grid & World Setup ---
WORLD_GRID_WIDTH_CELLS = 200  # Our "planet" is 50x50 cells
WORLD_GRID_HEIGHT_CELLS = 200
CELL_SIZE = 10              # Each cell is 32x32 pixels

WORLD_PIXEL_WIDTH = WORLD_GRID_WIDTH_CELLS * CELL_SIZE
WORLD_PIXEL_HEIGHT = WORLD_GRID_HEIGHT_CELLS * CELL_SIZE

# --- Camera Offset ---
camera_x = 0
camera_y = 0
camera_speed = 5 # Pixels per frame for scrolling

# --- Colors ---
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)

# --- Game Clock ---
clock = pygame.time.Clock()
FPS = 60

# --- Game Loop ---
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False

    # --- Camera Movement (Scrolling Logic) ---
    keys = pygame.key.get_pressed()
    if keys[pygame.K_LEFT]:
        camera_x -= camera_speed
    if keys[pygame.K_RIGHT]:
        camera_x += camera_speed
    if keys[pygame.K_UP]:
        camera_y -= camera_speed
    if keys[pygame.K_DOWN]:
        camera_y += camera_speed

    # --- Clamp Camera to World Bounds ---
    camera_x = max(0, camera_x)
    camera_x = min(camera_x, WORLD_PIXEL_WIDTH - SCREEN_WIDTH)
    if WORLD_PIXEL_WIDTH < SCREEN_WIDTH: # If world is smaller than screen, center it
        camera_x = 0 # Or (WORLD_PIXEL_WIDTH - SCREEN_WIDTH) // 2 for centering

    camera_y = max(0, camera_y)
    camera_y = min(camera_y, WORLD_PIXEL_HEIGHT - SCREEN_HEIGHT)
    if WORLD_PIXEL_HEIGHT < SCREEN_HEIGHT:
        camera_y = 0 # Or (WORLD_PIXEL_HEIGHT - SCREEN_HEIGHT) // 2 for centering


    # --- Drawing ---
    screen.fill(BLACK) # Clear the screen

    # --- Culling and Drawing Grid Cells ---
    # Calculate which cells are actually visible on screen
    start_col = max(0, camera_x // CELL_SIZE)
    end_col = min(WORLD_GRID_WIDTH_CELLS, (camera_x + SCREEN_WIDTH) // CELL_SIZE + 1) # +1 for cells partially visible
    start_row = max(0, camera_y // CELL_SIZE)
    end_row = min(WORLD_GRID_HEIGHT_CELLS, (camera_y + SCREEN_HEIGHT) // CELL_SIZE + 1)

    for r in range(start_row, end_row):
        for c in range(start_col, end_col):
            world_cell_x = c * CELL_SIZE
            world_cell_y = r * CELL_SIZE

            # Calculate screen coordinates using camera offset
            screen_cell_x = world_cell_x - camera_x
            screen_cell_y = world_cell_y - camera_y

            cell_rect = pygame.Rect(screen_cell_x, screen_cell_y, CELL_SIZE, CELL_SIZE)

            # Example: Draw a checkered pattern based on grid coordinates (r, c)
            if (r + c) % 2 == 0:
                color = (200, 200, 200) # Darker green
            else:
                color = (120, 120, 120) # Lighter green
            
            pygame.draw.rect(screen, color, cell_rect)
            pygame.draw.rect(screen, (0, 0, 0), cell_rect,1) # Black border

    pygame.display.flip() # Update the display

    clock.tick(FPS) # Control frame rate

pygame.quit()
sys.exit()