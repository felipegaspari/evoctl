#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include "settings.h"

void save_settings(const float volumes[NUM_OUTPUTS][NUM_INPUTS]);
void load_settings(float volumes[NUM_OUTPUTS][NUM_INPUTS]);

#endif // SETTINGS_MANAGER_H 