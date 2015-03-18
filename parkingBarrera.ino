/* Arduino Parking Barrera
Programa que implementa la maquina de estados preparada
para el funcionamiento logico de la barrera del parking
del Anden Sin Limite */

#include <SoftwareSerial.h>

#define DISPARADO_FINCARRERA HIGH
#define DISPARADO LOW
#define INPUTMODE INPUT_PULLUP

#define MHIGH LOW
#define MLOW HIGH

#define pinBtn_S 2
#define pinBtn_B 3
#define pinFoto_1 4
#define pinFoto_2 5
#define pinFin_B 6
#define pinFin_S 7
#define pinR2 8
#define pinR1 9
#define pinExt_B 10
#define pinExt_S 11

char *PINES_STR[] =
    { "pin0", "pin1", "boton subir", "boton bajar", "fotocelula1",
    "fotocelula2",
    "fincarrera subir", "fincarrera bajar", "rele2", "rele1",
    "externa bajar", "externa subir"
};

// Pines NewSoftwareSerial
#define pinRX 10
#define pinTX 11

SoftwareSerial mySerial(pinRX, pinTX);  // RX, TX

// VAriables de la Maquina de estados
int estado;

unsigned long crono_comienzo_subida, crono_diferencia, crono_subida;

#define BAJADA 0
#define SUBIENDO 1
#define SUBIDA 2
#define BAJANDO 3

char *ESTADOS_STR[] = { "bajada", "subiendo", "subida", "bajando" };

boolean tiempoS;
boolean tiempoB;

boolean presentaSerie;

void setup()
{
    pinMode(pinBtn_S, INPUTMODE);
    pinMode(pinBtn_B, INPUTMODE);
    pinMode(pinFoto_1, INPUTMODE);
    pinMode(pinFoto_2, INPUTMODE);
    pinMode(pinFin_B, INPUTMODE);
    pinMode(pinFin_S, INPUTMODE);
    pinMode(pinExt_B, INPUTMODE);
    pinMode(pinExt_S, INPUTMODE);

    pinMode(pinR1, OUTPUT);
    pinMode(pinR2, OUTPUT);
    digitalWrite(pinR1, HIGH);
    digitalWrite(pinR2, HIGH);

    mySerial.begin(57600);
    Serial.begin(9600);

    if (digitalRead(pinFin_B) == DISPARADO_FINCARRERA
        && digitalRead(pinFin_S) != DISPARADO_FINCARRERA) {
        estado = BAJADA;
    } else if (digitalRead(pinFin_B) != DISPARADO_FINCARRERA
               && digitalRead(pinFin_S) == DISPARADO_FINCARRERA) {
        estado = SUBIDA;
    } else {
        estado = SUBIENDO;
    }

    tiempoS = false;
    tiempoB = false;
    presentaSerie = true;
    crono_comienzo_subida = 0;
    crono_diferencia = 0;
    crono_subida = 99999;
}

void loop()
{
    if (presentaSerie) {
        printInputs();
        presentaSerie = false;
    }

    switch (estado) {
    case BAJADA:
        digitalWrite(pinR1, MLOW);
        digitalWrite(pinR2, MLOW);
        if (digitalRead(pinBtn_S) == DISPARADO) {
            estado = SUBIENDO;
            crono_comienzo_subida = millis();
            presentaSerie = true;
        }
        break;
    case SUBIENDO:
        digitalWrite(pinR1, MHIGH);
        digitalWrite(pinR2, MLOW);
        crono_diferencia = millis() - crono_comienzo_subida;
        if (digitalRead(pinFin_S) == DISPARADO_FINCARRERA) {
            if (crono_subida == 99999)
                crono_subida = crono_diferencia * 95 / 100;
        } 
        if (digitalRead(pinFin_S) == DISPARADO_FINCARRERA || crono_diferencia > crono_subida) {
            estado = SUBIDA;
            presentaSerie = true;
            Serial.print("diferencia=");
            Serial.println(crono_diferencia);
        }
        break;
    case SUBIDA:
        digitalWrite(pinR1, MLOW);
        digitalWrite(pinR2, MLOW);
        if (digitalRead(pinBtn_B) == DISPARADO &&
            !digitalRead(pinFoto_1) == DISPARADO &&
            !digitalRead(pinFoto_2) == DISPARADO) {
            estado = BAJANDO;
            presentaSerie = true;
        }
        break;
    case BAJANDO:
        digitalWrite(pinR1, MLOW);
        digitalWrite(pinR2, MHIGH);
        if (digitalRead(pinFoto_1) == DISPARADO ||
            digitalRead(pinFoto_2) == DISPARADO ||
            digitalRead(pinBtn_S) == DISPARADO) {
            estado = SUBIENDO;
            crono_comienzo_subida = millis();
        }
        if (digitalRead(pinFin_B) == DISPARADO_FINCARRERA || tiempoB) {
            estado = BAJADA;
            presentaSerie = true;
        }
        break;
    }

    serialCmd();
}

void serialCmd()
{

    while (Serial.available() > 0) {
        switch (Serial.read()) {
        case 'I':              // print inputs
            printInputs();
            break;
        case 'E':              // echo
            Serial.print("[OK]\n");
            break;
        default:
            // Nada
            break;
        }
    }
}

void printInputs()
{
    Serial.print("Estado: ");
    Serial.println(ESTADOS_STR[estado]);
    printInput(pinBtn_S, DISPARADO);
    printInput(pinBtn_B, DISPARADO);
    printInput(pinFoto_1, DISPARADO);
    printInput(pinFoto_2, DISPARADO);
    printInput(pinFin_B, DISPARADO_FINCARRERA);
    printInput(pinFin_S, DISPARADO_FINCARRERA);
    printInput(pinExt_B, DISPARADO);
    printInput(pinExt_S, DISPARADO);
    Serial.println();
}

void printInput(int pinX, int d)
{
    Serial.print("Input ");
    Serial.print(pinX);
    if (pinX > 1 && pinX < sizeof(ESTADOS_STR) / sizeof(char *)) {
        Serial.print(" ");
        Serial.print(PINES_STR[pinX]);
    }

    Serial.print(" : ");
    int v = digitalRead(pinX);
    if (v == d)
        Serial.println("PULSADO");
    else
        Serial.println("-------");
}
