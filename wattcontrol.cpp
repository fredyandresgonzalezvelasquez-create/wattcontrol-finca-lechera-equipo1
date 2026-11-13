// =============================================================
// (Etapa 2) WattControl - Gestor de energia de una FINCA LECHERA (El Nino 2026)
// EA2: herencia, polimorfismo, composicion, agregacion y asociacion
// Negocio: finca lechera (ultimo digito del documento: 6)
// Autor: Fredy Andres Gonzalez Velasquez
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
// Medidor: acumula los kWh y el costo (parte del Negocio: COMPOSICION)
// =============================================================
class Medidor {
private:
    // Energia total consumida, en kWh
    float kWh;
    // Costo total de esa energia, en pesos
    float costo;

public:
    // Constructor: empieza en 0
    Medidor() {
        kWh = 0;
        costo = 0;
    }

    // Suma la energia de una hora: kW x 1 h = kWh, y su costo = kWh x precio
    void acumular(float potenciaKW, float precio) {
        float energia = potenciaKW * DURACION_HORA;
        kWh = kWh + energia;
        costo = costo + energia * precio;
    }

    // Vuelve a 0 (se usa al empezar cada simulacion)
    void reiniciar() {
        kWh = 0;
        costo = 0;
    }

    // Getters
    float getKWh() { return kWh; }
    float getCosto() { return costo; }
};

// =============================================================
// Mercado: da el precio de cada hora y el nivel de los embalses (ASOCIACION)
// =============================================================
class Mercado {
public:
    // Precio al azar dentro del rango real de la franja de esa hora
    float precioHora(int hora) {
        // Busca la franja: avanza mientras la hora pase del final de la franja
        int f = 0;
        while (f < NUM_FRANJAS - 1 && hora > FRANJA_FIN[f]) f++;
        int minimo = PRECIO_MINIMO[f];
        int maximo = PRECIO_MAXIMO[f];
        // rand() da un numero entero al azar. % (maximo - minimo + 1) lo deja entre 0 y
        // la diferencia; al sumarle el minimo queda entre minimo y maximo
        return minimo + rand() % (maximo - minimo + 1);
    }

    // Nivel de los embalses en esa hora (%): en El Nino empiezan bajos y bajan durante el dia
    float nivelEmbalses(int hora) {
        float nivel = NIVEL_EMBALSES_INICIAL - hora * BAJADA_EMBALSES_HORA;
        // Un nivel nunca es menor que 0
        if (nivel < PORCENTAJE_MINIMO) nivel = PORCENTAJE_MINIMO;
        return nivel;
    }
};

// =============================================================
// Negocio: reune todo y simula el dia
// =============================================================
class Negocio {
private:
    // El nombre del negocio
    string nombre;
    // COMPOSICION: el medidor es un atributo objeto. Nace y muere con el negocio
    Medidor medidor;
    // AGREGACION: un arreglo de punteros (*) a Carga. Las cargas se crean afuera
    // y aqui solo se guarda su direccion
    Carga* cargas[MAX_CARGAS];
    // Cuantas cargas se han agregado
    int numCargas;
    // ASOCIACION: un puntero al mercado. El negocio no lo crea: solo lo conoce
    // y le pide los precios y el nivel de los embalses
    Mercado* mercado;

    // Resultados de la ultima simulacion, para mostrarlos en el resumen (opcion 4)
    // costoSin: lo que costaria con las tres cargas encendidas todo el dia
    float costoSin;
    // horasSimuladas: cuantas horas alcanzo a simular; fallas: lecturas de precio que fallaron
    int horasSimuladas, fallas;
    // recortes: veces que la regla de la potencia contratada apago una carga
    int recortes;
    // racionamiento: true si se declaro emergencia; horaRacionamiento: en que hora
    bool racionamiento;
    int horaRacionamiento;

    // Suma las potencias de las cargas que la decision dejo encendidas
    float potenciaDecidida(bool decision[]) {
        float total = 0;
        for (int i = 0; i < numCargas; i++) {
            if (decision[i]) total = total + cargas[i]->getPotencia();
        }
        return total;
    }

    // REGLA 6: si la potencia encendida supera la contratada, apaga la carga APLAZABLE
    // encendida de mayor potencia. Devuelve cual apago (su posicion), o -1 si no apago ninguna
    int aplicarReglaPotencia(bool decision[]) {
        int apagada = -1;
        if (potenciaDecidida(decision) > POTENCIA_CONTRATADA) {
            // Busca la carga aplazable encendida con mayor potencia.
            // Desempate: si dos tienen la misma potencia, se apaga la que mas horas lleva,
            // asi las dos aplazables se turnan y ninguna se queda sin trabajar
            for (int i = 0; i < numCargas; i++) {
                if (decision[i] && !cargas[i]->esCritica()) {
                    bool esMejorCandidata = apagada == -1
                        || cargas[i]->getPotencia() > cargas[apagada]->getPotencia()
                        || (cargas[i]->getPotencia() == cargas[apagada]->getPotencia()
                            && cargas[i]->getHorasUso() > cargas[apagada]->getHorasUso());
                    if (esMejorCandidata) apagada = i;
                }
            }
            // La apaga en la decision
            if (apagada != -1) decision[apagada] = false;
        }
        return apagada;
    }

