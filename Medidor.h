#pragma once
#include "configuracion.h"

// Medidor: acumula los kWh y el costo (parte del Negocio: COMPOSICION)
class Medidor {
private:
    float kWh;
    float costo;

public:
    Medidor() {
        kWh = 0;
        costo = 0;
    }

    void acumular(float potenciaKW, float precio) {
        float energia = potenciaKW * DURACION_HORA;
        kWh = kWh + energia;
        costo = costo + energia * precio;
    }

    void reiniciar() {
        kWh = 0;
        costo = 0;
    }

    float getKWh() { return kWh; }
    float getCosto() { return costo; }
};