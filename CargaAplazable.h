#pragma once
#include "Carga.h"
#include "configuracion.h"

// HERENCIA: CargaAplazable ES UNA Carga (la ordenadora y la bomba)
class CargaAplazable : public Carga {
private:
    int horasMinimas;

public:
    CargaAplazable(string n, float p, int hMin) : Carga(n, p) {
        horasMinimas = hMin;
    }

    bool debeEncender(int hora, float precio, float embalses) override {
        bool horaPico = hora >= PICO_INICIO && hora <= PICO_FIN;
        bool embalsesBajos = embalses < NIVEL_ALERTA_EMBALSES;
        bool precioCaro = precio > PRECIO_CARO;
        bool aplazar = embalsesBajos || horaPico || precioCaro;
        return !aplazar;
    }

    int getHorasMinimas() override { return horasMinimas; }
};