// =============================================================
// WattControl - Gestor de energia de una FINCA LECHERA (El Nino 2026)
// EA2: herencia, polimorfismo, composicion, agregacion y asociacion
// Negocio: finca lechera (ultimo digito del documento: 6)
// Autor: Fredy Andres Gonzalez Velasquez
// =============================================================
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "configuracion.h"
#include "Mercado.h"
#include "CargaCritica.h"
#include "CargaAplazable.h"
#include "Negocio.h"

using namespace std;

int main() {
    srand(SEMILLA);

    Mercado mercado;
    CargaCritica enfriador("Enfriador", POTENCIA_ENFRIADOR);
    CargaAplazable ordenadora("Ordenadora", POTENCIA_ORDENADORA, HORAS_MIN_ORDENADORA);
    CargaAplazable bomba("Bomba", POTENCIA_BOMBA, HORAS_MIN_BOMBA);

    Negocio negocio("Finca Lechera", &mercado);
    negocio.agregarCarga(&enfriador);
    negocio.agregarCarga(&ordenadora);
    negocio.agregarCarga(&bomba);
    negocio.mostrar();

    int hora = HORA_EJEMPLO;
    float precio = PRECIO_EJEMPLO, embalses = EMBALSES_EJEMPLO;
    bool haySimulacion = false;
    int opcion;

    do {
        cout << endl << "===== WattControl: Finca Lechera =====" << endl;
        cout << "1. Ingresar lectura manual (hora, precio, embalses)" << endl;
        cout << "2. Evaluar decision de cada carga" << endl;
        cout << "3. Simular un dia completo (24 h)" << endl;
        cout << "4. Ver resumen de la ultima simulacion" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;
        if (cin.fail()) {
            cin.clear();
            opcion = -1;
        }

        switch (opcion) {
        case 1:
            do {
                cout << "Hora (" << HORA_MINIMA << " a " << HORA_MAXIMA << "): ";
                cin >> hora;
                if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); hora = HORA_MINIMA - 1; }
                if (hora < HORA_MINIMA || hora > HORA_MAXIMA) cout << "  Hora invalida." << endl;
            } while (hora < HORA_MINIMA || hora > HORA_MAXIMA);

            do {
                cout << "Precio de bolsa (COP/kWh): ";
                cin >> precio;
                if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); precio = 0; }
                if (precio <= 0) cout << "  El precio debe ser mayor que 0." << endl;
            } while (precio <= 0);

            do {
                cout << "Nivel de los embalses (0 a 100 %): ";
                cin >> embalses;
                if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); embalses = PORCENTAJE_MINIMO - 1; }
                if (embalses < PORCENTAJE_MINIMO || embalses > PORCENTAJE_MAXIMO) cout << "  Nivel invalido." << endl;
            } while (embalses < PORCENTAJE_MINIMO || embalses > PORCENTAJE_MAXIMO);
            cout << "Lectura guardada." << endl;
            break;

        case 2:
            cout << fixed << setprecision(1);
            cout << "Hora " << hora << " | precio " << precio << " | embalses " << embalses << " %" << endl;
            negocio.evaluar(hora, precio, embalses);
            break;

        case 3:
            negocio.simularDia();
            haySimulacion = true;
            break;

        case 4:
            if (haySimulacion) negocio.resumen();
            else cout << "Primero simule un dia con la opcion 3." << endl;
            break;

        case 0:
            cout << "Hasta luego." << endl;
            break;

        default:
            cout << "Opcion invalida." << endl;
        }

        if (opcion != 0) {
            cout << endl << "Presione Enter para continuar...";
            cin.ignore(1000, '\n');
            cin.get();
        }
    } while (opcion != 0);

    return 0;
}