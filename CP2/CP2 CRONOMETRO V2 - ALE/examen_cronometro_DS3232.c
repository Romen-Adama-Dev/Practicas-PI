// HABILITACION DEL TIMER 3 MODO CTC
TCCR1A  = 0;
// Aunque sea 5ms en verdad es 10ms si da 25000..
OCR1A = 1249; // 1.249-> TOP   5ms (200hz) = 16.000.000 / 2 * 64 * (1 + 1.249)
OCR1B = OCR1A/2; // 625-> TOP   Desfasada 2,5 milisegundos
TCNT1 = 0;
TCCR1A = B01010100;
TCCR1B = B00001011; // N = 64, MODO 4 = CTC = 0CRA
TIMSK1 |= (1<<OCIE1A);  // Habilitacion  de la mascara OCIE3A
TIMSK1 |= (1<<OCIE1B);  // Habilitacion  de la mascara OCIE3A	

ISR(TIMER3_COMPA_vect){
	digitalWrite(10,1);
	// Resto del codigo de la practica anterior...
	digitalWrite(10,0);
}

ISR(TIMER3_COMPB_vect){
	digitalWrite(13,1);
	// Esta rutina para realizar le cronometro
	digitalWrite(13,0);
}

// Conectar las salidas IO11 y IO13 al osciloscopio y estar IO11 y IO12 ya conectados


 // Solucion rebote
long  time_old = 0L;
long  time_now = 0L;

long int cronometro_ms;
int cronometro_running;

int numero_vuelta = 0;
int ultima_linea_LCD = 3; // Linea que vale o 3 o 4
int lineas_cronometro_LCD = 0;
byte direccion_cronometro = 0x14; // Direccion inicial del SRAM del DS3232
byte avanze_direccion_cronometro = 0x0B;// Avanza de  12 en 12...


// Reloj del DS3232
struct CuentaCronometro { int cronometro;  int numeroVuelta;};

CuentaCronometro cuentaPrimera;
CuentaCronometro cuentaSegunda; 

String lapTime(CuentaCronometro cuenta){
  String tiempo = "";
  tiempo += "Vuelta ";
  tiempo += cuenta.numeroVuelta;
  tiempo += "  ";

  int minutos = cuenta.cronometro / 60000;
  if (minutos < 10) {
	tiempo += "0"; // Agregar un cero al principio si es necesario
  }
  tiempo += minutos;
  tiempo += ":";

  // Representación de los segundos del valor del cronómetro
  int segundos = (cuenta.cronometro / 1000) % 60;
  if (segundos < 10) {
    tiempo += "0"; // Agregar un cero al principio si es necesario
  }
  tiempo += segundos;

  tiempo += ":";

  // Representación de las centésimas del valor del cronómetro
  int centesimas = (cuenta.cronometro % 1000) / 10;
  if (centesimas < 10) {
    tiempo += "0"; // Agregar un cero al principio si es necesario
  }
  tiempo += centesimas;

  return tiempo;
}

void updateLCDCronometro(){
	homeCursor();    
    writeScreen(1, "     " + fechaDS3232(Date) + "       ");
	
	int minutos = cronometro_ms / 60000;
	if (minutos < 10) {
		writeScreen(2, "Crono=>Minutos   0" + minutos);
	}else{
		writeScreen(2, "Crono=>Minutos    " + minutos);
	}
	if(lineas_cronometro_LCD==1){
		writeScreen(3, lapTime(cuentaPrimera));	
	}
	if(lineas_cronometro_LCD==2){
		writeScreen(4, lapTime(cuentaSegunda));	
	}
}	



