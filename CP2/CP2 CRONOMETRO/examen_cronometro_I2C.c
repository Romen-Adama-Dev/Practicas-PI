// EXAMEN MODO CRONOMETRO 

pinMode(6, OUTPUT); // Habilitacion del pin 0C4A de SALIDA
	
// HABILITACION DEL TIMER 3 MODO CTC
TCCR4A  = 0;
// Aunque sea 100ms en verdad es 200ms si da 25000..
OCR4A = 24999; // 24.999-> TOP   100ms(200ms) (50hz) = 16.000.000 / 2 * 64 * (1 + 24.999) si fuera 200hz(12.500)
TCNT4 = 0;
TCCR4A = B01010100;
TCCR4B = B00001011; // N = 64, MODO 4 = CTC = 0CRA
TIMSK4 |= (1<<OCIE4A);  // Habilitacion  de la mascara OCIE3A
	
// PONER EN EL OSCILOSCOPIO EL PIN....	

ISR(TIMER4_COMPA_vect){
	digitalWrite(13,1);
	digitalWrite(13,0);
}

// Declarar las siguientes variables

 // Solucion rebote
long  time_old = 0L;
long  time_now = 0L;

// int modo; // variable modo de visualizacion

long int cronometro_ms;
int cronometro_running;

int orden_pulsacionCr = 0;
int ultima_linea_LCD = 3; // Linea que vale o 3 o 4
byte direccion_cronometro = 0x00; 

int lineas_cronometro_LCD = 0;

// Reloj del DS3232
struct CuentaCronometro { int cronometro; int ordenPulsacion; byte dir };

CuentaCronometro cuentaPrimera;
CuentaCronometro cuentaSegunda; 

String lapTime(CuentaCronometro cuenta){
	String tiempo = "";
	
	 // Representación en minutos del valor del cronómetro
	int minutos = cuenta.cronometro / 60000;
  	if (minutos < 10) {
		tiempo += "0"; // Agregar un cero al principio si es necesario
	}
	tiempo += minutos;
	tiempo += ":";
	int segundos = (cuenta.cronometro / 1000) % 60;
	if (segundos < 10) {
		tiempo += "0"; // Agregar un cero al principio si es necesario
	}
	tiempo += segundos;

	tiempo += "       ";
	tiempo += "cuenta.ordenPulsacion;
	tiempo += "      ";
	tiempo += "cuenta.dir;
	return tiempo;
}

void updateLCDCronometro(){
	homeCursor();    
    writeScreen(1, "     " + fechaDS3232(Date) + "       ");
	writeScreen(2, "Cronometro NN Dir.M. ");
	if(lineas_cronometro_LCD==1){
		writeScreen(3, lapTime(cuentaPrimera));	
	}
	if(lineas_cronometro_LCD==2){
		writeScreen(4, lapTime(cuentaSegunda));	
	}
}			
			
void guardarEnMemoria(int direccion, int valor) {
  // Convierte el valor entero a dos bytes
  byte byteAlto = (valor >> 8) & 0xFF;
  byte byteBajo = valor & 0xFF;

  // Escribe los dos bytes en la memoria I2C
  I2C_write_operation(byteAlto, direccion);
  I2C_write_operation(byteBajo, direccion + 1);
}


// Modo Cronometro
if (modo == 8) {
	// Manejo de pulsadores para el cronómetro
	if (PINC == 0xEE) {  // PENTER
		// Para el cronometro y lo pone a cero
		cronometro_ms = 0;
		cronometro_running = 0;
	}else if (PINC == 0xBE) {  // PDOWN
		// GUARDAR EN MEMORIA I2C 24LC64 Y cammbiar el LCD
		guardarEnMemoria(direccion_cronometro, cronometro_ms / 60000); // Guarda minutos
		guardarEnMemoria(direccion_cronometro + 1, (cronometro_ms / 1000) % 60); // Guarda segundos

		
		if(orden_pulsacionCr%2 == 0){
			cuentaPrimera.cronometro = cronometro_ms;
			cuentaPrimera.ordenPulsacion = orden_pulsacionCr;
			cuentaPrimera.dir = direccion_cronometro;
			lineas_cronometro_LCD == 0? lineas_cronometro_LCD = 1 : lineas_cronometro_LCD;
		}else{
			cuentaSegunda.cronometro = cronometro_ms;
			cuentaSegunda.ordenPulsacion = orden_pulsacionCr;
			cuentaSegunda.dir = direccion_cronometro;			
			lineas_cronometro_LCD = 2;
		}
		
		updateLCDCronometro();// Plasmamos los datos en el lcd
		
		// Actualizacion del cronometro
		orden_pulsacionCr = orden_pulsacionCr + 1; 
		direccion_cronometro = direccion_cronometro + 0x02;
		

		} else if (PINC == 0xF6) {  // PUP
			// Continúa el cronómetro
			cronometro_running == 1? cronometro_running = 0: cronometro_running = 1;
		}
	}  

int modo = 0;     // Modo: 0 (Menú Principal), 1 (Configuración)

if (Buffer[sizeBuffer - 1] == '*' && Buffer[sizeBuffer - 2] == '1') {
	modo = 8; //MODO CRONOMETRO
}	

if (Buffer[sizeBuffer - 1] == '*' && Buffer[sizeBuffer - 2] == '2') {
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
		PORTA = value_hex_display[int(((cronometro_ms / 60) / 10) % 10)];
		PORTL=D4;
		teclado(digit);
		digit++;
	break;
	case 1:
		PORTA = value_hex_display[int(((cronometro_ms / 60) / 1) % 10)];
		PORTL=D3;
		teclado(digit);
		digit++;
		break;
	case 2:
		PORTA = value_hex_display_with_dot[int(((cronometro_ms % 60) / 10) % 10)];
		PORTL = D2;
		teclado(digit);
		digit++;
    break;
	case 3:
		PORTA = value_hex_display[int(((tiempo_total % 60) / 1) % 10)];
		PORTL = D1;
		teclado(digit);
		digit=0;
		break;
	}
}
