

pinMode(12, OUTPUT); // Habilitacion del pin 0C1C de SALIDA

// HABILITACION DEL TIMER 3 MODO CTC
TCCR1A  = 0;
// Aunque sea 5 ms en verdad es 10ms si da 20.000..
OCR1A = 19999; // 19.999-> TOP   5ms (200hz) = 16.000.000 /  8 * (1 - 20.000)
OCR1B = 9.999; // 9.999 -> desfasada 40% 2 ms a nivel alto y 3 ms a nivel alto duty cycle
TCNT1 = 0;
TCCR1A = B10101011;
TCCR1B = B00011xxx; // N = 64, MODO 11 = PWM PHASE CORRECT CON OCR1A OCUPADO
TIMSK1 |= (1<<OCIE1B);  // Habilitacion  de la mascara OCIE3A



// Modo Cronometro
if (modo == 8) {
		guardarEnMemoria(date.alarm1Seconds, date.alarm1Minute, date.alarm1Hour, direccion_I2C_extendido); // Guarda lap time	
		direccion_I2C_extendido = direccion_I2C_extendido + avanze_I2C_extendido;
		updateLCDCronometro();// Plasmamos los datos en el lcd
}

int modo = 0;     // Modo: 0 (Menú Principal), 1 (Configuración)

if (Buffer[sizeBuffer - 1] == '*' && Buffer[sizeBuffer - 2] == '1') {
	modo = 8; //MODO Visualizacion extenndida
}	

if (Buffer[sizeBuffer - 1] == '*' && Buffer[sizeBuffer - 2] == '7') {
	// Modo normal de visualizacion
	guardarEnMemoria(date.alarm1Seconds, date.alarm1Minute, date.alarm1Hour, direccion_I2C_extendido + 0x0010); // Guarda lap time
    modo = 0;
}

