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
 #include <math.h>
 #include "wait.h"
 #include "debug.h"
 #include <stdio.h>
 #include "alternative_layout.h"

// prototype declaration
void nofunc(void);
void func_sentence_in(void (*func)(void));
void func_kagikakko(void);
void func_marukakko(void);
void func_sumikakko(void);
void func_kagikakko_in(void);
void func_marukakko_in(void);
void func_sumikakko_in(void);
void func_kagikakko_paste(void);
void func_marukakko_paste(void);
void func_sumikakko_paste(void);
void func_esc(void);

// QWERTY convert Colemak-DHk
static const uint16_t qwerty_to_colemakdhk[][2] = {
    // Top row
    {KC_E, KC_F},
    {KC_R, KC_P},
    {KC_T, KC_B},
    {KC_Y, KC_J},
    {KC_U, KC_L},
    {KC_I, KC_U},
    {KC_O, KC_Y},
    {KC_P, KC_SCLN},
    // Home row
    {KC_S, KC_R},
    {KC_D, KC_S},
    {KC_F, KC_T},
    {KC_H, KC_K},
    {KC_J, KC_N},
    {KC_K, KC_E},
    {KC_L, KC_I},
    {KC_SCLN, KC_O},
    // Bottom row
    {KC_V, KC_D},
    {KC_B, KC_V},
    {KC_N, KC_M},
    {KC_M, KC_H},
};

typedef struct {
    uint16_t key;
    const char *output;
} KanaTandaEntry;

static const KanaTandaEntry kana_tanda_table[] = {
    {KC_Q, "xyu"},
    {KC_W, "ko"},
    {KC_E, "to"},
    {KC_R, "sa"},
    {KC_T, "xya"},
    {KC_Y, "wa"},
    {KC_U, "ki"},
    {KC_I, "shi"},
    {KC_O, "ku"},
    {KC_P, "chi"},
    {KC_A, "ta"},
    {KC_S, "ka"},
    {KC_D, ","},
    {KC_F, "te"},
    {KC_G, "ha"},
    {KC_H, "no"},
    {KC_J, "i"},
    {KC_K, "."},
    {KC_L, "u"},
    {KC_SCLN, "nn"},
    {KC_Z, "xyo"},
    {KC_X, "ni"},
    {KC_C, "na"},
    {KC_V, "ru"},
    {KC_B, "mo"},
    {KC_N, "tsu"},
    {KC_M, "su"},
    {KC_COMM, "o"},
    {KC_DOT, "a"},
    {KC_SLSH, "xtsu"},
    {LSFT(KC_INT1), "func"},
};

typedef struct {
    uint16_t first;
    uint16_t second;
    const char *output;
    void (*func)(void);
} KanaChikanEntry;

