/* Copyright 2020 Alexander Tulloh
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "quantum.h"

#ifndef ALTERNATIVE_LAYOUT_MODE
#    define ALTERNATIVE_LAYOUT_MODE true
#endif
#ifndef KANA_MODE
#    define KANA_MODE false
#endif
#ifndef KANA_TOGG
#    define KANA_TOGG true
#endif
#ifndef BUFFER_SIZE
#    define BUFFER_SIZE 3
#endif
#ifndef INTERVAL
#    define INTERVAL 10
#endif
#ifndef COMBINATION_MAX_TIME
#    define COMBINATION_MAX_TIME 50
#endif

enum {
    KN_TOG = QK_USER_0, // 0x7E40
    KN_MOD, // 0x7E41
};
// CW_TOG = 0x7c73

void set_alternative_mode(bool mode);
bool is_alternative_mode(void);
void alternative_mode_toggle(void);
void set_kana_mode(bool mode);
bool is_kana_mode(void);
void kana_mode_toggle(void);
bool process_english_kana_layout(uint16_t keycode, keyrecord_t *record);
void refresh_rgb_matrix_for_mode(void);