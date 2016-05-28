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
//#define pinFoto_1 12 // Para que la barrera trabaje sin fotocelula
#define pinFoto_1 4   // Para que la barrera trabaje con fotocelula
#define pinFoto_2 5
#define pinFin_B 7
#define pinFin_S 6
#define pinR2 8
#define pinR1 9
#define pinExt_B 10
#define pinExt_S 11

char *PINES_STR[] =
    { "pin0", "pin1", "boton subir", "boton bajar", "fotocelula1",
    "fotocelula2",
    "fincarrera subida", "fincarrera bajada", "rele2", "rele1",
    "externa bajar", "externa subir"
};

// Pines NewSoftwareSerial
#define pinRX 10
#define pinTX 11

SoftwareSerial mySerial(pinRX, pinTX);  // RX, TX

// VAriables de la Maquina de estados
int estado;
int sig_ext_sube;
int sig_ext_baja;

unsigned long crono_comienzo_subida, crono_diferencia, crono_subida;
unsigned long crono_timeout, crono_diferencia_timeout;
unsigned long crono_coche, crono_diferencia_coche;

#define BAJADA 0
#define SUBIENDO 1
#define SUBIDA 2
#define BAJANDO 3

#define EXT_NULO 0
#define EXT_DISP 1
#define TIEMPO_MS_MINIMO_COCCHE 2000

/* ***
   *** TIEMPO_MS_TIMEOUT reducido para acortar el tiempo
   ***   de espera para bajar la barrera cuando recibe
   ***   la señal externa de bajada
   ***
#define TIEMPO_MS_TIMEOUT 5000
*/
#define TIEMPO_MS_TIMEOUT 2500

boolean coche_paso;
boolean timeout;

char *ESTADOS_STR[] = { "bajada", "subiendo", "subida", "bajando" };

#define CRONO_NULL 99999
boolean tiempoS;
boolean tiempoB;

