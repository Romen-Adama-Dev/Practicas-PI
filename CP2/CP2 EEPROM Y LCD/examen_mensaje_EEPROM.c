pinMode(13, OUTPUT); // Habilitacion del pin 0C1C de SALIDA

// HABILITACION DEL TIMER 1 FAST PWM
TCCR1A  = 0;
// Aunque sea 4ms en verdad es 8ms si da 16.000..
ICR1 = 16000; // 16.000-> TOP 8ms (125hz) = 16.000.000 / 8 * (16.000)
OCR1A = 6400;  // duty cycle 40% -> 16.000 x 40% => 6.400 -> 1.6 a nivel alto y 2,4 a nivel bajo
TCNT1 = 0;
TCCR1A = B10101010;
TCCR1B = B00011010; // N = 8, MODO 11 = PWM PHASE CORRECT CON OCR1A OCUPADO
TIMSK1 |= (1<<TOIE1);  // Habilitacion  de la mascara OCIE3A



// Revisar la señal por el osciloscopio pin 13


Serial.println("11.- Leer mensaje de texto y almacenar");
Serial.println("12.- Leeer mensaje de memoria y visualizar");



// GUARDAR EN MEMORIA I2C 24LC64 Y cambiar el LCD

void guardarEnMemoria(int direccion, String mensaje) {
  // Obtener la longitud de la cadena
  int longitud = mensaje.length();

  // Escribe la longitud de la cadena en la memoria I2C
  I2C_write_operation((longitud >> 8) & 0xFF, direccion);
  I2C_write_operation(longitud & 0xFF, direccion + 1);

  // Escribe cada carácter de la cadena en la memoria I2C
  for (int i = 0; i < longitud; i++) {
    I2C_write_operation(mensaje.charAt(i), direccion + 2 + i);
  }

  // Agrega el carácter nulo al final de la cadena en la memoria I2C
  I2C_write_operation(0x00, direccion + 2 + longitud);
}




if(11){
  Serial.println("Introducir mensaje => ");

  if (Serial.available() > 0) {
    // Lee la cadena de texto introducida por el teclado
    String incomingString = Serial.readStringUntil(0x0D);

    // Imprime la cadena recibida
    Serial.print("I received: ");
    Serial.println(incomingString);

    // Guarda la cadena en la memoria I2C
    guardarEnMemoria(0x00, incomingString);
  }
}


if(12){
	String leerDesdeMemoria(int direccion) {
  // Lee la longitud de la cadena desde la memoria I2C
  int longitud = (I2C_read_operation(direccion) << 8) | I2C_read_operation(direccion + 1);

  // Lee cada carácter de la cadena desde la memoria I2C
  String resultado = "";
  for (int i = 0; i < longitud; i++) {
    char caracter = (char)I2C_read_operation(direccion + 2 + i);
    // Verifica si es el carácter nulo de terminación
    if (caracter == '\0') {
      break;  // Termina el bucle si se encuentra el carácter nulo
    }
    resultado += caracter;
  }

  return resultado;
}
}

if(12){
	String valorMemoria = leerDesdeMemoria(0x00); //lees de memoria
	clearScreen();// limpias la pantalla
	writeScreen(1,valorMemoria);
	blankDisplay(); // Apaga la pantalla LCD, pero retiene los datos
	delay(50);
	restoreDisplay(); 
	delay(50);
	blankDisplay(); // Apaga la pantalla LCD, pero retiene los datos
	delay(50);
	clearScreen();
	modo = 0; // Volver a modo de visualizacion normal
}


// Modo Cronometro
if (modo == 8) {
		guardarEnMemoria(date.alarm1Seconds, date.alarm1Minute, date.alarm1Hour, direccion_I2C_extendido); // Guarda lap time	
		direccion_I2C_extendido = direccion_I2C_extendido + avanze_I2C_extendido;
		updateLCDCronometro();// Plasmamos los datos en el lcd
}

int modo = 0;     // Modo: 0 (Menú Principal), 1 (Configuración)

if (Buffer[sizeBuffer - 1] == '#' && Buffer[sizeBuffer - 2] == '3') {
	modo = 8; //MODO Visualizacion extenndida
}	

if (Buffer[sizeBuffer - 1] == '#' && Buffer[sizeBuffer - 2] == '9') {
	// Modo normal de visualizacion
	// guardarEnMemoria(date.alarm1Seconds, date.alarm1Minute, date.alarm1Hour, direccion_I2C_extendido + 0x0010); // Guarda lap time
    modo = 0;
}






int value_hex_display[10] = {63, 06, 91, 79, 102, 109, 125, 39, 127, 103};
 
int value_hex_display_with_dot[10] = {191, 134, 219, 207, 230, 237, 253, 167, 255, 231};

int digit = 0;
void display_visualizacion_extendida() {
	PORTL = DOFF;
    switch(digit){
	case 0:
		if(alarmStatus.alarm1Active)
			PORTA = value_hex_display_with_dot[Date.alarm1Seconds % 10];
		}else{
			PORTA = value_hex_display[Date.alarm1Seconds % 10];
		}
	
		PORTL=D4;
		teclado(digit);
		digit++;
	break;
	case 1:
		PORTA = value_hex_display[int(Date.alarm1Seconds / 10) % 10]);
		PORTL=D3;
		teclado(digit);
		digit++;
		break;
	case 2:
		PORTA = 0;
		PORTL = D2;
		teclado(digit);
		digit++;
    break;
	case 3:
		PORTA = 0;
		PORTL = D1;
		teclado(digit);
		digit=0;
		break;
	}
}


