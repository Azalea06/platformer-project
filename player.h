#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>


class player {
    public:

    static player &get_instance_player() {
        static player instance;
        return instance;
    }

    player(const player &) = delete;
    player &operator=(const player &) = delete;
    player(player &&) = delete;
    player &operator=(player &&) = delete;

    player()  = default;
    ~player() = default;
    bool is_player_on_ground;
    bool is_looking_forward;
    bool is_moving;


    [[nodiscard]] bool is_is_player_on_ground() const {
        return is_player_on_ground;
    }

    [[nodiscard]] bool is_is_looking_forward() const {
        return is_looking_forward;
    }

    [[nodiscard]] bool is_is_moving() const {
        return is_moving;
    }

    [[nodiscard]] int get_player_lives() const {
        return player_lives;
    }

    [[nodiscard]] int get_max_player_lives() const {
        return MAX_PLAYER_LIVES;
    }

    [[nodiscard]] float get_player_y_velocity() const {
        return player_y_velocity;
    }

    [[nodiscard]] Vector2 get_player_pos() const {
        return player_pos;
    }

    void set_is_player_on_ground(bool is_player_on_ground) {
        this->is_player_on_ground = is_player_on_ground;
    }

    void set_is_looking_forward(bool is_looking_forward) {
        this->is_looking_forward = is_looking_forward;
    }

    void set_is_moving(bool is_moving) {
        this->is_moving = is_moving;
    }

    void set_player_lives(int player_lives) {
        this->player_lives = player_lives;
    }

    void set_player_y_velocity(float player_y_velocity) {
        this->player_y_velocity = player_y_velocity;
    }

    void set_player_pos(const Vector2 &player_pos) {
        this->player_pos = player_pos;
    }

    void reset_player_stats();
    void increment_player_score();
    int get_total_player_score();

    void spawn_player();
    void kill_player();

    void move_player_horizontally(float delta);
    void update_player();
    void update_player_gravity();

private:

    int player_level_scores[LEVEL_COUNT];
    int player_lives = MAX_PLAYER_LIVES;
    const int MAX_PLAYER_LIVES = 3;
    float player_y_velocity = 0;
    Vector2 player_pos;

};


#endif //PLAYER_H
