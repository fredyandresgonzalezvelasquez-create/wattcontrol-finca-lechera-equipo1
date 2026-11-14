#pragma once

// --- Directivas del preprocesador ------------------------------------------
#define HORAS_DIA 24
#define MAX_CARGAS 5
#define SEMILLA 2026
#define NUM_FRANJAS 5

// --- Constantes del negocio: FINCA LECHERA ---------------------------------
const float POTENCIA_ENFRIADOR = 3.0;
const float POTENCIA_ORDENADORA = 3.0;
const float POTENCIA_BOMBA = 3.0;
const float POTENCIA_CONTRATADA = 7.5;
const float PRECIO_CARO = 800.0;
const float PRECIO_ESCASEZ = 1500.0;
const int PICO_INICIO = 18;
const int PICO_FIN = 20;
const float NIVEL_ALERTA_EMBALSES = 40.0;
const int HORAS_MIN_ORDENADORA = 6;
const int HORAS_MIN_BOMBA = 4;
const int FALLA_CADA = 20;

// --- Constantes del mercado y de la simulacion -----------------------------
const float NIVEL_EMBALSES_INICIAL = 46.0;
const float BAJADA_EMBALSES_HORA = 0.5;
const int HORA_MINIMA = 0;
const int HORA_MAXIMA = HORAS_DIA - 1;
const float PORCENTAJE_MINIMO = 0.0;
const float PORCENTAJE_MAXIMO = 100.0;
const float DURACION_HORA = 1.0;
const int HORA_EJEMPLO = 12;
const float PRECIO_EJEMPLO = 500.0;
const float EMBALSES_EJEMPLO = 60.0;

const int FRANJA_FIN[NUM_FRANJAS] = {6, 14, 17, 20, 23};
const int PRECIO_MINIMO[NUM_FRANJAS] = {398, 456, 596, 705, 565};
const int PRECIO_MAXIMO[NUM_FRANJAS] = {640, 970, 1040, 1612, 970};