    // Aplica la decision: enciende o apaga cada carga y el Medidor acumula lo que gasto.
    // Devuelve el costo de esta hora
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

    // Escribe el aviso de que la regla 6 apago una carga
    void avisarRecorte(bool decisionAntes[], int apagada) {
        // Las potencias llevan 1 decimal (7.5 kW). precision() devuelve la anterior, para restaurarla
        streamsize anterior = cout.precision(1);
        cout << "      ALERTA: la potencia (" << potenciaDecidida(decisionAntes) << " kW) supera la contratada ("
             << POTENCIA_CONTRATADA << " kW): se apaga " << cargas[apagada]->getNombre() << endl;
        cout.precision(anterior);
    }

public:
    // Constructor: recibe el nombre y la direccion del mercado (Mercado* m)
    Negocio(string n, Mercado* m) {
        nombre = n;
        // Guarda la direccion del mercado: desde aqui el negocio lo conoce
        mercado = m;
        numCargas = 0;
        costoSin = 0;
        horasSimuladas = 0;
        fallas = 0;
        recortes = 0;
        racionamiento = false;
        horaRacionamiento = -1;
    }

    // Agrega una carga al negocio. Recibe su direccion (Carga* c)
    void agregarCarga(Carga* c) {
        // Solo si todavia hay espacio en el arreglo
        if (numCargas < MAX_CARGAS) {
            cargas[numCargas] = c;
            numCargas++;
        }
    }

    // Muestra el negocio y sus cargas
    void mostrar() {
        // fixed y setprecision(1): las potencias se muestran con 1 decimal (3.0, 2.2, 4.0)
        cout << fixed << setprecision(1);
        cout << "Negocio: " << nombre << " | potencia contratada " << POTENCIA_CONTRATADA << " kW" << endl;
        for (int i = 0; i < numCargas; i++) {
            cout << "  " << cargas[i]->getNombre() << " (" << cargas[i]->getPotencia() << " kW)"
                 << (cargas[i]->esCritica() ? " - critica" : " - aplazable") << endl;
        }
    }

