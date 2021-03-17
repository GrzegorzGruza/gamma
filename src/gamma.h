/** @file
 * Interfejs klasy przechowującej stan gry gamma
 *
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 18.03.2020
 */

#ifndef GAMMA_H
#define GAMMA_H

#define _POSIX_C_SOURCE 199309L

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/** @brief Reprezentacja pary
 */
struct pair {
    uint32_t st; ///< pierwszy element pary
    uint32_t nd; ///< drugi element pary
};
typedef struct pair pair_t;

/** @brief Struktura przechowująca stan gry.
 */
struct gamma {
    uint32_t width; ///< szerokość planszy, liczba dodatnia
    uint32_t height; ///< wysokosć planszy, liczba dodatnia
    uint32_t players; ///< liczba graczy, liczba dodatnia

    uint64_t *fields_next_to; ///< liczba pól bezpośrednio obok gracza
    uint64_t *busy_fields; ///< liczba pól zajętych przez gracza
    uint64_t busy_fields_all; ///< liczba pól zajętych przez wszystkich graczy

    uint32_t areas; ///< maksymalna liczba obszarów, jakie można zająć, l.dodatnia
    uint32_t *used_areas; ///< liczba obszarów, które zajmuje gracz

    uint32_t **board; ///< board[x][y]=g – na polu (x,y) stoi pionek gracza g
    bool *is_golden_used; ///< is_golden_used[g] – czy gracz g wykonał złoty ruch

    pair_t **parent; ///< struktura find and union
    uint32_t **rank; ///< struktura find and union


    uint64_t **low_visit_time; ///< czas odwiedzenia wierzchołka w trkacie LOW
    uint64_t **low; ///< wartości funkcji LOW dla grafów poszczególnych obszarów
};

typedef struct gamma gamma_t;

/** @brief Tworzy strukturę przechowującą stan gry.
 * Alokuje pamięć na nową strukturę przechowującą stan gry.
 * Inicjuje tę strukturę tak, aby reprezentowała początkowy stan gry.
 * @param[in] width   – szerokość planszy, liczba dodatnia,
 * @param[in] height  – wysokość planszy, liczba dodatnia,
 * @param[in] players – liczba graczy, liczba dodatnia,
 * @param[in] areas   – maksymalna liczba obszarów,
 *                      jakie może zająć jeden gracz.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 * zaalokować pamięci lub któryś z parametrów jest niepoprawny.
 */
gamma_t *gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas);

/** @brief Usuwa strukturę przechowującą stan gry.
 * Usuwa z pamięci strukturę wskazywaną przez @p g.
 * Nic nie robi, jeśli wskaźnik ten ma wartość NULL.
 * @param[in] g       – wskaźnik na usuwaną strukturę.
 */
void gamma_delete(gamma_t *g);

/** @brief Wykonuje ruch.
 * Ustawia pionek gracza @p player na polu (@p x, @p y).
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 * @return Wartość @p true, jeśli ruch został wykonany, a @p false,
 * gdy ruch jest nielegalny lub któryś z parametrów jest niepoprawny.
 */
bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Wykonuje złoty ruch.
 * Ustawia pionek gracza @p player na polu (@p x, @p y) zajętym przez innego
 * gracza, usuwając pionek innego gracza.
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 * @return Wartość @p true, jeśli ruch został wykonany, a @p false,
 * gdy gracz wykorzystał już swój złoty ruch, ruch jest nielegalny
 * lub któryś z parametrów jest niepoprawny.
 */
bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Podaje liczbę pól zajętych przez gracza.
 * Podaje liczbę pól zajętych przez gracza @p player.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new.
 * @return Liczba pól zajętych przez gracza lub zero,
 * jeśli któryś z parametrów jest niepoprawny.
 */
uint64_t gamma_busy_fields(gamma_t *g, uint32_t player);

/** @brief Podaje liczbę pól, jakie jeszcze gracz może zająć.
 * Podaje liczbę wolnych pól, na których w danym stanie gry gracz @p player może
 * postawić swój pionek w następnym ruchu.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new.
 * @return Liczba pól, jakie jeszcze może zająć gracz lub zero,
 * jeśli któryś z parametrów jest niepoprawny.
 */
uint64_t gamma_free_fields(gamma_t *g, uint32_t player);

/** @brief Sprawdza, czy gracz może wykonać złoty ruch.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new.
 * @return Wartość @p true, jeśli gracz jeszcze nie wykonał w tej rozgrywce
 * złotego ruchu i jest przynajmniej jedno pole zajęte przez innego gracza,
 * na które może wykonać ruch, a @p false w przeciwnym przypadku.
 */
bool gamma_golden_possible(gamma_t *g, uint32_t player);

/** @brief Daje napis opisujący stan planszy.
 * Alokuje w pamięci bufor, w którym umieszcza napis zawierający tekstowy
 * opis aktualnego stanu planszy. Przykład znajduje się w pliku gamma_test.c.
 * Funkcja wywołująca musi zwolnić ten bufor.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry.
 * @return Wskaźnik na zaalokowany bufor zawierający napis opisujący stan
 * planszy lub NULL, jeśli nie udało się zaalokować pamięci.
 */
char *gamma_board(gamma_t *g);

/** @brief Dodaje znak @p l na koniec bufora @p board.
 * Sprawdza, czy bufor nie uległ przepełnieniu.
 * Jeśli tak, to zwiększa jego rozmiar dwukrotnie
 * @param[in] index     – liczba, wskazująca jaki prefiks bufora wypełniono,
 * @param[in] size      – rozmiar bufora,
 * @param[in] board     – wskaźnik na bufor,
 * @param[in] l         – znak dodawany na koniec bufora.
 * @return Wartość @p true jeśli operacja przebiegła poprawnie,
 * a @p false, jeśli nie.
 */
bool add_letter(uint64_t *index, uint64_t *size, char **board, char l);

#endif /* GAMMA_H */
