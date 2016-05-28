# cambios pythoneros arduinicos
from time import time, sleep
millis = lambda: int(time() * 1000) % (256**4)  # tiempo sistema en milisegundos en 4 bytes
delay = lambda msecs: sleep(msecs/1E3)  # delay en milisegundos
get_pin = lambda pin: 0
set_pin = lambda pin, bit: 0

# pines
subir_pin_1 = 10
bajar_pin_1 = 11
subir_pin_2 = 12
bajar_pin_2 = 13
boton_pin_1 = 14
boton_pin_2 = 15
celula_pin_1 = 16
celula_pin_2 = 17

# estados
DESCONOCIDO = 0
SUBIENDO = 1
SUBIDA_ESPERANDO_BOTON = 2
SUBIDA_ESPERANDO_ENTRE_COCHE = 3
SUBIDA_ESPERANDO_SALGA_COCHE = 4
BAJANDO = 5
BAJADO = 6


class DetectorCambios:
    # ejemplos de disparo de eventos
    #
    # if boton1.value and boton1.millis > 10:  # recien pulsado boton1 (1 centésima sg)
    #     print "recien pulsado boton 1"
    #
    # if not boton1.value and boton1.milis > 10:  # recien soltado boton1  (1 cent sg)
    #     print "recien soltado boton 1"
    #
    # if celula1.value and celula1.millis > 10:  # recien pulsada celula1 (1 cent sg)
    #     print "recien pulsada celula 1"
    #
    # if estado1.value and estado1.seconds > 2:  # lleva >=2 seg en estado 1
    #     print "tal"

    def __init__(self, value):
        "Constructor poniendo en marcha el crono con el valor pasado por argumento"
        self.value = value
        self.time = millis()
        self.millis = 0
        self.seconds = 0

    def set(self, value):
        "Detectar transiciones y cronometrar cuanto tiempo estan estables"
        current_value = value
        current_time = millis()
        if current_value != self.value:
            self.value = current_value
            self.time = current_time
            self.millis = 0
            self.seconds = 0
        else:
            # FIXME: mirar cuanto tiempo tiene que pasar para que haya overflow
            self.millis = current_time - self.time
            self.seconds = int(self.millis / 1000)

    def update(self):
        "Version recortada de set() cuando no cambia nada"
        current_time = millis()
        self.millis = current_time - self.time
        self.seconds = int(self.millis / 1000)


def pulso(pin):
    set_pin(pin, 1)
    delay(10)
    set_pin(pin, 0)


def maquina_estados(boton, boton_pin, celula, celula_pin, subir_pin, bajar_pin, estado):
    boton.set(get_pin(boton_pin))
    celula.set(get_pin(celula_pin))
    estado.update()

    if estado.value == DESCONOCIDO:  # inicial sin mucho conocimiento del mundo
        if celula.value:  # algo corta
            estado.set(SUBIENDO)
            pulso(subir_pin)
        else:  # nada corta
            estado.set(BAJANDO)
            pulso(bajar_pin)

    elif estado.value == SUBIENDO:
        if estado.seconds > 2:
            if celula.value and celula.seconds >= 1:  # algo corta
                estado.set(SUBIDA_ESPERANDO_SALGA_COCHE)
            else:
                estado.set(SUBIDA_ESPERANDO_ENTRE_COCHE)

    elif estado.value == SUBIDA_ESPERANDO_BOTON:
        if boton.value:
            estado.set(BAJANDO)

    elif estado.value == SUBIDA_ESPERANDO_ENTRE_COCHE:
        if celula.value and celula.seconds >= 1:  # coche pasando durante 1 sg
            estado.set(SUBIDA_ESPERANDO_SALGA_COCHE)

    elif estado.value == SUBIDA_ESPERANDO_SALGA_COCHE:
        if not celula.value and celula.seconds >= 1:  # coche fuera durante 1 sg
            estado.set(BAJANDO)
            pulso(bajar_pin)

    elif estado.value == BAJANDO:
        if celula.value or boton.value:  # pasa algo o boton -> warning -> subir
            estado.set(SUBIENDO)
            pulso(subir_pin)

    elif estado.value == BAJADO:
        if boton.value:
            estado.set(SUBIENDO)
            pulso(subir_pin)


# def setup():
if True:
    boton1 = DetectorCambios(get_pin(boton_pin_1))
    celula1 = DetectorCambios(get_pin(celula_pin_1))
    estado1 = DetectorCambios(DESCONOCIDO)
    boton2 = DetectorCambios(get_pin(boton_pin_2))
    celula2 = DetectorCambios(get_pin(celula_pin_2))
    estado2 = DetectorCambios(DESCONOCIDO)


def loop():
    maquina_estados(boton1, boton_pin_1, celula1, celula_pin_1, subir_pin_1, bajar_pin_1, estado1)
    maquina_estados(boton2, boton_pin_2, celula2, celula_pin_2, subir_pin_2, bajar_pin_2, estado2)
