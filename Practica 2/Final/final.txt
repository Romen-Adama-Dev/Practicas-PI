/* Práctica 2: Timers en el uc Atmega 2560
 * 
 * Descripción: Aplicación correspondiente a la práctica 2 (frecuencímetro)
 *
 *
 * Fichero: 	22-23_lab2_prs.pdsprj
 * Creado: 	16/10/2023
 * Autor:		Romen Adama Caetano Ramirez
*/

#include <math.h>

// Definición para pulsadores
// PORTC: PC[7:0] --> PINES 30-31-32-33-34-35-36-37

#define PRIGHT  	30    //	PC[7] pulsador right 
#define PDOWN   	31    //	PC[6] pulsador down
#define PLEFT   	32    //	PC[5] pulsador left
#define PENTER 	33    //	PC[4] pulsador entrar
#define PUP     	34    //	PC[3] pulsador up
#define SPEAKER 	37    // 	PC[0] speaker 

/*
// Definiciones para teclado/cátodos de display (para el manejo de los 4 dígitos del display de forma entrelazada)
// PORTL: PL[7:0] --> PINES 42-43-44-45-46-47-48-49
//														 R1-R2-R3-R4-D1-D2-D3-D4 --> FILAS_TECLADO (R) | CÁTODOS DISPLAY (D)
#define D4    0xFE    	// 1111 1110 visualiza 	unidades
#define D3    0xFD    	// 1111 1101 		"				decenas
#define D2    0xFB    	// 1111 1011 		"				centenas
#define D1    0xF7    	// 1111 0111 		"				millares
#define DOFF  0xFF    	// 1111 1111 todos apagados: todos los cátados comunes a "1"
#define DON   0xC0    	// 1111 0000 todos encendidos: todos los cátodos comunes a "0"
*/

//Otra forma de definición de señales de cátodos para manejo de forma individual 
#define D4 49   // El pin 49 controla el cátodo común del dígito de las unidades (D4)
#define D3 48   // El pin 48 controla el cátodo común del dígito de las decenas (D3)
#define D2 47   // El pin 47 controla el cátodo común del dígito de las centenas (D2)
#define D1 46   // El pin 46 controla el cátodo común del dígito de las unidades de millar (D1)


// Mapeo matricial para las teclas del teclado
char teclado_map[][3] = {  {'1','2','3'},
					  {'4','5','6'},
					  {'7','8','9'},
					  {'*','0','#'}  };

//Practica 1 Variables
					  
// Tabla de segmentos para mostrar dígitos en el display de 7 segmentos
unsigned char tabla_7segm[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};

volatile int contador = 0;
volatile int digito = 0;
unsigned long time_now = 0;
unsigned long time_before = 0;

unsigned long time_now2 = 0;
unsigned long time_before2 = 0;

#define STemp 54

String buffer = "";
int vCount = 1;     // Variable para el tipo de cuenta del contador (1 o 2)
int modo = 1;       // Variable para guardar el modo de visualización
volatile int temp; // Variable para guardar la temperatura medida con el sensor
boolean mTemp;

//Practica 2 

#define OC3A 5
#define OC3B 2
#define OC3C 3

unsigned long Told = 0;
unsigned long Tnew = 0;
unsigned long Periodo = 0; // guarda el periodo calculado
unsigned long Frecuencia = 0; // guarda la frecuencia calculada

