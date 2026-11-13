// =============================================================
// WattControl - Gestor de energia de una FINCA LECHERA (El Nino 2026)
// EA2: herencia, polimorfismo, composicion, agregacion y asociacion
// Negocio: finca lechera (ultimo digito del documento: 6)
// Autor: Fredy Andres Gonzalez Velasque<
// =============================================================
// --- Librerias: codigo que ya existe y que se trae al programa -------------
// iostream: cout (escribir en pantalla) y cin (leer del teclado)
#include <iostream>
// iomanip: setw (ancho de columna) y setprecision (cantidad de decimales)
#include <iomanip>
// string: el tipo string, para guardar textos
#include <string>
// cstdlib: rand() y srand(), para generar numeros al azar
#include <cstdlib>

// Permite escribir cout en vez de std::cout (todo lo de la libreria esta en "std")
using namespace std;

// --- Directivas del preprocesador ------------------------------------------
// Antes de compilar, el preprocesador cambia cada nombre por su valor en todo el archivo.
// Horas de la simulacion: un dia
#define HORAS_DIA 24
// Cuantas cargas puede tener el negocio, como maximo (tamano del arreglo de punteros)
#define MAX_CARGAS 5
// Semilla de rand(): con la misma semilla se obtienen los mismos precios al azar
#define SEMILLA 2026
// Cuantas franjas horarias tiene la tabla de precios del mercado
#define NUM_FRANJAS 5

// --- Constantes del negocio: FINCA LECHERA ---------------------------------
// Potencias de las tres cargas, en kW
// CARGA CRITICA: el enfriador del tanque de leche nunca se apaga (la leche se dana)
const float POTENCIA_ENFRIADOR = 3.0;
// CARGA APLAZABLE: la ordenadora (equipo de ordeno) se puede aplazar unas horas
const float POTENCIA_ORDENADORA = 3.0;
// CARGA APLAZABLE: la bomba de agua de los bebederos y el riego tambien se aplaza
const float POTENCIA_BOMBA = 3.0;
// Potencia contratada con la empresa de energia, en kW
const float POTENCIA_CONTRATADA = 7.5;
// COP/kWh: si el precio pasa de aqui, la energia esta "cara" y las cargas aplazables esperan
const float PRECIO_CARO = 800.0;
// COP/kWh: si el precio pasa de aqui (y los embalses estan bajos), emergencia de racionamiento
const float PRECIO_ESCASEZ = 1500.0;
// La hora pico empieza a las 18 h (6 pm)...
const int PICO_INICIO = 18;
// ...y termina a las 20 h (la hora 20 esta incluida)
const int PICO_FIN = 20;
// %: si los embalses estan por debajo de este nivel, las cargas aplazables esperan
const float NIVEL_ALERTA_EMBALSES = 40.0;
// Horas minimas al dia que debe funcionar cada carga aplazable (ver justificacion en el PDF)
// Ordenadora: 2 ordenos al dia de 3 horas cada uno
const int HORAS_MIN_ORDENADORA = 6;
// Bomba: 4 horas para llenar los tanques de agua de los bebederos y para el riego
const int HORAS_MIN_BOMBA = 4;
// 1 de cada 20 lecturas del precio falla
const int FALLA_CADA = 20;

// --- Constantes del mercado y de la simulacion -----------------------------
// % de los embalses al empezar el dia (en El Nino los embalses estan bajos)...
const float NIVEL_EMBALSES_INICIAL = 46.0;
// ...y cuantos puntos de % bajan cada hora
const float BAJADA_EMBALSES_HORA = 0.5;
// Primera y ultima hora validas, y limites de un porcentaje
const int HORA_MINIMA = 0;
const int HORA_MAXIMA = HORAS_DIA - 1;
const float PORCENTAJE_MINIMO = 0.0;
const float PORCENTAJE_MAXIMO = 100.0;
// Cada paso de la simulacion dura una hora: kWh = kW x 1 h
const float DURACION_HORA = 1.0;
// Valores de ejemplo de la lectura manual (antes de que el usuario escriba la suya)
const int HORA_EJEMPLO = 12;
const float PRECIO_EJEMPLO = 500.0;
const float EMBALSES_EJEMPLO = 60.0;
// Precios reales de bolsa por franja (XM): hasta que hora llega cada franja,
// y su precio minimo y maximo en COP/kWh.
// Franjas: madrugada (0-6), manana y mediodia (7-14), tarde (15-17), hora pico (18-20), noche (21-23)
const int FRANJA_FIN[NUM_FRANJAS] = {6, 14, 17, 20, 23};
const int PRECIO_MINIMO[NUM_FRANJAS] = {398, 456, 596, 705, 565};
const int PRECIO_MAXIMO[NUM_FRANJAS] = {640, 970, 1040, 1612, 970};

// =============================================================
// Clase base abstracta Carga: lo que tienen en comun todas las cargas
// =============================================================
// Cada carga real (enfriador, ordenadora, bomba) se crea con el molde de su clase hija,
// que hereda todo lo de aqui
class Carga {
// protected: estos atributos los ven esta clase y sus hijas, pero no el resto del programa
protected:
    // El nombre de la carga: "Enfriador", "Ordenadora", "Bomba"
    string nombre;
    // La potencia en kW
    float potenciaKW;
    // true si esta encendida en esta hora; false si esta apagada
    bool encendida;
    // Cuantas horas ha estado encendida (para el resumen)
    int horasUso;

public:
    // Constructor: recibe el nombre (n) y la potencia (p). Toda carga empieza apagada y sin horas
    Carga(string n, float p) {
        nombre = n;
        potenciaKW = p;
        encendida = false;
        horasUso = 0;
    }