static const KanaChikanEntry kana_chikan_table[] = {
    {KC_SCLN, KC_Q, "myu", nofunc},
    {KC_SCLN, KC_T, "mya", nofunc},
    {KC_SCLN, KC_Z, "myo", nofunc},
    {KC_SLSH, KC_Q, "xyu", nofunc},
    {KC_SLSH, KC_T, "xya", nofunc},
    {KC_SLSH, KC_Z, "xyo", nofunc},
    {KC_B, KC_Q, "pyu", nofunc},
    {KC_B, KC_T, "pya", nofunc},
    {KC_B, KC_Z, "pyo", nofunc},
    {KC_D, KC_COMM, "e", nofunc},
    {KC_D, KC_DOT, "mi", nofunc},
    {KC_D, KC_SLSH, "fo", nofunc},
    {KC_D, KC_SCLN, "-", nofunc},
    {KC_D, KC_A, "xyou", nofunc},
    {KC_D, KC_B, "po", nofunc},
    {KC_D, KC_C, "ze", nofunc},
    {KC_D, KC_D, "/", nofunc},
    {KC_D, KC_E, "nu", nofunc},
    {KC_D, KC_F, "desu", nofunc},
    {KC_D, KC_G, "pa", nofunc},
    {KC_D, KC_H, "ba", nofunc},
    {KC_D, KC_I, "ji", nofunc},
    {KC_D, KC_J, "ro", nofunc},
    {KC_D, KC_K, "wo", nofunc},
    {KC_D, KC_L, "yo", nofunc},
    {KC_D, KC_M, "zu", nofunc},
    {KC_D, KC_N, "bo", nofunc},
    {KC_D, KC_O, "ya", nofunc},
    {KC_D, KC_P, "fa", nofunc},
    {KC_D, KC_Q, "pi", nofunc},
    {KC_D, KC_R, "za", nofunc},
    {KC_D, KC_S, "ra", nofunc},
    {KC_D, KC_T, "pu", nofunc},
    {KC_D, KC_U, "yu", nofunc},
    {KC_D, KC_V, "ge", nofunc},
    {KC_D, KC_W, "go", nofunc},
    {KC_D, KC_X, "zo", nofunc},
    {KC_D, KC_Y, "mu", nofunc},
    {KC_D, KC_Z, "pe", nofunc},
    {KC_COMM, KC_Q, "thi", nofunc},
    {KC_COMM, KC_T, "tolu", nofunc},
    {KC_COMM, KC_Z, "sye", nofunc},
    {KC_F, KC_Q, "hyu", nofunc},
    {KC_F, KC_T, "hya", nofunc},
    {KC_F, KC_Z, "hyo", nofunc},
    {KC_G, KC_Q, "fi", nofunc},
    {KC_G, KC_T, "che", nofunc},
    {KC_G, KC_Z, "fe", nofunc},
    {KC_I, KC_Q, "shu", nofunc},
    {KC_I, KC_T, "sha", nofunc},
    {KC_I, KC_Z, "sho", nofunc},
    {KC_J, KC_Q, "chu", nofunc},
    {KC_J, KC_T, "cha", nofunc},
    {KC_J, KC_Z, "cho", nofunc},
    {KC_K, KC_COMM, "xu", nofunc},
    {KC_K, KC_DOT, "xe", nofunc},
    {KC_K, KC_SLSH, "xo", nofunc},
    {KC_K, KC_SCLN, "xyuu", nofunc},
    {KC_K, KC_A, "da", nofunc},
    {KC_K, KC_B, "ho", nofunc},
    {KC_K, KC_C, "se", nofunc},
    {KC_K, KC_D, "wo", nofunc},
    {KC_K, KC_E, "do", nofunc},
    {KC_K, KC_F, "de", nofunc},
    {KC_K, KC_G, "ma", nofunc},
    {KC_K, KC_H, "xa", nofunc},
    {KC_K, KC_I, "xi", nofunc},
    {KC_K, KC_J, "masu", nofunc},
    {KC_K, KC_K, "...", nofunc},
    {KC_K, KC_L, "re", nofunc},
    {KC_K, KC_M, "ri", nofunc},
    {KC_K, KC_N, "du", nofunc},
    {KC_K, KC_O, "gu", nofunc},
    {KC_K, KC_P, "di", nofunc},
    {KC_K, KC_Q, "hi", nofunc},
    {KC_K, KC_R, "me", nofunc},
    {KC_K, KC_S, "ga", nofunc},
    {KC_K, KC_T, "fu", nofunc},
    {KC_K, KC_U, "gi", nofunc},
    {KC_K, KC_V, "ke", nofunc},
    {KC_K, KC_W, "ne", nofunc},
    {KC_K, KC_X, "so", nofunc},
    {KC_K, KC_Y, "vu", nofunc},
    {KC_K, KC_Z, "he", nofunc},
    {KC_L, KC_Q, "bi", nofunc},
    {KC_L, KC_T, "bu", nofunc},
    {KC_L, KC_Z, "be", nofunc},
    {KC_M, KC_Q, "ryu", nofunc},
    {KC_M, KC_T, "rya", nofunc},
    {KC_M, KC_Z, "ryo", nofunc},
    {KC_O, KC_Q, "ju", nofunc},
    {KC_O, KC_T, "ja", nofunc},
    {KC_O, KC_Z, "jo", nofunc},
    {KC_P, KC_Q, "dyu", nofunc},
    {KC_P, KC_T, "dya", nofunc},
    {KC_P, KC_Z, "dyo", nofunc},
    {KC_Y, KC_Q, "gyu", nofunc},
    {KC_Y, KC_T, "gya", nofunc},
    {KC_Y, KC_Z, "gyo", nofunc},
    {KC_DOT, KC_Q, "dhi", nofunc},
    {KC_DOT, KC_T, "dwu", nofunc},
    {KC_DOT, KC_Z, "je", nofunc},
    {KC_U, KC_Q, "kyu", nofunc},
    {KC_U, KC_T, "kya", nofunc},
    {KC_U, KC_Z, "kyo", nofunc},
    {KC_V, KC_Q, "byu", nofunc},
    {KC_V, KC_T, "bya", nofunc},
    {KC_V, KC_Z, "byo", nofunc},
    {KC_H, KC_Q, "nyu", nofunc},
    {KC_H, KC_T, "nya", nofunc},
    {KC_H, KC_Z, "nyo", nofunc},
    {KC_E, KC_T, "kudasai", nofunc},
    {LSFT(KC_INT1), LSFT(KC_INT1), ";", nofunc},
    {LSFT(KC_INT1), KC_Q, "func", func_esc},
    {LSFT(KC_INT1), KC_C, "osewaninatteorimasu"},
    {LSFT(KC_INT1), KC_V, "moushiwakearimasenn", nofunc},
    {LSFT(KC_INT1), KC_B, "mondaiarimasenn", nofunc},
    {LSFT(KC_INT1), KC_N, "otsukaresamadesu", nofunc},
    {LSFT(KC_INT1), KC_Y, "func", func_kagikakko_in},
    {LSFT(KC_INT1), KC_U, "func", func_kagikakko},
    {LSFT(KC_INT1), KC_I, "func", func_kagikakko_paste},
    {LSFT(KC_INT1), KC_H, "func", func_marukakko_in},
    {LSFT(KC_INT1), KC_J, "func", func_marukakko},
    {LSFT(KC_INT1), KC_K, "func", func_marukakko_paste},
    {LSFT(KC_INT1), KC_M, "func", func_sumikakko},
    {LSFT(KC_INT1), KC_COMMA, "func", func_sumikakko_paste},
};

