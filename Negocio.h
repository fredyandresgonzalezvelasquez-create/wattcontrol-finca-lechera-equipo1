#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include "configuracion.h"
#include "Medidor.h"
#include "Carga.h"
#include "Mercado.h"

using namespace std;

// Negocio: une todo y simula el dia
class Negocio {
private:
    string nombre;
    Medidor medidor;
    Carga* cargas[MAX_CARGAS];
    int numCargas;
    Mercado* mercado;

    float costoSin;
    int horasSimuladas, fallas;
    int recortes;
    bool racionamiento;
    int horaRacionamiento;

    float potenciaDecidida(bool decision[]) {
        float total = 0;
        for (int i = 0; i < numCargas; i++) {
            if (decision[i]) total = total + cargas[i]->getPotencia();
        }
        return total;
    }

    int aplicarReglaPotencia(bool decision[]) {
        int apagada = -1;
        if (potenciaDecidida(decision) > POTENCIA_CONTRATADA) {
            for (int i = 0; i < numCargas; i++) {
                if (decision[i] && !cargas[i]->esCritica()) {
                    bool esMejorCandidata = apagada == -1
                        || cargas[i]->getPotencia() > cargas[apagada]->getPotencia()
                        || (cargas[i]->getPotencia() == cargas[apagada]->getPotencia()
                            && cargas[i]->getHorasUso() > cargas[apagada]->getHorasUso());
                    if (esMejorCandidata) apagada = i;
                }
            }
            if (apagada != -1) decision[apagada] = false;
        }
        return apagada;
    }

    float ejecutarHora(bool decision[], float precio) {
        float costoHora = 0;
        for (int i = 0; i < numCargas; i++) {
            if (decision[i]) {
                cargas[i]->encender();
                medidor.acumular(cargas[i]->getPotencia(), precio);
                costoHora = costoHora + cargas[i]->getPotencia() * DURACION_HORA * precio;
            } else {
                cargas[i]->apagar();
            }
        }
        return costoHora;
    }

    void avisarRecorte(bool decisionAntes[], int apagada) {
        streamsize anterior = cout.precision(1);
        cout << "      ALERTA: la potencia (" << potenciaDecidida(decisionAntes) << " kW) supera la contratada ("
             << POTENCIA_CONTRATADA << " kW): se apaga " << cargas[apagada]->getNombre() << endl;
        cout.precision(anterior);
    }

public:
    Negocio(string n, Mercado* m) {
        nombre = n;
        mercado = m;
        numCargas = 0;
        costoSin = 0;
        horasSimuladas = 0;
        fallas = 0;
        recortes = 0;
        racionamiento = false;
        horaRacionamiento = -1;
    }

    void agregarCarga(Carga* c) {
        if (numCargas < MAX_CARGAS) {
            cargas[numCargas] = c;
            numCargas++;
        }
    }

    void mostrar() {
        cout << fixed << setprecision(1);
        cout << "Negocio: " << nombre << " | potencia contratada " << POTENCIA_CONTRATADA << " kW" << endl;
        for (int i = 0; i < numCargas; i++) {
            cout << "  " << cargas[i]->getNombre() << " (" << cargas[i]->getPotencia() << " kW)"
                 << (cargas[i]->esCritica() ? " - critica" : " - aplazable") << endl;
        }
    }

    void evaluar(int hora, float precio, float embalses) {
        bool decision[MAX_CARGAS];
        for (int i = 0; i < numCargas; i++) {
            decision[i] = cargas[i]->debeEncender(hora, precio, embalses);
        }
        bool antes[MAX_CARGAS];
        for (int i = 0; i < numCargas; i++) antes[i] = decision[i];
        int apagada = aplicarReglaPotencia(decision);

        for (int i = 0; i < numCargas; i++) {
            cout << "  " << setw(12) << left << cargas[i]->getNombre() << right;
            if (decision[i]) cout << "ENCENDER" << endl;
            else if (i == apagada) cout << "APAGAR (supera la potencia contratada)" << endl;
            else cout << "APLAZAR (espera una hora mejor)" << endl;
        }
        if (apagada != -1) avisarRecorte(antes, apagada);
        cout << "  Potencia encendida: " << potenciaDecidida(decision) << " kW de "
             << POTENCIA_CONTRATADA << " kW contratados" << endl;
    }