void guardarEnMemoria(int direccion, int valor) {
  // Convierte el valor entero a minutos, segundos y centésimas
  int minutos = valor / 60000;
  int segundos = (valor / 1000) % 60;
  int centesimas = (valor % 1000) / 10;

  // Escribe los minutos en la memoria I2C
  DS3232_write_operation((minutos / 10) + '0', direccion);
  DS3232_write_operation((minutos % 10) + '0', direccion + 1);
  // Escribe los dos puntos ":"
  DS3232_write_operation(':', direccion + 2);

  // Escribe los segundos en la memoria I2C
  DS3232_write_operation((segundos / 10) + '0', direccion + 3);
  DS3232_write_operation((segundos % 10) + '0', direccion + 4);
  // Escribe los dos puntos ":"
  DS3232_write_operation(':', direccion + 5);

  // Escribe las centésimas en la memoria I2C
  DS3232_write_operation((centesimas / 10) + '0', direccion + 6);
  DS3232_write_operation((centesimas % 10) + '0', direccion + 7);
  // Escribe el punto y coma ";"
  DS3232_write_operation(';', direccion + 8);

  // Escribe el número de vuelta en la memoria I2C
  DS3232_write_operation(((numero_vuelta / 100) % 10) + '0', direccion + 9);
  DS3232_write_operation(((numero_vuelta / 10) % 10) + '0', direccion + 10);
  DS3232_write_operation((numero_vuelta % 10) + '0', direccion + 11);
  // Escribe la barra "-"
  DS3232_write_operation('-', direccion + 12);
}



int value_hex_display[10] = {63, 06, 91, 79, 102, 109, 125, 39, 127, 103};
 
int value_hex_display_with_dot[10] = {191, 134, 219, 207, 230, 237, 253, 167, 255, 231};

// Modo Cronometro
if (modo == 8) {
	// Manejo de pulsadores para el cronómetro
	if (PINC == 0xEE) {  // PENTER
		// Para el cronometro y lo pone a cero
		cronometro_ms = 0;
		cronometro_running = 0;
	}else if (PINC == 0xBE) {  // PDOWN
		// GUARDAR EN MEMORIA DS3232 Y cammbiar el LCD
		guardarEnMemoria(direccion_cronometro, cronometro_ms); // Guarda lap time		
		
		if(numero_vuelta%2 == 0){
			cuentaPrimera.cronometro = cronometro_ms;
			cuentaPrimera.numeroVuelta = numero_vuelta;
			lineas_cronometro_LCD == 0? lineas_cronometro_LCD = 1 : lineas_cronometro_LCD;
		}else{
			cuentaSegunda.cronometro = cronometro_ms;
			cuentaSegunda.numeroVuelta = numero_vuelta;		
			lineas_cronometro_LCD = 2;
		}
		
		updateLCDCronometro();// Plasmamos los datos en el lcd
		
		// Actualizacion del cronometro
		numero_vuelta = numero_vuelta + 1; 
		direccion_cronometro = direccion_cronometro + avanze_direccion_cronometro;
		
		// Llegamos al limite de la memoria
		if(direccion_cronometro >= 0xFF){
			direccion_cronometro = 0x14;
		}

		} else if (PINC == 0xF6) {  // PUP
			// Continúa el cronómetro
			cronometro_running == 1? cronometro_running = 0: cronometro_running = 1;
		}
	}  

int modo = 0;     // Modo: 0 (Menú Principal), 1 (Configuración)

if (Buffer[sizeBuffer - 1] == '*' && Buffer[sizeBuffer - 2] == '4') {
	modo = 8; //MODO CRONOMETRO
}	

if (Buffer[sizeBuffer - 1] == '*' && Buffer[sizeBuffer - 2] == '5') {
	// Modo normal de visualizacion
    modo = 0;
}
	

int value_hex_display[10] = {63, 06, 91, 79, 102, 109, 125, 39, 127, 103};
 
int value_hex_display_with_dot[10] = {191, 134, 219, 207, 230, 237, 253, 167, 255, 231};

int digit = 0;
void display_cronometro() {
	PORTL = DOFF;
    switch(digit){
	case 0:
		PORTA = value_hex_display[int(cronometro_ms % 10)];
		PORTL=D4;
		teclado(digit);
		digit++;
	break;
	case 1:
		PORTA = value_hex_display[int((cronometro_ms / 10) % 10)];
		PORTL=D3;
		teclado(digit);
		digit++;
		break;
	case 2:
		PORTA = value_hex_display_with_dot[int((cronometro_ms / 100)% 10)];
		PORTL = D2;
		teclado(digit);
		digit++;
    break;
	case 3:
		PORTA = value_hex_display[int((cronometro_ms / 100)/ 10)];
		PORTL = D1;
		teclado(digit);
		digit=0;
		break;
	}
}

