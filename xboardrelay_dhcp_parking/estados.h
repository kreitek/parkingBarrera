enum Estado {
  INICIAL,
  CERRADA,
  ABRIENDO_AUTOMATICO,
  REABRIENDO_AUTOMATICO,
  ABIERTA_SIN_OCUPAR,
  ABIERTA_OCUPADA,
  ABIERTA_LIBRE,
  CERRANDO_AUTOMATICO,
  ABRIENDO_MANUAL,
  ABIERTA_MANUAL
};

enum Orden {
  ORDEN_NINGUNA,
  ORDEN_ABRIR_AUTOMATICO,
  ORDEN_ABRIR_MANUAL,
  ORDEN_CERRAR
};

void estado_loop();
void orden_siguiente(Orden siguiente);
void estado_siguiente(Estado siguiente);
bool esta_cerrada();
unsigned int estado_mask();
const __FlashStringHelper* EstadoStr();
const __FlashStringHelper* OrdenStr();

extern long unsigned int estado_millis;
extern long unsigned int ultima_orden_millis;
