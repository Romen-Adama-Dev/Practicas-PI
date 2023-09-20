/* Práctica Lab1: Interfaces paralelos
 *
 * Fichero: 22-23_lab1_base.pdsprj
 * Creado: 20 septiembre 2023
 * Autor: Romen Adama Caetano Ramirez
 */

// Definición para pulsadores
// PORTC: PC[7:0] --> PINES 30-31-32-33-34-35-36-37
#define PRIGHT  30    //	PC[7] pulsador right 
#define PDOWN   31    //	PC[6]		"" 		down
#define PLEFT   32    //	PC[5]		"" 		left
#define PENTER 	33    //	PC[4]		"" 		entrar
#define PUP     34    //	PC[3]		"" 		up
#define SPEAKER 37    // 	PC[0]					speaker 

/*
// Definiciones para teclado/cátodos de display (para el manejo de los 4 dígitos del display de forma entrelazada)
// PORTL: PL[7:0] --> PINES 42-43-44-45-46-47-48-49
//														 R1-R2-R3-R4-D1-D2-D3-D4 --> FILAS_TECLADO (R) | CÁTODOS DISPLAY (D)
#define D4    0xFE    // 1111 1110 visualiza 	unidades
#define D3    0xFD    // 1111 1101 		"				decenas
#define D2    0xFB    // 1111 1011 		"				centenas
#define D1    0xF7    // 1111 0111 		"				millares
#define DOFF  0xFF    // 1111 1111 todos apagados: todos los cátados comunes a "1"
#define DON   0xC0    // 1111 0000 todos encendidos: todos los cátodos comunes a "0"
*/

//Otra forma de definición de señales de cátodos para manejo de forma individual 
#define D4 49   				// El pin 49 controla el cátodo común del dígito de las unidades (D4)
#define D3 48   				// El pin 48 controla el cátodo común del dígito de las decenas (D3)
#define D2 47   				// El pin 47 controla el cátodo común del dígito de las centenas (D2)
#define D1 46   				// El pin 46 controla el cátodo común del dígito de las unidades de millar (D1)


// Mapeo matricial para las teclas del teclado
char teclado_map[][3] = { {'1','2','3'},
                        {'4','5','6'},
                        {'7','8','9'},
                        {'*','0','#'}};

// Definición de los patrones de los dígitos para el display 7 segmentos
unsigned char digitos[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};

int contador = 0;                 // Valor del contador
unsigned long time_now = 0;       // Tiempo actual
unsigned long time_before = 0;    // Tiempo anterior

void setup() {
  Serial.begin(9600);  // Inicialización del canal serie

  // Configuración de pines de salida para el display 7 segmentos
  DDRA = 0xFF;
  PORTA = 0xFF;  // Todos los segmentos apagados

  // Configuración de pines para el teclado y el display
  DDRL = 0x0F;    // Filas del teclado como entradas, cátodos del display como salidas
  PORTL = 0xFF;   // Activación de resistencias de pull-up en las entradas del teclado

  // Configuración de puertos para pulsadores
  DDRC = 0x01;    // Todos los bits de entrada, excepto PC0 que es de salida (speaker)
  PORTC = 0xFE;   // Activación de resistencias de pull-up en todos los pines de PC, excepto PC0

  setNumber(0, D4);  // Inicializar el display con el valor 0
}

void loop() {
  updateScreen();  // Actualizar el display entrelazado

  // Verificar si se ha pulsado alguno de los botones
  boolean pup_pressed = (digitalRead(PUP) == 0);
  boolean pdown_pressed = (digitalRead(PDOWN) == 0);
  boolean pcenter_pressed = (digitalRead(PENTER) == 0);

  if (pup_pressed || pdown_pressed || pcenter_pressed) {
    time_now = millis();
    if (time_now - time_before > 500) {
      time_before = time_now;

      // Actualizar el contador según el botón pulsado
      if (pup_pressed && contador < 99) {
        contador++;
      } else if (pdown_pressed && contador == 0) {
        contador = 99;
      } else if (pdown_pressed && contador > 0) {
        contador--;
      } else {
        contador = 0;
      }

      // Emitir un pitido corto
      digitalWrite(SPEAKER, HIGH);
      delay(100);  // Duración del pitido
      digitalWrite(SPEAKER, LOW);
    }
  }
}

// Función para actualizar el display entrelazado
void updateScreen() {
  String scontador = String(contador);
  int slength = scontador.length();

  for (int j = 0; j < 4; j++) {
    int numpin = 49 - j;
    if (slength > j) {
      // Verificar si hay unidades/decenas/centenas/millares
      int number = scontador[slength - (j + 1)] - '0';
      setNumber(number, numpin);
    }
  }
}

// Función para mostrar un número en el display
void setNumber(int number, int pin) {
  PORTA = digitos[number];
  digitalWrite(pin, LOW);
  digitalWrite(pin, HIGH);
}