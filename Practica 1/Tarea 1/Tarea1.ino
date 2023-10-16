/* Práctica Lab1: Interfaces paralelos
 *
 * Fichero: 22-23_lab1_base.pdsprj
 * Creado: 12 septiembre 2023
 * Autor: Romen Adama Caetano Ramirez
 */

// Definición de pines para pulsadores
#define PRIGHT  30    // PC[7] - Pulsador right 
#define PDOWN   31    // PC[6] - Pulsador down
#define PLEFT   32    // PC[5] - Pulsador left
#define PENTER  33    // PC[4] - Pulsador entrar
#define PUP     34    // PC[3] - Pulsador up
#define SPEAKER 37    // PC[0] - Altavoz

// Otra forma de definición de señales de cátodos para manejo de forma individual 
#define D4 49   				// El pin 49 controla el cátodo común del dígito de las unidades (D4)
#define D3 48   				// El pin 48 controla el cátodo común del dígito de las decenas (D3)
#define D2 47   				// El pin 47 controla el cátodo común del dígito de las centenas (D2)
#define D1 46   				// El pin 46 controla el cátodo común del dígito de las unidades de millar (D1)

// Mapeo matricial para las teclas del teclado
char teclado_map[][3] = { {'1','2','3'},
                        {'4','5','6'},
                        {'7','8','9'},
                        {'*','0','#'}};

// Declaración de las duraciones y frecuencias de la señal acústica
#define DURACION_PITIDO 100  // Duración del pitido en milisegundos
#define FRECUENCIA_PITIDO 1000  // Frecuencia del pitido en Hz

void setup() {
  Serial.begin(9600);
  
  // Configuración de pines
  DDRA = 0xFF;      // Puerto A como salida para controlar el display 7 segmentos
  PORTA = 0xFF;     // Todos los segmentos apagados inicialmente
  DDRL = 0x0F;      // Puerto L, filas del teclado, como entradas; cátodos display como salidas
  PORTL = 0xFF;     // Activación de resistencias de pull-up en las entradas del teclado
  DDRC = B00000001; // Configura PC0 como salida (altavoz); los demás pines de PORTC como entrada
  PORTC = B11111110; // Activación de resistencias de pull-up en los pines de PORTC excepto PC0
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
}

void loop() {
  // Variable para almacenar el dígito actual
  int digitoActual = 4;  // Comenzamos con el dígito de las unidades (D4)

  while (true) {
    // Enciende el dígito actual y apaga los demás
    for (int i = 1; i <= 4; i++) {
      digitalWrite(D1, i == 4);
      digitalWrite(D2, i == 3);
      digitalWrite(D3, i == 2);
      digitalWrite(D4, i == 1);
      delay(500);  // Espera medio segundos
      generarPitido();  // Genera la señal acústica
    }

    // Cambia al siguiente dígito en secuencia
    digitoActual = (digitoActual % 4) + 1;
  }
}

// Función para generar una señal acústica
void generarPitido() {
  tone(SPEAKER, FRECUENCIA_PITIDO, DURACION_PITIDO); // Genera un tono en el altavoz
  delay(DURACION_PITIDO + 50);  // Espera durante la duración del tono más un pequeño espacio
  noTone(SPEAKER);  // Apaga el altavoz
}