    // Opcion 2: que haria cada carga con una lectura (hora, precio, embalses)
    void evaluar(int hora, float precio, float embalses) {
        // Cada carga decide con SU regla: la misma pregunta a todas, con el mismo ciclo
        bool decision[MAX_CARGAS];
        for (int i = 0; i < numCargas; i++) {
            decision[i] = cargas[i]->debeEncender(hora, precio, embalses);
        }
        // Copia de la decision antes de la regla 6, para poder avisar
        bool antes[MAX_CARGAS];
        for (int i = 0; i < numCargas; i++) antes[i] = decision[i];
        // REGLA 6: potencia contratada
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

    // Opcion 3: simula 24 horas, con gestor y sin gestor
    void simularDia() {
        // Todo empieza de nuevo en 0 (son atributos: se guardan para el resumen)
        medidor.reiniciar();
        for (int i = 0; i < numCargas; i++) cargas[i]->reiniciar();
        costoSin = 0; horasSimuladas = 0; fallas = 0; recortes = 0;
        racionamiento = false; horaRacionamiento = -1;

        // Potencia de las cargas si estuvieran todas encendidas (para el costo sin gestor)
        float potenciaTotal = 0;
        for (int i = 0; i < numCargas; i++) potenciaTotal = potenciaTotal + cargas[i]->getPotencia();

        // fixed y setprecision(0): los numeros se muestran sin decimales
        cout << fixed << setprecision(0);
        // El encabezado de la tabla; setw(5) ocupa 5 espacios para alinear las columnas
        cout << setw(5) << "Hora" << setw(8) << "Precio" << setw(10) << "Embalses";
        for (int i = 0; i < numCargas; i++) cout << setw(12) << cargas[i]->getNombre();
        cout << setw(9) << "Costo" << endl;

        // Repite para cada hora del dia: h = 0, 1, 2, ..., 23
        for (int h = HORA_MINIMA; h <= HORA_MAXIMA; h++) {
            // ASOCIACION: el negocio le pide al mercado el precio y el nivel de embalses de esta hora
            float precio = mercado->precioHora(h);
            float embalses = mercado->nivelEmbalses(h);

            // break: precio de escasez con embalses bajos -> emergencia de racionamiento
            if (precio > PRECIO_ESCASEZ && embalses < NIVEL_ALERTA_EMBALSES) {
                racionamiento = true;
                horaRacionamiento = h;
                cout << setw(5) << h << setw(8) << precio
                     << "  EMERGENCIA DE RACIONAMIENTO: se detiene la simulacion" << endl;
                // break SALE del for: no se simulan las horas que faltan
                break;
            }
            // Si llego aqui, esta hora si se simula: se cuenta
            horasSimuladas++;

            // Sin gestor: las tres cargas encendidas todo el dia
            costoSin = costoSin + potenciaTotal * DURACION_HORA * precio;

            // La decision de cada carga en esta hora
            bool decision[MAX_CARGAS];

            // continue: falla la lectura del precio -> esa hora solo funciona la carga critica.
            // rand() % FALLA_CADA da un numero de 0 a 19; si da 0 (1 de cada 20 veces)...
            if (rand() % FALLA_CADA == 0) {
                // ...solo queda encendida la carga critica...
                for (int i = 0; i < numCargas; i++) decision[i] = cargas[i]->esCritica();
                // ...el costo igual se cobra con el precio real de esa hora...
                float costoHora = ejecutarHora(decision, precio);
                fallas++;
                // ...se avisa en la tabla (el precio no se pudo leer: "--")...
                cout << setw(5) << h << setw(8) << "--" << setprecision(1) << setw(10) << embalses << setprecision(0);
                for (int i = 0; i < numCargas; i++) cout << setw(12) << (cargas[i]->estaEncendida() ? "ON" : "off");
                cout << setw(9) << costoHora << "  falla de lectura: solo la critica" << endl;
                // ...y continue SALTA a la siguiente hora: lo que sigue no se ejecuta en esta
                continue;
            }

            // Con gestor: cada carga decide (polimorfismo), con el mismo ciclo
            for (int i = 0; i < numCargas; i++) {
                decision[i] = cargas[i]->debeEncender(h, precio, embalses);
            }
            // Copia de la decision antes de la regla 6, para poder avisar
            bool antes[MAX_CARGAS];
            for (int i = 0; i < numCargas; i++) antes[i] = decision[i];
            // REGLA 6: si la potencia supera la contratada, apaga la aplazable de mayor potencia
            int apagada = aplicarReglaPotencia(decision);
            if (apagada != -1) recortes++;

            // Aplica la decision y el Medidor acumula los kWh y el costo
            float costoHora = ejecutarHora(decision, precio);

            // Escribe la fila de la tabla: hora, precio, embalses, ON u off por carga y costo
            cout << setw(5) << h << setw(8) << precio << setprecision(1) << setw(10) << embalses << setprecision(0);
            for (int i = 0; i < numCargas; i++) cout << setw(12) << (cargas[i]->estaEncendida() ? "ON" : "off");
            cout << setw(9) << costoHora << endl;
            // Si la regla 6 apago una carga, lo avisa debajo de la fila
            if (apagada != -1) avisarRecorte(antes, apagada);
        }
        // Los resultados quedaron guardados en los atributos: se ven con la opcion 4
        cout << "Simulacion terminada. Vea el resumen con la opcion 4." << endl;
    }

    // Opcion 4: resumen y ahorro
    void resumen() {
        cout << fixed << setprecision(0);
        cout << "Negocio: " << nombre << endl;
        // Cuantas horas se simularon, cuantas lecturas fallaron y cuantas veces actuo la regla 6
        cout << "Horas simuladas: " << horasSimuladas << " de " << HORAS_DIA
             << " | fallas de lectura: " << fallas << " | recortes por potencia: " << recortes << endl;
        // Si se declaro emergencia, lo dice
        if (racionamiento) cout << "EMERGENCIA DE RACIONAMIENTO declarada en la hora " << horaRacionamiento << endl;
        // Una linea por carga: horas encendida y energia gastada (horas x kW = kWh)
        cout << setprecision(1);
        for (int i = 0; i < numCargas; i++) {
            cout << "  " << setw(12) << left << cargas[i]->getNombre() << right
                 << setw(3) << cargas[i]->getHorasUso() << " h, "
                 << cargas[i]->getHorasUso() * cargas[i]->getPotencia() << " kWh" << endl;
        }
        // La energia total que midio el Medidor
        cout << "Energia total (medidor): " << medidor.getKWh() << " kWh" << endl;
        cout << setprecision(0);
        // Los costos del dia
        cout << "Costo con gestor: $" << medidor.getCosto() << endl;
        cout << "Costo sin gestor: $" << costoSin << endl;
        // El ahorro: lo que se paga sin gestor menos lo que se paga con gestor
        cout << "Ahorro:           $" << costoSin - medidor.getCosto();
        // El ahorro en porcentaje, con 1 decimal. Solo si costoSin > 0, para no dividir por 0
        if (costoSin > 0) cout << " (" << setprecision(1) << (costoSin - medidor.getCosto()) / costoSin * 100 << " %)";
        cout << endl;
        // Horas minimas: si alguna carga no cumplio su minimo, produccion incompleta
        for (int i = 0; i < numCargas; i++) {
            if (!cargas[i]->cumpleMinimo()) {
                cout << "ALERTA: produccion incompleta: " << cargas[i]->getNombre() << " trabajo "
                     << cargas[i]->getHorasUso() << " h de " << cargas[i]->getHorasMinimas() << " h minimas" << endl;
            }
        }
    }
};

// =============================================================
// main de prueba (etapa 2): negocio con la simulacion, todavia sin menu
// =============================================================
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
    // Evalua una lectura de ejemplo y simula un dia
    negocio.evaluar(HORA_EJEMPLO, PRECIO_EJEMPLO, EMBALSES_EJEMPLO);
    negocio.simularDia();
    negocio.resumen();
    return 0;
}