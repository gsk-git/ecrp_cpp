#include "headers/chunkManager.hpp"
#include <vector>
#include "FastNoise/include/FastNoise/FastNoiseLite.h"
#include "SFML/Graphics/Color.hpp"

FastNoiseLite elevationNoise;
std::vector<sf::Color> biome_colors = {
	sf::Color(170, 200, 120),   // 0 - Plains
	sf::Color(240, 224, 170),   // 1 - Beach
	sf::Color(216, 184, 100),   // 2 - Desert
	sf::Color(33, 87, 141),     // 3 - Ocean
	sf::Color(26, 123, 59),     // 4 - Jungle
	sf::Color(110, 118, 130)   // 5 - Mountains
};
std::vector<sf::Color> biome_variants = {
	// --- OCEAN & WATER ---
	sf::Color(23, 66, 145),     // 0 - Deep Ocean
	sf::Color(28, 107, 160),    // 1 - Ocean
	sf::Color(46, 139, 187),    // 2 - Shallow Ocean
	sf::Color(70, 160, 200),    // 3 - Lagoon / Coral Sea
	sf::Color(90, 190, 220),    // 4 - River / Freshwater

	// --- COAST ---
	sf::Color(240, 224, 170),   // 5 - Beach (warm sand)
	sf::Color(228, 193, 119),   // 6 - Desert Coast (dry sand)
	sf::Color(210, 190, 140),   // 7 - Rocky Shore

	// --- TEMPERATE ---
	sf::Color(170, 200, 120),   // 8 - Plains
	sf::Color(150, 185, 100),   // 9 - Meadow
	sf::Color(115, 150, 75),    // 10 - Forest
	sf::Color(34, 139, 69),     // 11 - Jungle
	sf::Color(98, 140, 88),     // 12 - Swamp / Marsh

	// --- ARID ---
	sf::Color(216, 184, 100),   // 13 - Desert
	sf::Color(190, 170, 90),    // 14 - Savannah
	sf::Color(150, 120, 65),    // 15 - Badlands
	sf::Color(180, 130, 70),    // 16 - Mesa / Clay Canyon

	// --- COLD ---
	sf::Color(110, 130, 150),   // 17 - Tundra
	sf::Color(90, 110, 140),    // 18 - Taiga (cold conifer forest)

	// --- SNOW & ICE ---
	sf::Color(230, 240, 255),   // 19 - Snow Field
	sf::Color(200, 220, 240),   // 20 - Ice Sheet
	sf::Color(180, 200, 225),   // 21 - Frozen Ocean

	// --- MOUNTAINS ---
	sf::Color(120, 130, 140),   // 22 - Mountain Base (rock)
	sf::Color(150, 160, 170),   // 23 - High Mountains (light rock)
	sf::Color(200, 210, 220),   // 24 - Snowy Peaks
};
