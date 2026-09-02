#include "levels/MapGenerator.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const int WIDTH = 160;
const int HEIGHT = 20;
const int GROUND_ROW = 17;

static int randInt(int min, int max) { return min + std::rand() % (max - min + 1); }
static float randFloat() { return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX); }

struct GameMap {
    std::vector<std::string> grid;
    std::vector<int> heightMap;

    GameMap() : grid(HEIGHT, std::string(WIDTH, '.')), heightMap(WIDTH, GROUND_ROW) {}

    bool isEmpty(int r, int c) const {
        if (r < 0 || r >= HEIGHT || c < 0 || c >= WIDTH) return false;
        return grid[r][c] == '.';
    }

    bool isSolid(int c) const { return heightMap[c] != HEIGHT; }

    void buildTerrain(int c, int topR) {
        if (c < 0 || c >= WIDTH) return;
        grid[topR][c] = 'T';
        for (int r = topR + 1; r < GROUND_ROW; ++r) grid[r][c] = 'D';
        heightMap[c] = std::min(heightMap[c], topR);
    }
};

void MapGenerator::generateMap(int level, const std::string& outputPath) {
    GameMap m;

    const int SAFE_START_COL = 20;

    int minPits = (level == 1) ? 2 : (level == 2 ? 4 : 6);
    int maxPits = (level == 1) ? 3 : (level == 2 ? 6 : 9);
    float terrainProb = (level == 1) ? 0.3f : (level == 2 ? 0.5f : 0.6f);
    float brickProb = (level == 1) ? 0.4f : (level == 2 ? 0.5f : 0.6f);
    float coinProb = 0.4f;
    std::vector<char> gndEnemies = (level == 1) ? std::vector<char>{'G'} : (level == 2 ? std::vector<char>{'G', 'K'} : std::vector<char>{'G', 'K', 'K'});
    std::vector<char> snipers = (level == 1) ? std::vector<char>{'H'} : std::vector<char>{'H', 'h'};

    // 1. Pits
    int numPits = randInt(minPits, maxPits);
    std::vector<int> possiblePits;
    for (int i = 15; i < WIDTH - 20; i += 10) possiblePits.push_back(i);
    std::random_shuffle(possiblePits.begin(), possiblePits.end());
    std::vector<int> pitCols;
    for (int i = 0; i < std::min(numPits, (int)possiblePits.size()); ++i) {
        int pitCol = possiblePits[i];
        int pitWidth = randInt(2, (level == 1 ? 3 : (level == 2 ? 4 : 5)));
        for (int c = pitCol; c < pitCol + pitWidth; ++c) {
            if (c < WIDTH) { m.heightMap[c] = HEIGHT; pitCols.push_back(c); }
        }
    }

    // Ground Base
    for (int c = 0; c < WIDTH; ++c) {
        if (m.heightMap[c] == GROUND_ROW) {
            m.grid[GROUND_ROW][c] = '#';
            m.grid[GROUND_ROW+1][c] = '=';
            m.grid[GROUND_ROW+2][c] = '=';
        }
    }

    // Safe zones
    for(int c=0; c<8; ++c) { m.heightMap[c]=GROUND_ROW; m.grid[GROUND_ROW][c]='#'; m.grid[GROUND_ROW+1][c]='='; m.grid[GROUND_ROW+2][c]='='; }
    for(int c=WIDTH-15; c<WIDTH; ++c) { m.heightMap[c]=GROUND_ROW; m.grid[GROUND_ROW][c]='#'; m.grid[GROUND_ROW+1][c]='='; m.grid[GROUND_ROW+2][c]='='; }

    auto checkFlat = [&](int st, int w) {
        if (st+w >= WIDTH-10) return false;
        for (int i=st; i<st+w; ++i) if(m.heightMap[i] != GROUND_ROW) return false;
        return true;
    };

    // 2. Terrain Patterns
    int col = 10;
    while (col < WIDTH - 20) {
        if (randFloat() < terrainProb) {
            int pat = randInt(1, 6);
            if (pat == 1 && checkFlat(col, 6)) { // Flat
                int w = randInt(4, 6), h = randInt(2, 4);
                for(int c=col; c<col+w; ++c) m.buildTerrain(c, GROUND_ROW - h);
                col += w + 2;
            } else if (pat == 2 && checkFlat(col, 7)) { // Pyramid
                m.buildTerrain(col, GROUND_ROW-1); m.buildTerrain(col+6, GROUND_ROW-1);
                m.buildTerrain(col+1, GROUND_ROW-2); m.buildTerrain(col+5, GROUND_ROW-2);
                m.buildTerrain(col+2, GROUND_ROW-3); m.buildTerrain(col+3, GROUND_ROW-3); m.buildTerrain(col+4, GROUND_ROW-3);
                // Sniper
                if (col + 3 >= SAFE_START_COL && randFloat() < (level * 0.3f)) m.grid[GROUND_ROW-4][col+3] = snipers[randInt(0, snipers.size()-1)];
                col += 8;
            } else if (pat == 3 && checkFlat(col, 6)) { // Staircase
                int h = 1;
                for(int c=col; c<col+6; c+=2) {
                    m.buildTerrain(c, GROUND_ROW-h); m.buildTerrain(c+1, GROUND_ROW-h); h++;
                }
                col += 7;
            } else if (pat == 4 && checkFlat(col, 8)) { // Pillars
                m.buildTerrain(col, GROUND_ROW-3); m.buildTerrain(col+1, GROUND_ROW-3);
                m.buildTerrain(col+4, GROUND_ROW-4); m.buildTerrain(col+5, GROUND_ROW-4);
                m.buildTerrain(col+8, GROUND_ROW-2); m.buildTerrain(col+9, GROUND_ROW-2);
                col += 11;
            } else if (pat == 5 && checkFlat(col, 10)) { // Valley
                for(int c=col; c<col+3; ++c) m.buildTerrain(c, GROUND_ROW-4);
                // gap is just ground
                for(int c=col+7; c<col+10; ++c) m.buildTerrain(c, GROUND_ROW-4);
                // Valley ambush
                m.grid[GROUND_ROW-1][col+4] = 'K';
                m.grid[GROUND_ROW-1][col+5] = 'G';
                col += 11;
            } else if (pat == 6 && checkFlat(col, 7)) { // Birthday Cake
                int bw = randInt(5, 7);
                for(int c=col; c<col+bw; ++c) m.buildTerrain(c, GROUND_ROW-1);
                int lw = randInt(3, bw-1);
                int lo = randInt(0, bw-lw);
                for(int c=col+lo; c<col+lo+lw; ++c) m.buildTerrain(c, GROUND_ROW-2);
                if (lw >= 3) {
                    int tlw = randInt(1, lw-1);
                    int tlo = randInt(0, lw-tlw);
                    for(int c=col+lo+tlo; c<col+lo+tlo+tlw; ++c) m.buildTerrain(c, GROUND_ROW-3);
                }
                col += bw + 2;
            } else col++;
        } else col++;
    }

    // Edge guards (Enemy Pattern 2)
    for (int i = 0; i < WIDTH - 5; ++i) { // Need at least some space
        if (m.heightMap[i] == HEIGHT && m.heightMap[i+1] <= GROUND_ROW) {
            // Check if platform is at least 4 blocks wide
            bool wideEnough = true;
            for (int k = 1; k <= 4; ++k) {
                if (m.heightMap[i+k] != m.heightMap[i+1]) wideEnough = false;
            }
            if (wideEnough && randFloat() < (level * 0.3f) && m.isEmpty(m.heightMap[i+1]-1, i+1)) {
                m.grid[m.heightMap[i+1]-1][i+1] = 'K';
            }
        }
    }

    // 2.5 Pipes
    col = 15;
    while (col < WIDTH - 20) {
        if (randFloat() < 0.15f) {
            // Check if we have 4 flat tiles
            bool flat = true;
            int baseH = m.heightMap[col];
            if (baseH == HEIGHT) flat = false; // Pit
            for (int i = 0; i < 4; ++i) {
                if (m.heightMap[col + i] != baseH) flat = false;
            }
            if (flat) {
                int ph = randInt(2, 4); // Pipe height (2 to 4)
                // Draw pipe at col+1, col+2
                for (int h = 1; h < ph; ++h) {
                    m.grid[baseH - h][col + 1] = '|';
                    m.grid[baseH - h][col + 2] = '|';
                }
                m.grid[baseH - ph][col + 1] = 'W';
                m.grid[baseH - ph][col + 2] = 'W';
                
                // Update height map so bricks don't overlap
                m.heightMap[col + 1] = baseH - ph;
                m.heightMap[col + 2] = baseH - ph;
                
                // Spawn enemy (Piranha Plant / Goomba) sometimes?
                // The game might not have Piranha Plant, so just leave it empty or spawn a Koopa?
                
                col += 5; // Skip past pipe
                continue;
            }
        }
        col++;
    }

    // 3. Bricks
    auto checkClear = [&](int st, int w, int r) {
        for(int c=st; c<st+w; ++c) {
            if (c >= WIDTH) return false;
            for(int dr = r-2; dr <= r+2; ++dr) {
                if (!m.isEmpty(dr, c)) return false;
            }
        }
        return true;
    };

    col = 10;
    while (col < WIDTH - 15) {
        if (randFloat() < brickProb) {
            int pat = randInt(1, 5);
            int maxH = HEIGHT;
            for(int i=col; i<col+6; ++i) maxH = std::min(maxH, m.heightMap[i]);
            int baseR = maxH - 4;
            
            if (baseR > 4) {
                if (pat == 1 && checkClear(col, 5, baseR)) { // Flat Line
                    m.grid[baseR][col] = 'B'; m.grid[baseR][col+1] = 'B'; 
                    m.grid[baseR][col+2] = '?'; m.grid[baseR][col+3] = 'B'; m.grid[baseR][col+4] = 'B';
                    // Spawn enemy on top if level >= 2
                    if (col + 2 >= SAFE_START_COL && level >= 2 && randFloat() < 0.4f && m.isEmpty(baseR-1, col+2)) {
                        m.grid[baseR-1][col+2] = gndEnemies[randInt(0, gndEnemies.size()-1)];
                    }
                    col += 7;
                } else if (pat == 2 && checkClear(col, 7, baseR)) { // Two-Tier
                    m.grid[baseR][col] = 'B'; m.grid[baseR][col+1] = 'B';
                    m.grid[baseR-2][col+3] = 'B'; m.grid[baseR-2][col+4] = '?'; m.grid[baseR-2][col+5] = 'B';
                    col += 8;
                } else if (pat == 3 && checkClear(col, 8, baseR - 1)) { // Canopy
                    for(int c=col; c<col+8; ++c) m.grid[baseR-1][c] = 'B';
                    // Spawn enemy on Canopy
                    if (col + 4 >= SAFE_START_COL && randFloat() < 0.6f && m.isEmpty(baseR-2, col+4)) {
                        m.grid[baseR-2][col+4] = gndEnemies[randInt(0, gndEnemies.size()-1)];
                    }
                    col += 10;
                } else if (pat == 4 && baseR > 5 && checkClear(col, 5, baseR-1)) { // Hidden Cache
                    m.grid[baseR-3][col] = 'B'; m.grid[baseR-3][col+1] = 'B'; m.grid[baseR-3][col+2] = 'B'; m.grid[baseR-3][col+3] = 'B'; m.grid[baseR-3][col+4] = 'B';
                    m.grid[baseR][col] = 'B'; m.grid[baseR][col+2] = '?'; m.grid[baseR][col+4] = 'B';
                    // Spawn enemy on top
                    if (col + 2 >= SAFE_START_COL && level >= 2 && randFloat() < 0.5f && m.isEmpty(baseR-4, col+2)) {
                        m.grid[baseR-4][col+2] = gndEnemies[randInt(0, gndEnemies.size()-1)];
                    }
                    col += 7;
                } else if (pat == 5 && checkClear(col, 7, baseR)) { // Scattered
                    m.grid[baseR][col] = 'B'; m.grid[baseR][col+2] = 'B'; m.grid[baseR][col+3] = '?'; m.grid[baseR][col+6] = 'B';
                    col += 9;
                } else col++;
            } else col++;
        } else col++;
    }

    // 4. Coins
    col = 10;
    while (col < WIDTH - 10) {
        if (randFloat() < coinProb) {
            int pat = randInt(1, 5);
            int maxH = HEIGHT;
            for(int i=col; i<col+5; ++i) maxH = std::min(maxH, m.heightMap[i]);
            
            if (pat == 1) { // Arc (Check for pit or flat)
                int r = maxH - 2;
                int pts[5][2] = {{0,0}, {1,-1}, {2,-2}, {3,-1}, {4,0}};
                bool ok = true;
                for(int i=0; i<5; ++i) if(!m.isEmpty(r+pts[i][1], col+pts[i][0])) ok = false;
                if(ok) {
                    for(int i=0; i<5; ++i) m.grid[r+pts[i][1]][col+pts[i][0]] = 'C';
                }
            } else if (pat == 2) { // Top-line
                int r = maxH - 5;
                if (r > 0) {
                    bool ok = true;
                    for(int c=col; c<col+4; ++c) if(!m.isEmpty(r, c)) ok = false;
                    if(ok) {
                        for(int c=col; c<col+4; ++c) m.grid[r][c] = 'C';
                    }
                }
            } else if (pat == 3) { // Block
                int r = maxH - 3;
                if (r > 2 && m.isEmpty(r,col) && m.isEmpty(r,col+1) && m.isEmpty(r-1,col) && m.isEmpty(r-1,col+1)) {
                    m.grid[r][col] = 'C'; m.grid[r][col+1] = 'C'; m.grid[r-1][col] = 'C'; m.grid[r-1][col+1] = 'C';
                }
            } else if (pat == 4) { // Diamond
                int r = maxH - 3;
                if (r > 2 && m.isEmpty(r,col+1) && m.isEmpty(r-1,col) && m.isEmpty(r-1,col+2) && m.isEmpty(r-2,col+1)) {
                    m.grid[r][col+1]='C'; m.grid[r-1][col]='C'; m.grid[r-1][col+2]='C'; m.grid[r-2][col+1]='C';
                }
            } else if (pat == 5) { // Bait
                if (m.heightMap[col] == HEIGHT && m.isEmpty(GROUND_ROW-1, col-1)) {
                    m.grid[GROUND_ROW-1][col-1] = 'C'; // Edge of pit
                }
            }
            col += 6;
        } else col++;
    }

    // 5. Extra Enemies
    // Patrols (ensure enough space to move)
    int start = 0;
    while (start < WIDTH) {
        if (m.heightMap[start] == HEIGHT) { start++; continue; }
        int end = start;
        while (end < WIDTH && m.heightMap[end] == m.heightMap[start]) end++;
        int len = end - start;
        if (len >= 6) { // Minimum 6 blocks of flat ground to live comfortably
            if (randFloat() < 0.5f) {
                int r = m.heightMap[start] - 1;
                int c = start + len/2;
                if (c >= SAFE_START_COL && m.isEmpty(r, c)) {
                    m.grid[r][c] = gndEnemies[randInt(0, gndEnemies.size()-1)];
                }
            }
            if (len >= 12 && randFloat() < 0.4f) { // Second enemy if huge space
                int r = m.heightMap[start] - 1;
                int c = start + len/4;
                if (c >= SAFE_START_COL && m.isEmpty(r, c)) {
                    m.grid[r][c] = gndEnemies[randInt(0, gndEnemies.size()-1)];
                }
            }
        }
        start = end;
    }
    // Air Strike
    if (level >= 2) {
        for (int i=0; i < (level == 2 ? 2 : 4); ++i) {
            int ec = randInt(40, WIDTH-30);
            int er = randInt(5, 12);
            if (m.isEmpty(er, ec)) m.grid[er][ec] = 'E';
        }
    }

    m.grid[GROUND_ROW-1][1] = 'P';
    m.grid[GROUND_ROW-1][WIDTH-4] = 'X';
    if (level == 3) m.grid[GROUND_ROW-1][WIDTH-8] = 'Z';

    std::ofstream out(outputPath);
    if (!out) return;
    
    out << "@name=Level " << level << "\n";
    if (level == 1) out << "@difficulty=Easy\n";
    else if (level == 2) out << "@difficulty=Medium\n";
    else out << "@difficulty=Hard\n";
    out << "@tile_size=48\n@map\n";

    for (int r = 0; r < HEIGHT; ++r) {
        out << m.grid[r] << "\n";
    }
    out.close();
}

