#include "level_controller.h"
#include "raylib.h"
#include "globals.h"
#include "player.h"
#include <fstream>


bool LevelController::is_inside_level(int row, int column) {
    if (row < 0 || row >= get_instance_level().get_current_level().get_rows()) return false;
    if (column < 0 || column >= get_instance_level().get_current_level().get_columns()) return false;
    return true;
}

bool LevelController::is_colliding(Vector2 pos, char look_for) {
    Rectangle entity_hitbox = {pos.x, pos.y, 1.0f, 1.0f};

    // Scan the adjacent area in the level to look for a match in collision
    for (int row = pos.y - 1; row < pos.y + 1; ++row) {
        for (int column = pos.x - 1; column < pos.x + 1; ++column) {
            // Check if the cell is out-of-bounds
            if (!get_instance_level().is_inside_level(row, column)) continue;
            if (get_level_cell(row, column) == look_for) {
                Rectangle block_hitbox = {(float) column, (float) row, 1.0f, 1.0f};
                if (CheckCollisionRecs(entity_hitbox, block_hitbox)) {
                    return true;
                }
            }
        }
    }
    return false;
}

char& LevelController::get_collider(Vector2 pos, char look_for) {
    // Like is_colliding(), except returns a reference to the colliding object
    Rectangle player_hitbox = {pos.x, pos.y, 1.0f, 1.0f};

    for (int row = pos.y - 1; row < pos.y + 1; ++row) {
        for (int column = pos.x - 1; column < pos.x + 1; ++column) {
            // Check if the cell is out-of-bounds
            if (!get_instance_level().is_inside_level(row, column)) continue;
            if (get_instance_level().get_level_cell(row, column) == look_for) {
                Rectangle block_hitbox = {(float) column, (float) row, 1.0f, 1.0f};
                if (CheckCollisionRecs(player_hitbox, block_hitbox)) {
                    return get_level_cell(row, column);
                }
            }
        }
    }

    // If failed, get an approximation
    return get_level_cell(pos.x, pos.y);
}

void LevelController::reset_level_index() {
    level_index = 0;
}

void LevelController::load_level(int offset) {
    level_index += offset;

    // Win logic
    if (level_index >= LEVEL_COUNT) {
        game_state = VICTORY_STATE;
        create_victory_menu_background();
        level_index = 0;
        return;
    }

    // Level duplication
    size_t rows = LevelController::get_instance_level().get_levels()[level_index].get_rows();
    size_t columns = LevelController::get_instance_level().get_levels()[level_index].get_columns();
    current_level_data = new char[rows * columns];



    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            const char* source_data = LevelController::get_instance_level().get_levels()[level_index].get_data(); // Use the getter
            current_level_data[row * columns + column] = source_data[row * columns + column];
        }
    }
    get_instance_level().set_current_level(level{rows, columns, current_level_data});
    // Instantiate entities
    player::get_instance_player().spawn_player();
    EnemiesController::get_instance().spawn_enemies();

    // Calculate positioning and sizes
    derive_graphics_metrics_from_loaded_level();

    // Reset the timer
    timer = MAX_LEVEL_TIME;
}

void LevelController::unload_level() {
    delete[] get_instance_level().get_current_level_data();
}

// Getters and setters



void LevelController::set_level_cell(size_t row,size_t column, char chr) {
    get_level_cell(row, column) = chr;
}
char& LevelController::get_level_cell(size_t row, size_t column) {
    return get_instance_level().get_current_level().get_data()[row * get_instance_level().get_current_level().get_columns() + column];
}
void LevelController::set_current_level(const level &current_level) {
    this->current_level = current_level;
}



    level LevelController::FileReadingFunc(const std::string& rleData) {
    std::vector<std::string> rows;
    std::string currentRow;
    std::string counter;

    for (size_t i = 0; i < rleData.length(); ++i) {
        char c = rleData[i];

        if (c == '|') {
            rows.push_back(currentRow);
            currentRow.clear();
            counter.clear();
        } else if (c == ';') {
            if (!currentRow.empty()) {
                rows.push_back(currentRow);
                currentRow.clear();
            }
            break;
        } else if (std::isdigit(c)) {
            counter += c;
        } else {
            int count = counter.empty() ? 1 : std::stoi(counter);
            counter.clear();
            currentRow.append(count, c);
        }
    }

    if (!currentRow.empty()) {
        rows.push_back(currentRow);
    }

    if (rows.empty()) {
        throw std::runtime_error("RLE data is empty or malformed.");
    }

    size_t rowCount = rows.size();
    size_t colCount = rows[0].length();
    char* levelData = new char[rowCount * colCount];

    for (size_t r = 0; r < rowCount; ++r) {
        if (rows[r].length() != colCount) {
            delete[] levelData;
            throw std::runtime_error("Inconsistent row lengths in RLE data.");
        }
        for (size_t c = 0; c < colCount; ++c) {
            levelData[r * colCount + c] = rows[r][c];
        }
    }

    return {rowCount, colCount, levelData};
}

    std::vector<level> LevelController::TakeLevelsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';') {
            continue;
        }
            levels.push_back(FileReadingFunc(line));
        }
    if (levels.empty()) {
        throw std::runtime_error("No valid levels were loaded from the file.");
    }

    return levels;
}
