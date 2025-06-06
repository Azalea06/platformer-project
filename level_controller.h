#ifndef LEVEL_CONTROLLER_H
#define LEVEL_CONTROLLER_H
#include <raylib.h>

#include "globals.h"
#include "level.h"

class LevelController {
public:
    [[nodiscard]] std::vector<level> get_levels() const {
        return levels;
    }

     level &get_current_level(){
        return current_level;
    }

    [[nodiscard]] char *get_current_level_data() const {
        return current_level_data;
    }

    static LevelController &get_instance_level() {
        static LevelController instance;
        return instance;
    }

    LevelController(const LevelController&) = delete;
    LevelController operator=(const LevelController&) = delete;
    LevelController(LevelController&&) = delete;
    LevelController operator=(LevelController&&) = delete;

    bool is_inside_level(int row, int column);
    bool is_colliding(Vector2 pos, char look_for);
    char& get_collider(Vector2 pos, char look_for);
    static void reset_level_index();

    void load_level(int offset = 0);

    static void unload_level();

    void set_level_cell(size_t row,  size_t column, char chr);
    char& get_level_cell(size_t row, size_t column);

    void set_current_level(const level &current_level);

    level FileReadingFunc(const std::string& rleData);
    std::vector <level> TakeLevelsFromFile(const std::string& filename);

private:
    LevelController()  = default;
    ~LevelController() = default;
    level current_level;
    char* current_level_data{};
    std::vector <level> levels;
};

#endif //LEVEL_CONTROLLER_H