void setup() {
  // Inicialización del canal serie para comunicación
  Serial.begin(9600);

  // Configuración de puertos y pines
  DDRA = 0xFF;       		// Programar PORTA como salida (control del display de 7 segmentos)
  PORTA = 0xFF;     		// Todos los segmentos a "1": encendidos

  DDRL = 0x0F;       			// Configuración de filas_teclado (R) y cátodos display (D)
  PORTL = 0xFF;     			// Activación de resistencias pull-up en las entradas de las filas de teclado y display

  DDRC = 0x01;       		// Configuración del PORTC
  PORTC = 0xFE;     		// Activación de resistencias pull-up en todas las entradas excepto PC0
	
	// Programamos los pines OC3A, OC3B y OC3C como salida
	pinMode(OC3A, OUTPUT);
	pinMode(OC3B, OUTPUT);
	pinMode(OC3C, OUTPUT);
		
	// reset timers
	TCCR3A = B01000000;
	TCCR3B = B01001010; // cada vez que se produce un flanco de subida se guarda el contenido de TCNT3 en el registro ICR3
	TCCR3C= 0;
	TCNT3 = 0;
	OCR3A = 9999;

	TIMSK3  |= (1 << OCIE3A);

	 // Menú de opciones
	 Serial.println("---Menu Practica 1---");
	 Serial.println("Contador de 3 digitos");
	 Serial.println("PUP: Incrementa el contador");
	 Serial.println("PDOWN: Decrementa el contador");
	 Serial.println("PENTER: Puesta a cero (reset) del contador");
	 Serial.println("PLEFT: El contador incrementara o decrementara su cuenta de 1 en 1");
	 Serial.println("PRIGHT: El contador incrementara o decrementara su cuenta de 2 en 2.");
	 Serial.println("Inicializacion del contador entre 000 y 999 mediante el teclado de 4x3");
	 Serial.println("--------------------------------------------------------------------------------------------------------------------");
	 Serial.println("--Modos--");
	 Serial.println("1.- Modo normal de visualizacion (Turnomatic): ");
	 Serial.println("2.- Modo reducido-inferior de visualizacion (dos digitos): OFF-OFF-decenas-unidades");
	 Serial.println("3.- Modo reducido-superior de visualizacion (dos digitos): decenas-unidades-OFF-OFF");
	 Serial.println("4.- Frecuencimetro");
}

void loop() {  
   // Lectura de pulsadores
  boolean pup_pressed = (digitalRead(PUP) == 0);
  boolean pdown_pressed = (digitalRead(PDOWN) == 0);
  boolean pcenter_pressed = (digitalRead(PENTER) == 0);
  boolean pleft_pressed = (digitalRead(PLEFT) == 0);
  boolean pright_pressed = (digitalRead(PRIGHT) == 0);
  boolean pulled = pup_pressed || pdown_pressed || pcenter_pressed || pleft_pressed || pright_pressed;

  if (pulled) {
    // Control de rebotes y actualización del contador
    time_now = millis();
    if (time_now - time_before > 500) {
      time_before = time_now;
      if (pup_pressed && contador + vCount < 1000) {
        contador = contador + vCount;
      }
      else if (pdown_pressed && contador - vCount >= 0) {
        contador = contador - vCount;
      } else if (pleft_pressed) {
        vCount = 1;
      } else if (pright_pressed) {
        vCount = 2;
      } else if(pdown_pressed && contador == 0 && vCount == 1){
	 contador = 999;
      } else if(pdown_pressed && contador == 0 && vCount == 2){
	 contador = 998;
      } else if(pdown_pressed && contador == 1 && vCount == 2){
	 contador = 999;
      } else if(pup_pressed && contador == 999 && vCount == 1){
	 contador = 0;
      } else if(pup_pressed && contador == 998 && vCount == 2){
	 contador = 0;
      } else if(pup_pressed && contador == 999 && vCount ==2){
	 contador = 1;
      } else {
        contador = 0;
      }
    }
    // Generación de señal acústica
    digitalWrite(SPEAKER, 1);
    tone(SPEAKER,  300, 100);
    delay(50);
    digitalWrite(SPEAKER, 0);
  }
        
    // Lectura de comandos desde el canal serie
    Serial.available();    
    if(Serial.available() > 0){
		int opcion = Serial.read() - 48;
		if(opcion >= 1 && opcion <= 4){
			modo = opcion; // cambio de modo 		
			if(modo == 4){
				TIMSK3  |= (1 << ICIE3);				
			} else{
				TIMSK3  = TIMSK3  = TIMSK3  & B11011111;
			}
		}
		
    }
    
     // Inicialización del contador desde el buffer de entrada
     if (buffer[buffer.length() - 1] == '#') {
       int counter = 0;
       int potencia = 1;
       int numero = 0;
       for (int i = buffer.length() - 2; i >= 0 && counter < 3; i--, counter++, potencia *= 10) {
	 numero = numero + int(buffer[i] - 48) * potencia;
       }
       contador = numero;
       buffer = "";
     }
   
      if(modo == 4){
       // frecuencia en hz
       Frecuencia = int(1.0 / (Periodo *0.5*pow(10, -6)));
       }
}

/*
  Descripción: Este programa maneja un teclado y un display de siete segmentos.
*/

