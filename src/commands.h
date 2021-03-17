/** @file
 * Interfejs klasy odpowiadającej za tryb wsadowy
 * @author Grzegorz Gruza <gg417923@mimuw.edu.pl>
 * @copyright Grzegorz Gruza
 * @date 17.05.2020
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include "gamma.h"
#include "interactive.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

/** @brief Sprawdza, czy znak to cyfra lub biały znak
 * @param[in] letter       – znak do analizy
 * @return @p true jeśli znak to cyfra lub biały znak,
 * a @p false w przeciwnym razie.
 */
bool good_letter(char letter);

/** @brief Sprawdza, czy słowo składa się z samych białych znaków
 * @param[in] buffer       – tablica znaków, którą sprawdzamy
 * @param[in] index        – wypełniona część słowa
 * @return @p true jeśli na początku jest zeru, a @p false w przeciwnym razie.
 */
bool only_spaces(char *buffer, uint64_t index);

/** @brief Wywołuje gamma_new i w wypadku powodzenia przechodzi do interac mode.
 * @param[in] g          – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] buffer     – tablica znaków, w której szukamy argumentów,
 * @param[in] argv       – tablica, do wypełniona argumentami,
 * @param[in] line       – nr ostatniej linii.
 */
void try_command_I(gamma_t **g, char *buffer, uint32_t *argv, uint64_t line);

/** @brief Wywołuje gamma_new i w wypadku powodzenia przechodzi do batch mode.
 * @param[in] g          – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] buffer     – tablica znaków, w której szukamy argumentów,
 * @param[in] argv       – tablica, do wypełniona argumentami,
 * @param[in] line       – nr ostatniej linii,
 * @param[in] batch_mode – zmienna oznaczająca czy batch mode jest aktywny.
 */
void try_command_B(gamma_t **g, char *buffer, uint32_t *argv,
                   uint64_t line, bool *batch_mode);


/** @brief Wywołuje gamma_move.
 * @param[in] g          – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] buffer     – tablica znaków, w której szukamy argumentów,
 * @param[in] argv       – tablica, wypełniona argumentami,
 * @param[in] line       – nr ostatniej linii.
 */
void try_command_m(gamma_t *g, char *buffer, uint32_t *argv, uint64_t line);

/** @brief Wywołuje gamma_golden_move.
 * @param[in] g          – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] buffer     – tablica znaków, w której szukamy argumentów,
 * @param[in] argv       – tablica, wypełniona argumentami,
 * @param[in] line       – nr ostatniej linii.
 */
void try_command_g(gamma_t *g, char *buffer, uint32_t *argv, uint64_t line);

/** @brief Wywołuje gamma_busy_fields.
 * @param[in] g          – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] buffer     – tablica znaków, w której szukamy argumentów,
 * @param[in] argv       – tablica, wypełniona argumentami,
 * @param[in] line       – nr ostatniej linii.
 */
void try_command_b(gamma_t *g, char *buffer, uint32_t *argv, uint64_t line);

/** @brief Wywołuje gamma_free_fields.
 * @param[in] g          – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] buffer     – tablica znaków, w której szukamy argumentów,
 * @param[in] argv       – tablica, wypełniona argumentami,
 * @param[in] line       – nr ostatniej linii.
 */
void try_command_f(gamma_t *g, char *buffer, uint32_t *argv, uint64_t line);

/** @brief Wywołuje gamma_golden_possible.
 * @param[in] g          – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] buffer     – tablica znaków, w której szukamy argumentów,
 * @param[in] argv       – tablica, wypełniona argumentami,
 * @param[in] line       – nr ostatniej linii.
 */
void try_command_q(gamma_t *g, char *buffer, uint32_t *argv, uint64_t line);

/** @brief Wywołuje gamma_board i wypisuje zaalokowany bufor.
 * @param[in] g          – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] buffer     – tablica znaków, w której szukamy argumentów,
 * @param[in] argv       – tablica, wypełniona argumentami,
 * @param[in] line       – nr ostatniej linii.
 */
void try_command_p(gamma_t *g, char *buffer, uint32_t *argv, uint64_t line);

#endif /* COMMANDS_H */
