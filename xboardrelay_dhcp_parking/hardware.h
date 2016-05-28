// Conexionado:
//   * RELE2:
//     - COM2 -> negativo del motor
//     - NC2 -> 0V
//     - NO2 -> 12V
//   * RELE1:
//     - COM1 -> positivo del motor
//     - NC1 -> 0V
//     - NO1 -> 12V

void hardware_setup();
void apaga();
void cierra();
void abre();
bool rele1();
bool rele2();
bool obstaculo();
bool final_carrera_abierta();
bool final_carrera_cerrada();
bool boton_abrir_automatico();
bool boton_abrir_manual();
bool boton_cerrar();
