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
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  
  // Menú
  Serial.println("Menu:");
  Serial.println("1. Parpadeo de las unidades");
  Serial.println("2. Parpadeo de las decenas");
  Serial.println("3. Parpadeo de las centenas");
  Serial.println("4. Parpadeo de las unidades de millar");
  Serial.println("5. Parpadeo secuencial con todos los digitos");
  Serial.println("6. Seleccion del caracter hexadecimal (0-F) a visualizar en el display");
  Serial.print("Entrar opcion: ");
  
  // Esperar la entrada del usuario
  while (!Serial.available()) {
    // Esperar a que el usuario ingrese una opción
  }
  
  char opcion = Serial.read();
  Serial.println(opcion);
  
  switch (opcion) {
    case '1':
      parpadeoUnidades();
      break;
    case '2':
      parpadeoDecenas();
      break;
    case '3':
      parpadeoCentenas();
      break;
    case '4':
      parpadeoUnidadesDeMillar();
      break;
    case '5':
      parpadeoSecuencial();
      break;
    case '6':
      seleccionarCaracterHexadecimal();
      break;  
  }
}

// Función para encender las unidades
void parpadeoUnidades() {
  int d[4] = {D1, D2, D3, D4};

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(d[j], (i == 2 && j == 3) ? LOW : HIGH);
    }
    delay(500);
    digitalWrite(D4, LOW);
    delay(500);
    digitalWrite(D4, HIGH);
  }
}

// Función para encender las decenas
void parpadeoDecenas() {
  int d[4] = {D1, D2, D3, D4};

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(d[j], (i == 2 && j == 2) ? LOW : HIGH);
    }
    delay(500);
    digitalWrite(D3, LOW);
    delay(500);
    digitalWrite(D3, HIGH);
  }
}

// Función para encender las centenas
void parpadeoCentenas() {
  int d[4] = {D1, D2, D3, D4};

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(d[j], (i == 2 && j == 1) ? LOW : HIGH);
    }
    delay(500);
    digitalWrite(D2, LOW);
    delay(500);
    digitalWrite(D2, HIGH);
  }
}

// Función para encender las unidades de millar
void parpadeoUnidadesDeMillar() {
  int d[4] = {D1, D2, D3, D4};

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(d[j], (i == 2 && j == 0) ? LOW : HIGH);
    }
    delay(500);
    digitalWrite(D1, LOW);
    delay(500);
    digitalWrite(D1, HIGH);
  }
}

// Función para el parpadeo secuencial de los dígitos
void parpadeoSecuencial() {
  int d[4] = {D1, D2, D3, D4};

  // Parpadeo secuencial de todos los dígitos avanzando
  for (int i = 0; i < 4; i++) {
    digitalWrite(d[i], HIGH);
    delay(500);
    digitalWrite(d[i], LOW);
  }

  // Espera medio segundo
  delay(500);

  // Parpadeo secuencial de todos los dígitos retrocediendo
  for (int i = 3; i >= 0; i--) {
    digitalWrite(d[i], HIGH);
    delay(500);
    digitalWrite(d[i], LOW);
  }
}

// Función para seleccionar un carácter hexadecimal y mostrarlo en el display
void seleccionarCaracterHexadecimal() {
  Serial.println("Seleccion del caracter hexadecimal (0-F) a visualizar en el display");
  Serial.println("Ingrese un caracter hexadecimal (0-F):");

  while (!Serial.available()) {
    // Esperar a que el usuario ingrese un carácter hexadecimal
  }

  char caracterHexadecimal = Serial.read();
  Serial.println(caracterHexadecimal);

  // Verificar si el carácter es un dígito hexadecimal válido (0-9, A-F)
  if ((caracterHexadecimal >= '0' && caracterHexadecimal <= '9') ||
      (caracterHexadecimal >= 'A' && caracterHexadecimal <= 'F')) {
    // Convertir el carácter hexadecimal en un valor decimal
    int valorDecimal;

    if (caracterHexadecimal >= '0' && caracterHexadecimal <= '9') {
      valorDecimal = caracterHexadecimal - '0';
    } else {
      valorDecimal = caracterHexadecimal - 'A' + 10;
    }

    // Mostrar el valor decimal en el display de 7 segmentos
    mostrarDecimalEnDisplay(valorDecimal);
  } else {
    Serial.println("Caracter hexadecimal no valido. Ingrese un caracter valido (0-9, A-F).");
  }
}

// Función para mostrar un número decimal en el display
void mostrarDecimalEnDisplay(int valorDecimal) {
  // Apagar todos los dígitos del display
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);

  // Tabla de conversión para los dígitos hexadecimales (0-9, A-F)
  int tablaConversion[16] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // B
    0x39, // C
    0x5E, // D
    0x79, // E
    0x71  // F
  };

  if (valorDecimal >= 0 && valorDecimal <= 15) {
    // Mostrar el número en el dígito actual del display
    PORTA = tablaConversion[valorDecimal];
  } else {
    Serial.println("Numero fuera de rango (0-F).");
  }
}

// Función para apagar el display
void apagarDisplay() {
  // Apagar todos los dígitos del display
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
}