    void simularDia() {
        medidor.reiniciar();
        for (int i = 0; i < numCargas; i++) cargas[i]->reiniciar();
        costoSin = 0; horasSimuladas = 0; fallas = 0; recortes = 0;
        racionamiento = false; horaRacionamiento = -1;

        float potenciaTotal = 0;
        for (int i = 0; i < numCargas; i++) potenciaTotal = potenciaTotal + cargas[i]->getPotencia();

        cout << fixed << setprecision(0);
        cout << setw(5) << "Hora" << setw(8) << "Precio" << setw(10) << "Embalses";
        for (int i = 0; i < numCargas; i++) cout << setw(12) << cargas[i]->getNombre();
        cout << setw(9) << "Costo" << endl;

        for (int h = HORA_MINIMA; h <= HORA_MAXIMA; h++) {
            float precio = mercado->precioHora(h);
            float embalses = mercado->nivelEmbalses(h);

            if (precio > PRECIO_ESCASEZ && embalses < NIVEL_ALERTA_EMBALSES) {
                racionamiento = true;
                horaRacionamiento = h;
                cout << setw(5) << h << setw(8) << precio
                     << "  EMERGENCIA DE RACIONAMIENTO: se detiene la simulacion" << endl;
                break;
            }
            horasSimuladas++;
            costoSin = costoSin + potenciaTotal * DURACION_HORA * precio;

            bool decision[MAX_CARGAS];
            if (rand() % FALLA_CADA == 0) {
                for (int i = 0; i < numCargas; i++) decision[i] = cargas[i]->esCritica();
                float costoHora = ejecutarHora(decision, precio);
                fallas++;
                cout << setw(5) << h << setw(8) << "--" << setprecision(1) << setw(10) << embalses << setprecision(0);
                for (int i = 0; i < numCargas; i++) cout << setw(12) << (cargas[i]->estaEncendida() ? "ON" : "off");
                cout << setw(9) << costoHora << "  falla de lectura: solo la critica" << endl;
                continue;
            }

            for (int i = 0; i < numCargas; i++) {
                decision[i] = cargas[i]->debeEncender(h, precio, embalses);
            }
            bool antes[MAX_CARGAS];
            for (int i = 0; i < numCargas; i++) antes[i] = decision[i];
            int apagada = aplicarReglaPotencia(decision);
            if (apagada != -1) recortes++;

            float costoHora = ejecutarHora(decision, precio);

            cout << setw(5) << h << setw(8) << precio << setprecision(1) << setw(10) << embalses << setprecision(0);
            for (int i = 0; i < numCargas; i++) cout << setw(12) << (cargas[i]->estaEncendida() ? "ON" : "off");
            cout << setw(9) << costoHora << endl;
            if (apagada != -1) avisarRecorte(antes, apagada);
        }
        cout << "Simulacion terminada. Vea el resumen con la opcion 4." << endl;
    }

    void resumen() {
        cout << fixed << setprecision(0);
        cout << "Negocio: " << nombre << endl;
        cout << "Horas simuladas: " << horasSimuladas << " de " << HORAS_DIA
             << " | fallas de lectura: " << fallas << " | recortes por potencia: " << recortes << endl;
        if (racionamiento) cout << "EMERGENCIA DE RACIONAMIENTO declarada en la hora " << horaRacionamiento << endl;
        cout << setprecision(1);
        for (int i = 0; i < numCargas; i++) {
            cout << "  " << setw(12) << left << cargas[i]->getNombre() << right
                 << setw(3) << cargas[i]->getHorasUso() << " h, "
                 << cargas[i]->getHorasUso() * cargas[i]->getPotencia() << " kWh" << endl;
        }
        cout << "Energia total (medidor): " << medidor.getKWh() << " kWh" << endl;
        cout << setprecision(0);
        cout << "Costo con gestor: $" << medidor.getCosto() << endl;
        cout << "Costo sin gestor: $" << costoSin << endl;
        cout << "Ahorro:           $" << costoSin - medidor.getCosto();
        if (costoSin > 0) cout << " (" << setprecision(1) << (costoSin - medidor.getCosto()) / costoSin * 100 << " %)";
        cout << endl;
        for (int i = 0; i < numCargas; i++) {
            if (!cargas[i]->cumpleMinimo()) {
                cout << "ALERTA: produccion incompleta: " << cargas[i]->getNombre() << " trabajo "
                     << cargas[i]->getHorasUso() << " h de " << cargas[i]->getHorasMinimas() << " h minimas" << endl;
            }
        }
    }
};