typedef struct {
    uint16_t first;
    uint16_t second;
    uint16_t third;
    const char *output;
} KanaKatsuyoEntry;

// The first and second keycodes in KanaKatsuyoEntry are not part of KanaChikanEntry.
static const KanaKatsuyoEntry kana_katsuyo_table[] = {
    {KC_D, KC_F, KC_M, "deari"},
    {KC_D, KC_F, KC_V, "dearu"},
    {KC_D, KC_F, KC_L, "deshou"},
    {KC_D, KC_F, KC_F, "deshite"},
    {KC_D, KC_F, KC_A, "deshita"},
    {KC_D, KC_F, KC_SCLN, "naidesu"},
    {KC_K, KC_J, KC_L, "mashou"},
    {KC_K, KC_J, KC_F, "mashite"},
    {KC_K, KC_J, KC_A, "mashita"},
    {KC_K, KC_J, KC_SCLN, "masenn"},
    {KC_D, KC_K, KC_A, "atta"},
    {KC_D, KC_K, KC_L, "arou"},
    {KC_D, KC_K, KC_SCLN, "hanai"},
    {KC_SCLN, KC_D, KC_A, "myou"},
    {KC_B, KC_D, KC_A, "pyou"},
    {KC_F, KC_D, KC_A, "hyou"},
    {KC_I, KC_D, KC_A, "shou"},
    {KC_J, KC_D, KC_A, "chou"},
    {KC_M, KC_D, KC_A, "ryou"},
    {KC_O, KC_D, KC_A, "jou"},
    {KC_P, KC_D, KC_A, "dyou"},
    {KC_Y, KC_D, KC_A, "gyou"},
    {KC_U, KC_D, KC_A, "kyou"},
    {KC_V, KC_D, KC_A, "byou"},
    {KC_H, KC_D, KC_A, "nyou"},
    {KC_A, KC_D, KC_A, "datta"},
    {KC_S, KC_D, KC_A, "katta"},
    {KC_C, KC_D, KC_A, "natta"},
    {KC_SCLN, KC_K, KC_SCLN, "myuu"},
    {KC_B, KC_K, KC_SCLN, "pyuu"},
    {KC_F, KC_K, KC_SCLN, "hyuu"},
    {KC_I, KC_K, KC_SCLN, "shuu"},
    {KC_J, KC_K, KC_SCLN, "chuu"},
    {KC_M, KC_K, KC_SCLN, "ryuu"},
    {KC_O, KC_K, KC_SCLN, "juu"},
    {KC_P, KC_K, KC_SCLN, "dyuu"},
    {KC_Y, KC_K, KC_SCLN, "gyuu"},
    {KC_U, KC_K, KC_SCLN, "kyuu"},
    {KC_V, KC_K, KC_SCLN, "byuu"},
    {KC_H, KC_K, KC_SCLN, "nyuu"},
    {KC_COMM, KC_D, KC_A, "onegai"},
    {KC_DOT, KC_D, KC_A, "arigatou"},
    {KC_SLSH, KC_D, KC_A, "itada"},
    {KC_L, KC_D, KC_A, "yoroshiku"},
    {KC_COMM, KC_K, KC_J, "emasu"},
    {KC_DOT, KC_K, KC_J, "mimasu"},
    {KC_C, KC_K, KC_J, "semasu"},
    {KC_M, KC_K, KC_J, "rimasu"},
    {KC_R, KC_K, KC_J, "memasu"},
    {KC_V, KC_K, KC_J, "kemasu"},
    {KC_W, KC_K, KC_J, "nemasu"},
    {KC_T, KC_K, KC_J, "omoimasu"},
    {KC_Q, KC_K, KC_J, "gozaimasu"},
    {KC_Z, KC_K, KC_J, "itashimasu"},
};

