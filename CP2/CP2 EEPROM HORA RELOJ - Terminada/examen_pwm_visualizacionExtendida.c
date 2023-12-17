
pinMode(13, OUTPUT); // Habilitacion del pin 0C1C de SALIDA

// HABILITACION DEL TIMER 1 modo PWM PHASE CORRECT
TCCR1A  = 0;
// Aunque sea 10ms en verdad es 20ms si da 2500..
OCR1A = 1250; // 1.250-> TOP   5ms (200hz) = 16.000.000 / 2 * 64 * (1.250)
OCR1C = 500; // 1.250 x 40%  de duty cycle 2 ms a nivel alto y 3ms a nivel bajo
TCNT1 = 0;
TCCR1A = B10101011;
TCCR1B = B00010011; // N = 64, MODO 11 = PWM PHASE CORRECT CON OCR1A OCUPADO
TIMSK1 |= (1<<OCIE1A);  // Habilitacion  de la mascara OCIE3A

// PONER EN EL OSCILOSCOPIO EL PIN....	

byte direccion_I2C_extendido = 0x00; // Direccion inicial del EEPROM I2C
byte avanze_I2C_extendido = 0x08;// Avanza de  8 en 8...

struct date_time date;

char *dias_semana[] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"};
char *token;

char* calcular_dia_semana(int day, int month, int year) {
    // Códigos del día
    int codigo_dia = day % 7;

    // Códigos del mes
    int codigos_mes[] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
    int codigo_mes = codigos_mes[month - 1];

    // Códigos del año
    int codigo_anio;
    if (year >= 2000) {
        codigo_anio = year % 100;
    } else {
        codigo_anio = (year % 100 + 100) % 100;
    }

    // Sumar un adicional por cada año bisiesto
    codigo_anio += codigo_anio / 4;

    // Ajustes adicionales para enero y febrero en años bisiestos
    if (month <= 2) {
        if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
            codigo_anio -= 1;
        }
    }

    // Calcular el resultado
    int resultado = (codigo_dia + codigo_mes + codigo_anio) % 7;

    return dias_semana[resultado - 1];
}

void updateLCDCronometro(){
	homeCursor();    
    writeScreen(1, "     " + fechaDS3232(Date) + "       ");
	writeScreen(2, "ALARM        T = " + ds3232_temperature()+"C"); 
	// Print Alarm1 time
    Serial3.write(0xFE);
    Serial3.write(148 + 0);

    sprintf(buffer, "%02d:%02d%02d", Date.alarm1Hour, Date.alarm1Minute, Date.alarm1Seconds);
    Serial3.write(buffer);

    // Print alarm active or not
    Serial3.write(0xFE);
    Serial3.write(148 + 7);

    if (alarmStatus.alarm1Active)
    {
        Serial3.write("*");
    }
    else
    {
        Serial3.write(" ");
    }

    // Print date static
    Serial3.write(0xFE);
    Serial3.write(148 + 10);

    Serial3.write("DAYDDMMMYY");

    // Print Alarm2 time
    Serial3.write(0xFE);
    Serial3.write(212 + 0);

    sprintf(buffer, "%02d:%02d", Date.alarm2Hour, Date.alarm2Minute);
    Serial3.write(buffer);

    // Print alarm active or not
    Serial3.write(0xFE);
    Serial3.write(212 + 5);

    if (alarmStatus.alarm2Active)
    {
        Serial3.write("*");
    }
    else
    {
        Serial3.write(" ");
    }

    // Print Date
    Serial3.write(0xFE);
    Serial3.write(212 + 13);

    sprintf(buffer, "%s%02d%s%02d",calcular_dia_semana(Date.date, Date.month - 1, Date.year),  Date.date, months[Date.month - 1], Date.year);
    Serial3.write(buffer);
}	

void guardarEnMemoria(int segundos, int minutos, int horas, int address) {
  // Escribe los minutos en la memoria I2C
  I2C_write_operation(((segundos /10)*16 + (segundos%10)), address);
			   
  // Escribe los dos puntos ":"
  I2C_write_operation(':', address + 2);

  // Escribe los minutos en la memoria I2C
  I2C_write_operation(((minutos /10)*16 + (minutos%10)), address+3);

  // Escribe los dos puntos ":"
  I2C_write_operation(':', address + 5);

  // Escribe los minutos en la memoria I2C
  I2C_write_operation(((horas /10)*16 + (horas%10)), address+6);
}


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
