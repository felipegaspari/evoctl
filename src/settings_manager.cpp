#include "settings_manager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sys/stat.h>
#include <wordexp.h>
#include <iostream>

using json = nlohmann::json;

static std::string get_config_path() {
    wordexp_t p;
    wordexp("~/.config/evoctl", &p, 0);
    std::string path = p.we_wordv[0];
    wordfree(&p);
    return path;
}

static std::string get_config_file_path() {
    return get_config_path() + "/settings.json";
}

void save_settings(const float volumes[NUM_OUTPUTS][NUM_INPUTS]) {
    std::string dir_path = get_config_path();
    mkdir(dir_path.c_str(), 0755);

    json j;
    for (int i = 0; i < NUM_OUTPUTS; ++i) {
        for (int k = 0; k < NUM_INPUTS; ++k) {
            j["volumes"][i][k] = volumes[i][k];
        }
    }
    std::ofstream o(get_config_file_path());
    o << std::setw(4) << j << std::endl;
}

void load_settings(float volumes[NUM_OUTPUTS][NUM_INPUTS]) {
    std::ifstream i(get_config_file_path());
    if (!i.is_open()) {
        for (int j = 0; j < NUM_OUTPUTS; ++j) {
            for (int k = 0; k < NUM_INPUTS; ++k) {
                volumes[j][k] = -100.0f;
            }
        }
        return;
    }

    json j;
    i >> j;

    for (int out = 0; out < NUM_OUTPUTS; ++out) {
        for (int in = 0; in < NUM_INPUTS; ++in) {
            volumes[out][in] = j["volumes"][out][in];
        }
    }
} 