boolean presentaSerie;
boolean interrumpida_bajada;

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

    // Establce el estado inicial en funcion de lo que
    //  devuelvan los fines de carrera
    if (digitalRead(pinFin_B) == DISPARADO_FINCARRERA
        && digitalRead(pinFin_S) != DISPARADO_FINCARRERA) {
        estado = BAJADA;
    } else if (digitalRead(pinFin_B) != DISPARADO_FINCARRERA
               && digitalRead(pinFin_S) == DISPARADO_FINCARRERA) {
        estado = SUBIDA;
    } else {
        estado = SUBIENDO;
    }

    sig_ext_sube = EXT_NULO;
    sig_ext_baja = EXT_NULO;
    tiempoS = false;
    tiempoB = false;
    presentaSerie = true;
    crono_comienzo_subida = 0;
    crono_diferencia = 0;
    crono_subida = CRONO_NULL;
    interrumpida_bajada = false;

    // Variables del sensor automatico de coche
    coche_paso = false;
    timeout = false;
    crono_diferencia_coche = 0;
    crono_coche = CRONO_NULL;
    crono_diferencia_timeout = 0;
    crono_timeout = CRONO_NULL;
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
        if (sig_ext_baja)
          sig_ext_baja = EXT_NULO;
        if (digitalRead(pinExt_S) == DISPARADO &&
            digitalRead(pinExt_B) != DISPARADO)
          sig_ext_sube = EXT_DISP;
        if (digitalRead(pinBtn_S) == DISPARADO ||
            sig_ext_sube == EXT_DISP) {
            estado = SUBIENDO;
            crono_comienzo_subida = millis();
            presentaSerie = true;
        }
        break;
    case SUBIENDO:
        digitalWrite(pinR1, MHIGH);
        digitalWrite(pinR2, MLOW);
        crono_diferencia = millis() - crono_comienzo_subida;
        if (digitalRead(pinExt_B) == DISPARADO &&
            digitalRead(pinExt_S) != DISPARADO)
          sig_ext_baja = EXT_DISP;
        if (digitalRead(pinFin_S) == DISPARADO_FINCARRERA) {
            if (crono_subida == CRONO_NULL)
                crono_subida = crono_diferencia * 95 / 100;
        } 
        if (digitalRead(pinFin_S) == DISPARADO_FINCARRERA 
                /* ***
                   *** No temporizador de subida
                   *** 
                || (crono_diferencia > crono_subida &&
                  !interrumpida_bajada)
                  */
                  ) {
            estado = SUBIDA;
            presentaSerie = true;
            Serial.print("tiempo_de_subida= ");
            Serial.print(crono_diferencia);
            Serial.println("ms");
        }
        break;
    case SUBIDA:
        digitalWrite(pinR1, MLOW);
        digitalWrite(pinR2, MLOW);
        
        if (sig_ext_sube)
          sig_ext_sube = EXT_NULO;
        if (digitalRead(pinExt_B) == DISPARADO &&
            digitalRead(pinExt_S) != DISPARADO)
          sig_ext_baja = EXT_DISP;
        comprueba_coche_paso();
        interrumpida_bajada = false;
        if ((digitalRead(pinBtn_B) == DISPARADO || 
                (sig_ext_baja == EXT_DISP && (coche_paso || timeout))) &&
                !digitalRead(pinFoto_1) == DISPARADO &&
                !digitalRead(pinFoto_2) == DISPARADO) {
            estado = BAJANDO;
            presentaSerie = true;
            reset_crono_coche();
        }
        break;
    case BAJANDO:
        digitalWrite(pinR1, MLOW);
        digitalWrite(pinR2, MHIGH);
        if (digitalRead(pinExt_S) == DISPARADO &&
            digitalRead(pinExt_B) != DISPARADO)
          sig_ext_sube = EXT_DISP;
        if (digitalRead(pinFoto_1) == DISPARADO ||
                digitalRead(pinFoto_2) == DISPARADO ||
                digitalRead(pinBtn_S) == DISPARADO) {
            estado = SUBIENDO;
            interrumpida_bajada = true;
        }
        if (digitalRead(pinFin_B) == DISPARADO_FINCARRERA || tiempoB) {
            estado = BAJADA;
            presentaSerie = true;
        }
        break;
    }

    serialCmd();
}

void comprueba_coche_paso()
{
    timeout = false;

    if (digitalRead(pinFoto_1) == DISPARADO) {
        // Se resetea el cronometro por exceso de tiempo
        crono_timeout = CRONO_NULL;

        // Si el cronometro del intervalo con sensor bloqueado 
        //   es nulo se incia el cronometro. 
        // Si no, se mide la longitud del intervalo
        if (crono_coche == CRONO_NULL)
            crono_coche = millis();
        else
            crono_diferencia_coche = millis() - crono_coche;
/*  ***
    *** El disparador por la var coche_paso
    ***   queda desactivada con este bloque de comentarios
    ***   Se quiere conseguir que solo se dispare por timeout
    ***
       // El intervalo de tiempo minimo indica si el coche paso
       if (crono_diferencia_coche > TIEMPO_MS_MINIMO_COCCHE)
           coche_paso = true;
       else
           coche_paso = false;
*/
    } else
        // Se chequea si hay un exceso de tiempo en espera
        comprueba_timeout();

}

void reset_crono_coche() {
    crono_coche = CRONO_NULL;
    crono_timeout = CRONO_NULL;
    crono_diferencia_coche = 0;
    crono_diferencia_timeout = 0;
}

void comprueba_timeout()
{
    // Se resetea el cronometro por intervalo 
    crono_coche = CRONO_NULL;

    if (crono_timeout == CRONO_NULL)
        crono_timeout = millis();
    else
        crono_diferencia_timeout = millis() - crono_timeout;

    if (crono_diferencia_timeout > TIEMPO_MS_TIMEOUT)
        timeout = true;
    else
        timeout = false;
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
    if (pinX > 1 && pinX < sizeof(PINES_STR) / sizeof(char *)) {
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
