#include "globals.h"
#include "level_controller.h"
#include "player.h"


void player::reset_player_stats() {
    player_lives = MAX_PLAYER_LIVES;
    for (int i = 0; i < LEVEL_COUNT; i++) {
        player_level_scores[i] = 0;
    }
}

void player::increment_player_score() {
    PlaySound(coin_sound);
    player_level_scores[level_index]++;
}

int player::get_total_player_score() {
    int sum = 0;

    for (int i = 0; i < LEVEL_COUNT; i++) {
        sum += player_level_scores[i];
    }

    return sum;
}

void player::spawn_player() {
    player_y_velocity = 0;

    for (size_t row = 0; row < LevelController::get_instance_level().get_current_level().get_rows(); ++row) {
        for (size_t column = 0; column < LevelController::get_instance_level().get_current_level().get_columns(); ++column) {
            char cell = LevelController::get_instance_level().get_level_cell(row, column);;

            if (cell == PLAYER) {
                player_pos.x = column;
                player_pos.y = row;
                LevelController::get_instance_level().set_level_cell(row, column, AIR);
                return;
            }
        }
    }
}

void player::kill_player() {
    // Decrement a life and reset all collected coins in the current level
    PlaySound(player_death_sound);
    game_state = DEATH_STATE;
    player_lives--;
    player_level_scores[level_index] = 0;
}

void player::move_player_horizontally(float delta) {
    // See if the player can move further without touching a wall;
    // otherwise, prevent them from getting into a wall by rounding their position
    float next_x = player_pos.x + delta;
    if (!LevelController::get_instance_level().is_colliding({next_x, player_pos.y}, WALL)) {
        player_pos.x = next_x;
    }
    else {
        player_pos.x = roundf(player_pos.x);
        return;
    }

    // For drawing player animations
    is_looking_forward = delta > 0;
    if (delta != 0) is_moving = true;
}

void player::update_player_gravity() {
    // Bounce downwards if approaching a ceiling with upwards velocity
    if (LevelController::get_instance_level().is_colliding({player_pos.x, player_pos.y - 0.1f}, WALL) && player_y_velocity < 0) {
        player_y_velocity = CEILING_BOUNCE_OFF;
    }

    // Add gravity to player's y-position
    player_pos.y += player_y_velocity;
    player_y_velocity += GRAVITY_FORCE;

    // If the player is on ground, zero player's y-velocity
    // If the player is *in* ground, pull them out by rounding their position
    is_player_on_ground = LevelController::get_instance_level().is_colliding({player_pos.x, player_pos.y + 0.1f}, WALL);
    if (is_player_on_ground) {
        player_y_velocity = 0;
        player_pos.y = roundf(player_pos.y);
    }
}

void player::update_player() {
    update_player_gravity();

    // Interacting with other level elements
    if (LevelController::get_instance_level().is_colliding(player_pos, COIN)) {
        LevelController::get_instance_level().get_collider(player_pos, COIN) = AIR; // Removes the coin
        increment_player_score();
    }

    if (LevelController::get_instance_level().is_colliding(player_pos, EXIT)) {
        // Reward player for being swift
        if (timer > 0) {
            // For every 9 seconds remaining, award the player 1 coin
            timer -= 25;
            time_to_coin_counter += 5;

            if (time_to_coin_counter / 60 > 1) {
                increment_player_score();
                time_to_coin_counter = 0;
            }
        }
        else {
            // Allow the player to exit after the level timer goes to zero
            LevelController::get_instance_level().load_level(1);
            PlaySound(exit_sound);
        }
    }
    else {
        // Decrement the level timer if not at an exit
        if (timer >= 0) timer--;
    }

    // Kill the player if they touch a spike or fall below the level
    if (LevelController::get_instance_level().is_colliding(player_pos, SPIKE) || player_pos.y > LevelController::get_instance_level().get_current_level().get_rows()) {
        kill_player();
    }

    // Upon colliding with an enemy...
    if (EnemiesController::get_instance().is_colliding_with_enemies(player_pos)) {
        // ...check if their velocity is downwards...
        if (player_y_velocity > 0) {
            // ...if yes, award the player and kill the enemy
            EnemiesController::get_instance().remove_colliding_enemy(player_pos);
            PlaySound(kill_enemy_sound);

            increment_player_score();
            player_y_velocity = -BOUNCE_OFF_ENEMY;
        }
        else {
            // ...if not, kill the player
            kill_player();
        }
    }
}
