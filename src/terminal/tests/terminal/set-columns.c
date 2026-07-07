/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "terminal/terminal-priv.h"

#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <string.h>

/**
 * Verifies that guac_terminal_set_columns() preserves all columns occupied by
 * a wide character when that character is written beneath the visible cursor.
 */
void test_terminal_set_columns__preserves_wide_character_under_cursor() {

    guac_terminal_char default_char = {
        .value = GUAC_CHAR_SPACE,
        .width = 1
    };

    guac_terminal_char wide_char = {
        .value = 0x4E2D,
        .width = 2
    };

    guac_terminal_display display;
    memset(&display, 0, sizeof(display));
    display.width = 4;
    display.height = 1;
    display.operations = calloc(display.width * display.height,
            sizeof(guac_terminal_operation));
    CU_ASSERT_PTR_NOT_NULL_FATAL(display.operations);

    guac_terminal_buffer* buffer =
        guac_terminal_buffer_alloc(1, &default_char);
    CU_ASSERT_PTR_NOT_NULL_FATAL(buffer);

    guac_terminal_buffer_set_columns(buffer, 0, 0, 1, &wide_char);

    guac_terminal_char* characters;
    int length = guac_terminal_buffer_get_columns(buffer, &characters, NULL, 0);

    CU_ASSERT_EQUAL(length, 2);
    CU_ASSERT_EQUAL(characters[0].value, wide_char.value);
    CU_ASSERT_EQUAL(characters[0].width, wide_char.width);
    CU_ASSERT_EQUAL(characters[1].value, GUAC_CHAR_CONTINUATION);

    guac_terminal_buffer_set_cursor(buffer, 0, 0, true);
    CU_ASSERT_EQUAL(characters[0].value, wide_char.value);
    CU_ASSERT_EQUAL(characters[0].width, wide_char.width);
    CU_ASSERT_TRUE(characters[0].attributes.cursor);
    CU_ASSERT_EQUAL(characters[1].value, GUAC_CHAR_CONTINUATION);

    guac_terminal_buffer_set_columns(buffer, 0, 0, 1, &default_char);

    guac_terminal terminal;
    memset(&terminal, 0, sizeof(terminal));
    terminal.display = &display;
    terminal.current_buffer = buffer;
    terminal.visible_cursor_row = -1;
    terminal.visible_cursor_col = -1;

    guac_terminal_set_columns(&terminal, 0, 0, 1, &wide_char);

    length = guac_terminal_buffer_get_columns(buffer, &characters, NULL, 0);

    CU_ASSERT_EQUAL(length, 2);
    CU_ASSERT_EQUAL(characters[0].value, wide_char.value);
    CU_ASSERT_EQUAL(characters[0].width, wide_char.width);
    CU_ASSERT_EQUAL(characters[1].value, GUAC_CHAR_CONTINUATION);

    guac_terminal_buffer_set_columns(buffer, 0, 0, 1, &default_char);

    terminal.visible_cursor_row = 0;
    terminal.visible_cursor_col = 0;

    guac_terminal_set_columns(&terminal, 0, 0, 1, &wide_char);

    length = guac_terminal_buffer_get_columns(buffer, &characters, NULL, 0);

    CU_ASSERT_EQUAL(length, 2);
    CU_ASSERT_EQUAL(characters[0].value, wide_char.value);
    CU_ASSERT_EQUAL(characters[0].width, wide_char.width);
    CU_ASSERT_TRUE(characters[0].attributes.cursor);
    CU_ASSERT_EQUAL(characters[1].value, GUAC_CHAR_CONTINUATION);

    guac_terminal_buffer_free(buffer);
    free(display.operations);

}