typedef struct {
    uint16_t key1;
    uint16_t key2;
    uint16_t max_time;
} limmited_combo_t;

static const limmited_combo_t limmited_combos[] = {
    {KC_S, KC_D, COMBINATION_MAX_TIME / 2},
    {KC_F, KC_D, COMBINATION_MAX_TIME / 2},
    {KC_J, KC_K, COMBINATION_MAX_TIME / 2},
    {KC_L, KC_K, COMBINATION_MAX_TIME / 2},
    // 他のコンボは COMBINATION_MAX_TIME で許容
};

typedef enum {
    ALPHA_KEYS,   // a-z 1-0 ,./;
    SYMBOL_KEYS, // SPC-SLASH
    OTHER_KEYS    // それ以外（エンター・スペースなど）
} key_category_t;

typedef enum {
    MOD_NONE,     // No modifiers are pressed
    MOD_CONTROL,     // LCTL is pressed including other modifiers
    MOD_OTHER     // Other modifiers are pressed excluding LCTL
} mod_status_t;

typedef enum {
    MODE_Q,  // QWERTY mode
    MODE_C,  // Colemak-DHk mode (alternative layout)
    MODE_K   // Kana mode
} mode_state_t;

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


#define COLEMAK_SIZE (sizeof(qwerty_to_colemakdhk) / sizeof(qwerty_to_colemakdhk[0]))
#define TANDA_SIZE (sizeof(kana_tanda_table) / sizeof(KanaTandaEntry))
#define CHIKAN_SIZE (sizeof(kana_chikan_table) / sizeof(KanaChikanEntry))
#define KATSUYO_SIZE (sizeof(kana_katsuyo_table) / sizeof(KanaKatsuyoEntry))
#define LIMMITED_COMBOS_SIZE (sizeof(limmited_combos) / sizeof((limmited_combos)[0]))


// Global variables
static bool alternative_mode = ALTERNATIVE_LAYOUT_MODE;
static bool kana_mode = KANA_MODE;
static bool kana_toggle = KANA_TOGG;
static uint8_t shift_target_key_count = 0;
static bool eisu_mode_active = false;
static uint16_t key_buffer[BUFFER_SIZE] = {KC_NO, KC_NO, KC_NO};
static bool is_katsuyo = false;

// Function definitions used other files
void set_alternative_mode(bool mode) {alternative_mode = mode;}
bool is_alternative_mode(void) {return alternative_mode;}
void alternative_mode_toggle(void) {alternative_mode ^= 1;}
void set_kana_mode(bool mode) {kana_mode = mode;}
bool is_kana_mode(void) {return kana_mode;}
void kana_mode_toggle(void) {kana_mode ^= 1;}
void kana_qwerty_swich(void) {kana_toggle ^= 1;}

// Function definitions: alternative layout, kana layout, Caps Word, 
uint8_t extract_mods(uint16_t keycode) {
    if (IS_QK_MOD_TAP(keycode)) {
        return QK_MOD_TAP_GET_MODS(keycode);
    }
    return 0;
}

uint8_t extract_layer(uint16_t keycode) {
    if (IS_QK_LAYER_TAP(keycode)) {
        return QK_LAYER_TAP_GET_LAYER(keycode);
    }
    return 0xFF;
}