// Función para leer el teclado y actualizar el buffer de lectura
void teclado(int digit) {
  /*
    Descripción: Lee el estado del PINL para verificar si se ha seleccionado alguna fila del teclado
    y actualiza el buffer de lectura si se cumple la condición de tiempo.
    
    Parámetros:
      - digit: La columna del teclado.
  */
  
  int val = PINL >> 4;
  int fila = getRow(val);
  
  if (fila == -1) return;
  
  time_now2 = millis();
  if (time_now2 - time_before2 > 300) {
    time_before2 = time_now2;
    buffer += teclado_map[fila][digit]; // Actualizamos el buffer
   }      
}

// Función para determinar la fila seleccionada en el teclado
int getRow(int val) {
  /*
    Descripción: Verifica el número pasado (leído en PINL) para determinar si se ha seleccionado alguna fila.
    
    Parámetros:
      - val: El número pasado.
    
    Devuelve: El número de fila si se ha seleccionado alguna, o -1 si no se ha seleccionado ninguna.
  */
  
  switch (val) {
    case 7:
      // Caso 7: La fila 0 está seleccionada en el teclado.
      return 0;
    case 11:
      // Caso 11: La fila 1 está seleccionada en el teclado.
      return 1;
    case 13:
      // Caso 13: La fila 2 está seleccionada en el teclado.
      return 2;
    case 14:
      // Caso 14: La fila 3 está seleccionada en el teclado.
      return 3;
    default:
      // Si no se encuentra ninguna fila seleccionada, retornamos -1.
      return -1;
  }
}

void showNumber(int number, int pin, boolean punto = false);

void showNumber(int number, int pin, boolean punto){       
   /*
   Con este método ponemos en el display de siete segmentos el número solicitado, y ponemos un 0 en el pin correspondiente.
   • Parámetro number: el número a mostrar.
   • Parámetro pin: el pin en concreto (D4, D3, D2, D1...)
   • Parámetro punto: si se activa el punto en el número solicitado (true) o no (false)	
   */
   char segm = tabla_7segm[number];
   if(punto){
      segm |= B10000000;
   }
   PORTA = segm;
   digitalWrite(pin, 0);
}

ISR(TIMER3_COMPA_vect)
{   
   // Cuerpo de la subrutina de servicio de interrupción para INT3 (refrescar la pantalla)
	PORTL = 0xFF;   
	int pin;   
	int numeroAMostrar;
	boolean punto = false;
	if(modo == 4){
	// Cada tick es de 0.5 microsegundos = 0.5·10^-6 segundos			
		numeroAMostrar = Frecuencia;
		if(numeroAMostrar >= 10000){
			// Si la frecuencia es mayor o igual a 10K, la mostramos dividida			
			numeroAMostrar /= 10;
			punto = true;
		}
	} else{
		numeroAMostrar = contador;
	}      	
	switch(digito){
		case 0:
			// Visualizamos unidades	 
			pin = D4;
			if(modo == 3) pin = D2;	 	 
			numeroAMostrar = numeroAMostrar % 10;		
			showNumber(numeroAMostrar,  pin);	  
			teclado(digito);
			digito++;
			break;
		case 1:
			// Visualizamos decenas	
			pin = D3;
			if(modo == 3)  pin = D1;
			numeroAMostrar = (numeroAMostrar / 10) % 10;			
			showNumber(numeroAMostrar,  pin);
			teclado(digito);
			digito++;
			break;
		case 2:
			// Visualizamos centenas
			if(modo == 1 | modo == 4){	    	
				numeroAMostrar = int(numeroAMostrar/100)%10;
				showNumber(numeroAMostrar, D2, punto);		
			}
			teclado(digito);
			if(modo == 4){
				digito++;
			} else{
				digito = 0;
			}	 
			break;	   
		case 3:
			// Visualizamos unidades de millar				
			numeroAMostrar = int(numeroAMostrar / 1000) % 10;		
			showNumber(numeroAMostrar, D1);		
			digito = 0;
			break;
		}
}

ISR(TIMER3_CAPT_vect){
	/*
	 ISR que se ejecuta cuando se produce un flanco de subida en el pin Input Capture (ICP3)
	*/
	Told = Tnew;	
	Tnew = ICR3;	
	if(long(Tnew-Told) >= 0){
		Periodo = Tnew - Told;
	} else{
		Periodo = (OCR3A - Told) + Tnew;
	}
}
