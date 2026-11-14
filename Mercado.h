#pragma once
#include <cstdlib>
#include "configuracion.h"

// Mercado: da el precio de cada hora y el nivel de los embalses (ASOCIACION)
class Mercado {
public:
    float precioHora(int hora) {
        int f = 0;
        while (f < NUM_FRANJAS - 1 && hora > FRANJA_FIN[f]) f++;
        int minimo = PRECIO_MINIMO[f];
        int maximo = PRECIO_MAXIMO[f];
        return minimo + rand() % (maximo - minimo + 1);
    }

    float nivelEmbalses(int hora) {
        float nivel = NIVEL_EMBALSES_INICIAL - hora * BAJADA_EMBALSES_HORA;
        if (nivel < PORCENTAJE_MINIMO) nivel = PORCENTAJE_MINIMO;
        return nivel;
    }
};