uint16_t extract_tap_keycode(uint16_t keycode) {
    if (IS_QK_MOD_TAP(keycode)) {
        return QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
    } else if (IS_QK_LAYER_TAP(keycode)) {
        return QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    }
    return keycode;
}

uint8_t get_active_mods(uint16_t keycode, keyrecord_t *record) {
    uint8_t active_mods = get_mods() | get_oneshot_mods();
    if (IS_QK_MOD_TAP(keycode) && (record->tap.count == 0 || record->event.pressed)) {
        active_mods |= QK_MOD_TAP_GET_MODS(keycode);
    }

    return active_mods;
}

bool is_now_mod_active(uint8_t mod_mask) {
    return (get_mods() | get_oneshot_mods()) & mod_mask;
}

bool is_mod_active(uint16_t keycode, keyrecord_t *record, uint8_t mod_mask) {
    return get_active_mods(keycode, record) & mod_mask;
}

key_category_t get_key_category(uint16_t keycode) {
    if (IS_BASIC_KEYCODE(keycode)) {
        if ((KC_A <= keycode && keycode <= KC_0) || 
            keycode == KC_SEMICOLON || keycode == KC_COMMA ||
            keycode == KC_DOT || keycode == KC_SLASH) {
            return ALPHA_KEYS;
        }
        if ((KC_SPC <= keycode && keycode <= KC_SLASH) ||
            keycode == KC_NUBS || keycode == KC_INT1 || keycode == KC_INT3) {
            return SYMBOL_KEYS;
        }
    } else if (keycode == TD(2) || keycode == TD(6) ||
               keycode == TD(7) || keycode == TD(8) ||
               keycode == TD(9) || keycode == LSFT(KC_INT1)) {
        return SYMBOL_KEYS;
    }

    return OTHER_KEYS;
}

mod_status_t get_mod_status(void) {
    static mod_status_t last_mod_status = MOD_NONE;
    mod_status_t after_mod_status;
    uint8_t active_mods = get_mods() | get_oneshot_mods();  // 通常のモディファイア + ワンショットモディファイア
    bool has_lctl = is_now_mod_active(MOD_MASK_CTRL); // Check if LCTL is pressed
    if (active_mods == 0 && !eisu_mode_active) {
        after_mod_status = MOD_NONE;  // 何も押されていない
    } else {
        after_mod_status = has_lctl ? MOD_CONTROL : MOD_OTHER; // LCTL以外のモディファイアのみ押されている
    }

    if ((last_mod_status == MOD_CONTROL || after_mod_status == MOD_NONE) &&
        last_mod_status != after_mod_status) {
        clear_keys();
    }
    last_mod_status = after_mod_status;
    return after_mod_status;
}

mode_state_t get_mode_state(void) {
    mod_status_t mod_status = get_mod_status();
    int lyr = get_highest_layer(layer_state);

    if (mod_status == MOD_CONTROL || lyr != 0) {
        return MODE_Q;
    } else if (mod_status == MOD_NONE &&
                alternative_mode &&
                kana_mode) {
        return MODE_K;
    } else {
        return alternative_mode ? MODE_C : MODE_Q;
    }
}

uint16_t replace_keycode(uint16_t keycode) {
    for (size_t i = 0; i < COLEMAK_SIZE; i++) {
        if (keycode == qwerty_to_colemakdhk[i][0]) {
            return qwerty_to_colemakdhk[i][1];
        }
    }
    return keycode;
}

bool is_target_key(uint16_t keycode) {
    // Check if the keycode is a valid target key
    return (get_key_category(keycode) != OTHER_KEYS);
}


void reset_eisu_target_key_count(void) {
    // Reset the shift target key count and deactivate shift
    shift_target_key_count = 0;
    eisu_mode_active = false;
}

void check_eisu_mode(uint16_t keycode, uint8_t mod_mask, key_category_t key_category, keyrecord_t *record) {
    if (record->event.pressed) {
        if (is_now_mod_active(MOD_MASK_SHIFT)) {
            eisu_mode_active = true;
        }
        if (!eisu_mode_active || keycode == KC_LEFT || keycode == KC_RIGHT ||
            (IS_QK_ONE_SHOT_MOD(keycode) && record->tap.count > 0)) {
            return;
        }
        keycode = replace_keycode(keycode);
        if (shift_target_key_count == 0 && (keycode < KC_A || KC_Z < keycode)) {
            reset_eisu_target_key_count();
            return;
        } else if ((key_category != OTHER_KEYS) && (mod_mask == 0 || record->tap.count > 0)) {
            shift_target_key_count++; // Increment for target keys
            return;
        } else if ((keycode == KC_BSPC) && shift_target_key_count > 1) { // Handle Backspace
            shift_target_key_count--; // Decrement count
            return;
        }
        reset_eisu_target_key_count(); // Reset for other keys
    }
}