void MapGenerator::generateSubLevel(const std::string& outputPath) {
    const int SUB_WIDTH = 60;
    const int SUB_HEIGHT = 20;
    const int SUB_GROUND_ROW = 17;
    const int SUB_SAFE_START_COL = 18;

    std::vector<std::string> grid(SUB_HEIGHT, std::string(SUB_WIDTH, '.'));

    // Ground base
    for (int c = 0; c < SUB_WIDTH; ++c) {
        grid[SUB_GROUND_ROW][c] = '#';
        grid[SUB_GROUND_ROW + 1][c] = '=';
        grid[SUB_GROUND_ROW + 2][c] = '=';
    }

    // 1. Entrance Pipe at Start (Columns 4, 5)
    grid[SUB_GROUND_ROW - 2][4] = 'W';
    grid[SUB_GROUND_ROW - 2][5] = 'W';
    grid[SUB_GROUND_ROW - 1][4] = '|';
    grid[SUB_GROUND_ROW - 1][5] = '|';

    // Player Spawn near entrance pipe
    grid[SUB_GROUND_ROW - 1][2] = 'P';

    // 2. Exit Pipe at End (Columns 54, 55)
    grid[SUB_GROUND_ROW - 2][SUB_WIDTH - 6] = 'W';
    grid[SUB_GROUND_ROW - 2][SUB_WIDTH - 5] = 'W';
    grid[SUB_GROUND_ROW - 1][SUB_WIDTH - 6] = '|';
    grid[SUB_GROUND_ROW - 1][SUB_WIDTH - 5] = '|';

    // 3. Middle Procedural Content (Columns 8 to 50)
    int col = 8;
    while (col <= 46) {
        int pattern = randInt(1, 5);
        
        if (pattern == 1) {
            // Pattern 1: Question Blocks & Coins
            int numBlocks = randInt(3, 5);
            int blockRow = SUB_GROUND_ROW - 4; // Row 13
            for (int i = 0; i < numBlocks; ++i) {
                int c = col + i;
                if (c < 50) {
                    grid[blockRow][c] = (i % 2 == 1) ? '?' : 'B';
                    grid[blockRow - 1][c] = 'C'; // Coin above block
                }
            }
            if (randFloat() < 0.6f && col + 1 >= SUB_SAFE_START_COL && col + 1 < 50) {
                grid[SUB_GROUND_ROW - 1][col + 1] = 'G'; // Patrol enemy
            }
            col += numBlocks + randInt(2, 3);
        } else if (pattern == 2) {
            // Pattern 2: Elevated Terrain Plateau
            int plateauWidth = randInt(4, 6);
            int plateauHeight = randInt(2, 3);
            int topR = SUB_GROUND_ROW - plateauHeight;
            for (int c = col; c < col + plateauWidth && c < 50; ++c) {
                grid[topR][c] = 'T';
                for (int r = topR + 1; r < SUB_GROUND_ROW; ++r) {
                    grid[r][c] = 'D';
                }
                if (randFloat() < 0.7f) {
                    grid[topR - 1][c] = 'C'; // Coin on top
                }
            }
            if (randFloat() < 0.5f && col + 2 >= SUB_SAFE_START_COL && col + 2 < 50) {
                grid[topR - 1][col + 2] = (randFloat() < 0.5f) ? 'K' : 'G';
            }
            col += plateauWidth + randInt(2, 3);
        } else if (pattern == 3) {
            // Pattern 3: Stepping Stones (Brick Stairs)
            int step1 = col;
            int step2 = col + 2;
            int step3 = col + 4;
            if (step3 + 1 < 50) {
                grid[SUB_GROUND_ROW - 2][step1] = 'B';
                grid[SUB_GROUND_ROW - 2][step1 + 1] = 'B';

                grid[SUB_GROUND_ROW - 3][step2] = '?';
                grid[SUB_GROUND_ROW - 3][step2 + 1] = 'B';

                grid[SUB_GROUND_ROW - 4][step3] = 'B';
                grid[SUB_GROUND_ROW - 4][step3 + 1] = 'B';
                grid[SUB_GROUND_ROW - 5][step3] = 'C';
                grid[SUB_GROUND_ROW - 5][step3 + 1] = 'C';
            }
            col += 7;
        } else if (pattern == 4) {
            // Pattern 4: Coin Arch
            if (col + 5 < 50) {
                grid[SUB_GROUND_ROW - 2][col] = 'C';
                grid[SUB_GROUND_ROW - 3][col + 1] = 'C';
                grid[SUB_GROUND_ROW - 4][col + 2] = 'C';
                grid[SUB_GROUND_ROW - 4][col + 3] = 'C';
                grid[SUB_GROUND_ROW - 3][col + 4] = 'C';
                grid[SUB_GROUND_ROW - 2][col + 5] = 'C';

                if (randFloat() < 0.6f && col + 2 >= SUB_SAFE_START_COL) {
                    grid[SUB_GROUND_ROW - 1][col + 2] = 'G';
                }
            }
            col += 7;
        } else {
            // Pattern 5: Small Pipe or Brick Cache
            if (col + 3 < 50) {
                grid[SUB_GROUND_ROW - 3][col + 1] = '?';
                grid[SUB_GROUND_ROW - 3][col] = 'B';
                grid[SUB_GROUND_ROW - 3][col + 2] = 'B';
                if (col + 1 >= SUB_SAFE_START_COL) {
                    grid[SUB_GROUND_ROW - 1][col + 1] = 'G';
                }
            }
            col += 5;
        }
    }

    std::ofstream out(outputPath);
    if (!out) return;

    out << "@name=Bonus Sub-Level\n";
    out << "@difficulty=Easy\n";
    out << "@tile_size=48\n";
    out << "@map\n";

    for (int r = 0; r < SUB_HEIGHT; ++r) {
        out << grid[r] << "\n";
    }
    out.close();
}

int main(int argc, char* argv[]) { if (argc < 3) return 1; int difficulty = std::stoi(argv[1]); MapGenerator::generateMap(difficulty, argv[2]); return 0; }
