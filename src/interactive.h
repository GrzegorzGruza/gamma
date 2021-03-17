/** @file
 * Interfejs klasy odpowiadającej za tryb interaktywny
 * @author Grzegorz Gruza <gg417923@mimuw.edu.pl>
 * @copyright Grzegorz Gruza
 * @date 17.05.2020
 */

#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include "gamma.h"
#include <termios.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include <sys/ioctl.h>

/** @brief Obsługuje rozgrywkę w trybie interaktywnym
 * Wyświetla prostokątną plaszę początkowo wypełnioną przez '.'.
 * Pod planszą wyświetla napis:
 * "Player NR BUSY FREE GOLDEN", gdzie NR to numer gracza (początkowo 1),
 * BUSY i FREE to odpowiednio gamma_busy_fields i gamma_free_fields
 * wywołane dla @p player, natomiast GOLDEN to znak 'G'
 * jeśli gamma_golden_possible wywołana dla @p player zwróci @p true
 * lub nic, jeśli zwróci @p false
 * Plansza może byc modyfikowana w następujący sposób:
 * – strzałkami zmienia się wybrane pole (odpowiedno góra/dól/prawo/lewo),
 * – spacją wykonuje się ruchu,
 * – klawiszem 'G' wykonuje się złoty ruch (o ile jest możliwy),
 * – klawiszem 'C' rezygnuje się z ruchu (o ile jest możliwy),
 * – kombinacją Ctr + D kończy się działanie gry.
 * Wydanie powyższych komend modyfikuje zmianę wyglądu planszy i napisu pod nią.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 */
void interactive_mode(gamma_t *g);


#endif /* INTERACTIVE_H */