void reset_key_buffer(void) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        key_buffer[i] = KC_NO;
    }
}

void update_key_buffer(uint16_t keycode) {
    key_buffer[0] = key_buffer[1];
    key_buffer[1] = key_buffer[2];
    key_buffer[2] = keycode;
}

bool set_katsuyo_buffer(const char* str) {
    // Update key buffer based on specific substrings
    // Usable combinations are only those in KanaChikanEntry and must include KC_K and KC_J
    if (strstr(str, "masu")) {
        key_buffer[1] = KC_K;
        key_buffer[2] = KC_J;
        return true;
    } else if (strstr(str, "desu")) {
        key_buffer[1] = KC_D;
        key_buffer[2] = KC_F;
        return true;
    } else if (strstr(str, "dear")) {
        key_buffer[1] = KC_D;
        key_buffer[2] = KC_K;
        return true;
    } else {
        reset_key_buffer();
        return false;
    }
}

void reset_katsuyo_state(void) {
    key_buffer[1] = KC_NO;
    is_katsuyo = false;
}

bool check_string_func(int bspc_num, bool katsuyo_check, const char *str, void (*func)(void)) {
    if (bspc_num > 0) {
        for (size_t i = 0; i < bspc_num; i++) {
            send_string_P(SS_TAP(X_BSPC) SS_DELAY(INTERVAL));
        }
    }
    if (katsuyo_check) {
        is_katsuyo = set_katsuyo_buffer(str);
    }
    if (strstr(str, "func") != NULL) {
        func();
        return false;
    } else {
        send_string_with_delay(str, INTERVAL);
        return false;
    }
}

