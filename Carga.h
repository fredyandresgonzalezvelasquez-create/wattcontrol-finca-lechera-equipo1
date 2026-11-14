#pragma once
#include <string>
using namespace std;

// Clase base abstracta Carga: lo que tienen en comun todas las cargas
class Carga {
protected:
    string nombre;
    float potenciaKW;
    bool encendida;
    int horasUso;

public:
    Carga(string n, float p) {
        nombre = n;
        potenciaKW = p;
        encendida = false;
        horasUso = 0;
    }

    virtual ~Carga() {}

    virtual bool debeEncender(int hora, float precio, float embalses) = 0;
    virtual bool esCritica() { return false; }
    virtual int getHorasMinimas() { return 0; }

    void encender() {
        encendida = true;
        horasUso++;
    }

    void apagar() {
        encendida = false;
    }

    void reiniciar() {
        encendida = false;
        horasUso = 0;
    }

    bool cumpleMinimo() { return horasUso >= getHorasMinimas(); }

    string getNombre() { return nombre; }
    float getPotencia() { return potenciaKW; }
    bool estaEncendida() { return encendida; }
    int getHorasUso() { return horasUso; }
};