#pragma once
#include "Carga.h"

// HERENCIA: CargaCritica ES UNA Carga (el enfriador de leche)
class CargaCritica : public Carga {
public:
    CargaCritica(string n, float p) : Carga(n, p) {}

    bool debeEncender(int hora, float precio, float embalses) override {
        return true;
    }

    bool esCritica() override { return true; }
};