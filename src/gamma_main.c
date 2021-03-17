/* @file
 *
 * @author Grzegorz Gruza <gg417923@mimuw.edu.pl>
 * @copyright Grzegorz Gruza
 * @date 17.04.2020
 */

#include "gamma.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include "commands.h"

/* @brief Funkcja wywoływana na koniec programu
 * Zwania pamięć i kończy program odpowiednim kodem (0 lub 1).
 * sprawdza czy ostatnia linia miała enter, zwalnia zaalokowaną pamięć.
 * @param[in] g        – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] argv     – tablica do zwolnienia,
 * @param[in] buffer   – tablica char do zwolnienia,
 * @param[in] enter    – czy ostatnia linia kończyła się enterem,
 * @param[in] line     – nr ostatniej linii,
 * @param[in] index    – indeks, do którego zapełniony jest buffor,
 * @param[in] succesed – czy program ma zakończyć się z kodem 0.
 */
void final(gamma_t *g, uint32_t *argv, char *buffer,
           bool enter, uint64_t line, uint64_t index, bool succesed) {
    if (!enter
        && argv != NULL
        && !only_spaces(buffer, index)) {
        fprintf(stderr, "ERROR %lu\n", line);
    }
    gamma_delete(g);
    if (argv != NULL) free(argv);
    if (buffer != NULL) free(buffer);

    if (succesed) exit(0);
    else exit(1);
}

/* @brief Sprawdza czy polecenie jest poprawnym poleceniem w batch mode.
 * Jeśli pierwsza litera jest jedną z możliwych pierwszych liter polecenia,
 * to próbuje wywołać to polecenie.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] buffer  – buffor, w którym znajduje się polecenie
 * @param[in] argv    – tablica, do której przekazywane są polecenia.
 * @param[in] line    – nr ostatniej linii.
 */
void batch(gamma_t *g, char *buffer, uint32_t *argv, uint64_t line) {
    if (buffer[0] == 'm') try_command_m(g, buffer, argv, line);
    else if (buffer[0] == 'g') try_command_g(g, buffer, argv, line);
    else if (buffer[0] == 'b') try_command_b(g, buffer, argv, line);
    else if (buffer[0] == 'f') try_command_f(g, buffer, argv, line);
    else if (buffer[0] == 'q') try_command_q(g, buffer, argv, line);
    else if (buffer[0] == 'p') try_command_p(g, buffer, argv, line);
    else fprintf(stderr, "ERROR %lu\n", line);
}

/* @brief Funkcja main tworzonego pliku wykonywalnego gamma
 * Wczytuje i interpretuje polecenia.
 * @return @p 0 jeśli program wykonał się poprawnie,
 * a @p 1 jeśli wystąpił krytyczny błąd.
 */
int main(void) {
    gamma_t *g = NULL; //gra gamma
    uint64_t line = 1; //nr lini
    uint64_t size = 0; //aktualny zaalokowany rozmiar bufora
    char *buffer = NULL; //bufor przetrzymujący obecne polecenie
    uint64_t index = 0; //rozmiar polecenia
    char letter; //wczytywana litera
    uint32_t *argv = malloc(4 * sizeof(uint32_t)); //tu przekażmy argumenty
    bool batch_mode = false; //czy barch mode jest włączony
    bool enter = false; //czy linia zakończona jest eneterem
    bool error = false; //czy podano literę będącą nie liczbą i nie białym znakiem

    if (argv == NULL) final(g, argv, buffer, enter, line, index, false);

    while (scanf("%c", &letter) == 1) {
        if (size == 0) {//pierwszy znak po końcu linii
            size = 1;
            if (buffer != NULL) free(buffer);
            buffer = calloc(1, size * sizeof(char));
            if (buffer == NULL) final(g, argv, buffer, enter, line, index, false);
            index = 0;
        }
        if (letter != '\n') {//ciągle ta sama linia
            if (index != 0 && !good_letter(letter)) error = true;
            if (!add_letter(&index, &size, &buffer, letter)) {
                final(g, argv, buffer, enter, line, index, false);
            }
            enter = false;
            continue;
        }
        if (buffer[0] == '#' || index == 0) {
            //Nic się nie dzieje - komentarz lub pusta linia
        } else if (batch_mode == false) {
            if (error) fprintf(stderr, "ERROR %lu\n", line);
            else if (buffer[0] == 'I') try_command_I(&g, buffer, argv, line);
            else if (buffer[0] == 'B') try_command_B(&g, buffer, argv, line, &batch_mode);
            else fprintf(stderr, "ERROR %lu\n", line);
        } else {
            if (error) fprintf(stderr, "ERROR %lu\n", line);
            else batch(g, buffer, argv, line);
        }
        enter = true;
        line++;
        if (buffer != NULL) free(buffer);
        buffer = NULL;
        size = 0;
        error = false;
    }
    final(g, argv, buffer, enter, line, index, true);
}