bool process_katsuyo(void) {
    bool is_first_key_valid = (key_buffer[0] == KC_D || key_buffer[0] == KC_K);
    bool is_second_key_valid = (key_buffer[1] == KC_D || key_buffer[1] == KC_K);

    if (is_first_key_valid || is_second_key_valid) {
        for (size_t i = 0; i < KATSUYO_SIZE; i++) {
            if (key_buffer[0] == kana_katsuyo_table[i].first &&
                key_buffer[1] == kana_katsuyo_table[i].second &&
                key_buffer[2] == kana_katsuyo_table[i].third) {
                if (!check_string_func(2, true, kana_katsuyo_table[i].output, nofunc)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool process_chikan_table(void) {
    for (size_t i = 0; i < CHIKAN_SIZE; i++) {
        if (key_buffer[1] == kana_chikan_table[i].first &&
            key_buffer[2] == kana_chikan_table[i].second) {
            int bspc_num = (key_buffer[1] == LSFT(KC_INT1)) ? 0 : 1;
            if (!check_string_func(bspc_num, true, kana_chikan_table[i].output, kana_chikan_table[i].func)) {
                return true;
            }
        }
    }
    return false;
}

bool process_tanda_table(uint16_t *pressed_time) {
    for (size_t i = 0; i < TANDA_SIZE; i++) {
        if (key_buffer[2] == kana_tanda_table[i].key) {
            if (!check_string_func(0, false, kana_tanda_table[i].output, nofunc)) {
                *pressed_time = timer_read();
                return true;
            }
        }
    }
    return false;
}

bool is_limmited_combos(uint16_t k1, uint16_t k2, uint16_t elapsed_time) {
    for (uint8_t i = 0; i < LIMMITED_COMBOS_SIZE ; i++) {
        if (limmited_combos[i].key1 == k1 &&
            limmited_combos[i].key2 == k2 &&
            elapsed_time > limmited_combos[i].max_time
        ) {
            return false;
        }
    }
    return true;
}

void handle_key_swap(uint16_t *pressed_time, uint8_t *pressed_count) {
    bool is_swap_candidate =
        (key_buffer[1] == KC_Q || key_buffer[1] == KC_T || key_buffer[1] == KC_Z ||
         key_buffer[2] == KC_D || key_buffer[2] == KC_K);
    uint16_t elapsed_time = timer_elapsed(*pressed_time);

    if (elapsed_time < COMBINATION_MAX_TIME && *pressed_count >= 2) {
        if (is_swap_candidate && is_limmited_combos(key_buffer[1], key_buffer[2], elapsed_time)) {
            uint16_t temp_key_buffer = key_buffer[1];
            key_buffer[1] = key_buffer[2];
            key_buffer[2] = temp_key_buffer;
        }
    }
}

bool process_kana(uint16_t keycode, keyrecord_t *record) {
    static uint16_t pressed_time = 0;
    static uint8_t pressed_count = 0;

    if (record->event.pressed) {
        update_key_buffer(keycode);
        pressed_count++;
        handle_key_swap(&pressed_time, &pressed_count);

        if (key_buffer[0] != KC_NO && key_buffer[1] != KC_NO) {
            if (process_katsuyo()) {return false;}
        }

        if (is_katsuyo) {reset_katsuyo_state();}

        if (key_buffer[1] != KC_NO) {
            if (process_chikan_table()) {return false;}
        }

        if (key_buffer[2] != KC_NO) {
            if (process_tanda_table(&pressed_time)) {return false;}
        }
        return true;
    } else {
        if (pressed_count > 0) {
            pressed_count--;
        }
        return true;
    }
}

bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        // Keycodes that continue Caps Word, with shift applied.
        case KC_A ... KC_Z:
            add_weak_mods(MOD_BIT(KC_LSFT));  // Apply shift to next key.
            return true;

        // Keycodes that continue Caps Word, without shifting.
        case KC_1 ... KC_0:
        case KC_BSPC:
        case KC_DEL:
        case KC_MINS:
        case KC_SCLN:
        case KC_UNDS:
        case LSFT(KC_INT1):
        case TD(2):
            return true;

        default:
            return false;  // Deactivate Caps Word.
    }
}

void process_caps_word_and_register(uint16_t keycode, bool pressed) {
    if (pressed) {
        if (is_caps_word_on()) {
            caps_word_press_user(keycode);
        }
        register_code16(keycode);
    } else {
        unregister_code16(keycode);
    }
}

void process_mod_tap(uint8_t mod_mask, bool pressed) {
    if (pressed) {
        register_mods(mod_mask);
    } else {
        unregister_mods(mod_mask);
    }
}

void process_tap_keycode(uint16_t keycode, uint8_t mod_mask, keyrecord_t *record) {
    if (mod_mask == 0 || record->tap.count > 0) { 
        process_caps_word_and_register(keycode, record->event.pressed);
    } else {
        process_mod_tap(mod_mask, record->event.pressed);
    }
}

bool process_alternative_mode(mode_state_t mode_state, uint16_t keycode, uint8_t mod_mask, keyrecord_t *record) {
    if (mode_state == MODE_K && (mod_mask == 0 || record->tap.count > 0)) {
        if (!process_kana(keycode, record)) {return false;}
    } else {
        process_tap_keycode(replace_keycode(keycode), mod_mask, record);
        return false;
    }
    return true;
}

bool should_skip_processing(mode_state_t mode_state) {
    return mode_state == MODE_Q || (mode_state == MODE_K && kana_toggle == false);
}

bool mods_layer(uint16_t keycode, keyrecord_t *record) {
    static uint8_t active_oneshot_mods = 0;
    
    if ((IS_QK_LAYER_TAP(keycode) || IS_QK_MOMENTARY(keycode)) && record->tap.count == 0) {
        if (record->event.pressed) {
            active_oneshot_mods |= get_oneshot_mods();
            clear_oneshot_mods();
            if (active_oneshot_mods) {
                add_mods(active_oneshot_mods);
            }
        } else {
            if (active_oneshot_mods) {
                del_mods(active_oneshot_mods);
            }
            active_oneshot_mods = 0;
        }
        return true;
    }
    if (get_mods() == MOD_BIT(KC_LALT) && KC_1 <= keycode && keycode <= KC_0) {
        uint16_t num_keycode = keycode + KC_P1 - KC_1;
        if (record->event.pressed) {
            register_code16(num_keycode);
        } else {
            unregister_code16(num_keycode);
        }
        return false;
    }
    return true;
}

bool entry_mode_toggle(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case KC_LNG1:
            case TD(3):
                reset_eisu_target_key_count();
                set_kana_mode(true);
                reset_key_buffer();
                return true;
            
            case KC_LNG2:
            case TD(4):
                reset_eisu_target_key_count();
                set_kana_mode(false);
                return true;

            case KN_TOG:
                reset_eisu_target_key_count();
                kana_qwerty_swich();
                return true;

            case KN_MOD:
                reset_eisu_target_key_count();
                alternative_mode_toggle();
                return true;
       }
    }
    return false;
}

bool process_english_kana_layout(uint16_t keycode, keyrecord_t *record) {
    if (entry_mode_toggle(keycode, record)) {
        return true;
    }

    mode_state_t mode_state = get_mode_state();
    if (should_skip_processing(mode_state)) {
        return true;
    }

    uint16_t base_keycode = extract_tap_keycode(keycode); // include LSFT(code)
    uint8_t mod_mask = extract_mods(keycode);
    key_category_t key_category = get_key_category(base_keycode);
    check_eisu_mode(base_keycode, mod_mask, key_category, record);
    if (get_key_category(base_keycode) != ALPHA_KEYS && base_keycode != LSFT(KC_INT1)) {
        update_key_buffer(KC_NO);
        return true;
    }
    if (!process_alternative_mode(mode_state, base_keycode, mod_mask, record)) {
        return false;
    }

    return true;
}

void refresh_rgb_matrix_for_mode(void) {
    static uint16_t last_update = 0;
    if (timer_elapsed(last_update) < 100) {
        return;
    }
    
    last_update = timer_read(); 
    static mode_state_t last_mode = MODE_Q;
    mode_state_t current_mode = get_mode_state();
    if (current_mode != last_mode) {
        last_mode = current_mode;
        uint8_t random_hue = rand() % 256;
        rgb_matrix_sethsv_noeeprom(random_hue, 192, 192);
        if (!rgb_matrix_is_enabled()) {
            rgb_matrix_enable_noeeprom();
        }
        if (should_skip_processing(current_mode)) {
            rgb_matrix_mode_noeeprom(RGB_MATRIX_BREATHING);
            return;
        } else if (current_mode == MODE_C) {
            rgb_matrix_mode_noeeprom(RGB_MATRIX_GRADIENT_LEFT_RIGHT);
            return;
        } else if (current_mode == MODE_K) {
            rgb_matrix_mode_noeeprom(RGB_MATRIX_CYCLE_OUT_IN);
            return;
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if(!mods_layer(keycode, record)) {return false;}
    return process_english_kana_layout(keycode, record);
}

void matrix_scan_user(void) {
    refresh_rgb_matrix_for_mode();
}

void nofunc(void) {}

void func_common_in(const char *text) {
    send_string_with_delay(text, INTERVAL);
    tap_code(KC_LEFT);
}

void func_sentence_in(void (*func)(void)) {
    send_string_with_delay_P(SS_DOWN(X_LSFT) SS_TAP(X_HOME) SS_UP(X_LSFT) SS_LCTL("x") SS_DELAY(INTERVAL), INTERVAL);
    func();
    send_string_with_delay_P(SS_DELAY(100) SS_LCTL("v") SS_TAP(X_RIGHT), INTERVAL);
}

void func_copy_paste(void (*func)(void)) {
    send_string_with_delay_P(SS_LCTL("x") SS_DELAY(INTERVAL), INTERVAL);
    func();
    send_string_with_delay_P(SS_DELAY(100) SS_LCTL("v") SS_TAP(X_RIGHT), INTERVAL);
}

void func_marukakko(void) {
    func_common_in("marukakko \n");
}

void func_sumikakko(void) {
    func_common_in("sumikakko \n");
}

void func_kagikakko(void) {
    func_common_in("kagikakko \n");
}

void func_marukakko_in(void) {
    func_sentence_in(func_marukakko);
}

void func_sumikakko_in(void) {
    func_sentence_in(func_sumikakko);
}

void func_kagikakko_in(void) {
    func_sentence_in(func_kagikakko);
}

void func_marukakko_paste(void) {
    func_copy_paste(func_marukakko);
}

void func_sumikakko_paste(void) {
    func_copy_paste(func_sumikakko);
}

void func_kagikakko_paste(void) {
    func_copy_paste(func_kagikakko);
}

void func_esc(void) {
    send_string_with_delay_P(SS_TAP(X_ESC) SS_TAP(X_ESC) SS_TAP(X_ESC), INTERVAL);
}