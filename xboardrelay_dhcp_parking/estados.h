enum Estado {
  INICIAL,
  CERRADA,
  ABRIENDO_PULSO,
  ABRIENDO,
  ABRIENDO_OCUPADA,
  ABIERTA,
  ABIERTA_OCUPADA,
  ABIERTA_LIBRE,
  CERRANDO_PULSO,
  CERRANDO
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
const __FlashStringHelper* EstadoStr();
const __FlashStringHelper* OrdenStr();

extern long unsigned int estado_millis;
extern long unsigned int ultima_orden_millis;
extern bool automatica;