    // Destructor virtual: obligatorio en una clase base con metodos virtuales
    virtual ~Carga() {}

    // POLIMORFISMO: cada clase hija decide a su manera si se enciende.
    // "= 0" = metodo virtual puro: Carga no lo implementa; cada hija debe hacerlo.
    // Por eso Carga es ABSTRACTA: no se pueden crear objetos Carga directamente
    virtual bool debeEncender(int hora, float precio, float embalses) = 0;

    // Dice si la carga es critica. Por defecto no; CargaCritica lo cambia a true
    virtual bool esCritica() { return false; }

    // Horas minimas de funcionamiento al dia. Por defecto 0; CargaAplazable lo cambia
    virtual int getHorasMinimas() { return 0; }

    // Enciende la carga durante esta hora y suma una hora de uso
    void encender() {
        encendida = true;
        horasUso++;
    }

    // Apaga la carga durante esta hora
    void apagar() {
        encendida = false;
    }

    // Deja la carga como nueva: apagada y con 0 horas (se usa al empezar cada simulacion)
    void reiniciar() {
        encendida = false;
        horasUso = 0;
    }

    // Dice si ya cumplio sus horas minimas del dia
    bool cumpleMinimo() { return horasUso >= getHorasMinimas(); }

    // Getters: devuelven un atributo protegido para que el resto del programa lo lea
    string getNombre() { return nombre; }
    float getPotencia() { return potenciaKW; }
    bool estaEncendida() { return encendida; }
    int getHorasUso() { return horasUso; }
};

// =============================================================
// HERENCIA: CargaCritica ES UNA Carga (el enfriador de leche)
// =============================================================
// ": public Carga" = CargaCritica hereda todo lo de Carga
class CargaCritica : public Carga {
public:
    // Le pasa el nombre y la potencia al constructor de Carga
    CargaCritica(string n, float p) : Carga(n, p) {}

    // REGLA 1: la carga critica nunca se apaga, siempre devuelve verdadero.
    // Recibe hora, precio y embalses aunque no los use, porque debe tener la misma
    // forma que debeEncender de Carga
    bool debeEncender(int hora, float precio, float embalses) override {
        return true;
    }

    // La carga critica dice que si es critica
    bool esCritica() override { return true; }
};

// =============================================================
// HERENCIA: CargaAplazable ES UNA Carga (la ordenadora y la bomba)
// =============================================================
class CargaAplazable : public Carga {
private:
    // Horas minimas que esta carga debe funcionar al dia
    int horasMinimas;

public:
    // Recibe ademas las horas minimas. Nombre y potencia se los pasa a Carga
    CargaAplazable(string n, float p, int hMin) : Carga(n, p) {
        horasMinimas = hMin;
    }

    // REGLAS 2 a 5: la carga se APLAZA si los embalses estan bajos, si es hora pico
    // o si el precio esta caro. Se combinan con operadores logicos
    bool debeEncender(int hora, float precio, float embalses) override {
        // Hora pico: la hora esta entre 18 y 20 (&& = "y": las dos condiciones)
        bool horaPico = hora >= PICO_INICIO && hora <= PICO_FIN;
        // Embalses por debajo del nivel de alerta (40 %)
        bool embalsesBajos = embalses < NIVEL_ALERTA_EMBALSES;
        // Precio por encima del precio "caro" del negocio
        bool precioCaro = precio > PRECIO_CARO;
        // Se aplaza si se cumple ALGUNA de las tres (|| = "o")
        bool aplazar = embalsesBajos || horaPico || precioCaro;
        // ! = "no": se enciende cuando NO hay que aplazar
        return !aplazar;
    }

    // Devuelve las horas minimas de esta carga
    int getHorasMinimas() override { return horasMinimas; }
};

// =============================================================
// main de prueba (etapa 1): comprueba la herencia y el polimorfismo
// =============================================================
int main() {
    // Dos objetos de clases hijas distintas
    CargaCritica enfriador("Enfriador", POTENCIA_ENFRIADOR);
    CargaAplazable ordenadora("Ordenadora", POTENCIA_ORDENADORA, HORAS_MIN_ORDENADORA);

    // Un arreglo de punteros a Carga: cada puntero apunta a un objeto de una clase hija
    const int TOTAL_PRUEBA = 2;
    Carga* cargas[TOTAL_PRUEBA] = { &enfriador, &ordenadora };

    // Lectura de prueba: hora, precio y embalses de ejemplo
    cout << "Hora " << HORA_EJEMPLO << ", precio " << PRECIO_EJEMPLO << ", embalses " << EMBALSES_EJEMPLO << " %" << endl;
    // POLIMORFISMO: la misma pregunta a todas, cada una responde con su regla
    for (int i = 0; i < TOTAL_PRUEBA; i++) {
        cout << cargas[i]->getNombre() << ": "
             << (cargas[i]->debeEncender(HORA_EJEMPLO, PRECIO_EJEMPLO, EMBALSES_EJEMPLO) ? "ENCENDER" : "APLAZAR") << endl;
    }
    // Con hora pico, la aplazable espera pero la critica sigue encendida
    cout << "Hora " << PICO_INICIO << " (hora pico):" << endl;
    for (int i = 0; i < TOTAL_PRUEBA; i++) {
        cout << cargas[i]->getNombre() << ": "
             << (cargas[i]->debeEncender(PICO_INICIO, PRECIO_EJEMPLO, EMBALSES_EJEMPLO) ? "ENCENDER" : "APLAZAR") << endl;
    }
    return 0;
}