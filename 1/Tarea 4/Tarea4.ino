/* Práctica Lab1: Interfaces paralelos
 *
 * Fichero: 22-23_lab1_base.pdsprj
 * Creado: 21 septiembre 2023
 * Autor: Romen Adama Caetano Ramirez
 */

// Definición de pines para pulsadores
#define PRIGHT  30    //	Pulsador right, conectado al pin 30
#define PDOWN   31    //	Pulsador down, conectado al pin 31
#define PLEFT   32    //	Pulsador left, conectado al pin 32
#define PENTER  33    //	Pulsador entrar, conectado al pin 33
#define PUP     34    //	Pulsador up, conectado al pin 34
#define SPEAKER 37    // 	Speaker, conectado al pin 37

// Definición de pines para los cátodos de display
#define D4 49    // El pin 49 controla el cátodo común del dígito de las unidades (D4)
#define D3 48    // El pin 48 controla el cátodo común del dígito de las decenas (D3)
#define D2 47    // El pin 47 controla el cátodo común del dígito de las centenas (D2)
#define D1 46    // El pin 46 controla el cátodo común del dígito de las unidades de millar (D1)

// Mapeo matricial para las teclas del teclado
char teclado_map[][3] = { {'1','2','3'},
                        {'4','5','6'},
                        {'7','8','9'},
                        {'*','0','#'}};

// Definición de los patrones de los dígitos para el display 7 segmentos
unsigned char tabla_7segm[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};

int contador = 0;                 // Valor del contador
volatile int digito = 0;          // Dígito actual del display
unsigned long time_now = 0;       // Tiempo actual
unsigned long time_before = 0;    // Tiempo anterior

int tipoCuenta = 1; // Variable para el tipo de cuenta del contador. Si tipoCuenta = 1, cuenta de 1 en 1. Si tipoCuenta = 2, cuenta de 2 en 2

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

  cli(); // Deshabilita las interrupciones globales
  EICRA |= (1 << ISC31) | (1 << ISC30);
  EIMSK |= (1 << INT3); // Habilita las interrupciones de INT3
  sei(); // Habilita nuevamente las interrupciones globales

  setNumber(0, D4);  // Inicializar el display con el valor 0
  
  Serial.println("---Menu---");
  Serial.println("1. Se usan los dos dígitos superiores para visualizar el contador");
  Serial.println("2. Se usan los dos dígitos inferiores para visualizar el contador");
  tone(18, 200);
}

ISR(INT3_vect){
   // Cuerpo de la subrutina de servicio de interrupción a ejecutar cuando
   // se interrumpa por el pin 18 (INT3)         
   PORTL = 0xFF;

   switch(digito){
      case 0:
	 // Visualizamos unidades	 
	 setNumber(contador % 10, D4);
	 digito++;
	 break;
      case 1:
	// Visualizamos decenas	
	setNumber((contador / 10) % 10, D3);
	digito++;
	break;
      case 2:
	 // Visualizamos centenas
	 setNumber(int(contador/100)%10, D2);	 
	 digito++;
	 break;
      case 3:
	 // Visualizamos unidades de millar
	 setNumber(int(contador/1000), D1);
	 digito = 0;
	 break;
   } 
}

void loop() {  
   boolean pup_pressed = (digitalRead(PUP) == 0);
   boolean pdown_pressed = (digitalRead(PDOWN) == 0);
   boolean pcenter_pressed = (digitalRead(PENTER) == 0);
   boolean pleft_pressed = (digitalRead(PLEFT) == 0);
   boolean pright_pressed = (digitalRead(PRIGHT) == 0);
   boolean pulled = pup_pressed || pdown_pressed || pcenter_pressed || pleft_pressed || pright_pressed;
   
   if(pulled){       
      time_now = millis();      
      if(time_now - time_before > 500){	 
         time_before = time_now;
         noInterrupts(); // Deshabilita las interrupciones para evitar problemas de concurrencia
         if(pdown_pressed){
            if (contador == 0) {
               contador = 9999; // Establece el contador en 999 cuando es igual a 0 y se presiona "pdown"
            } else {
               contador = contador - tipoCuenta;
            }
         } else if(pup_pressed && contador < 9999){
            contador = contador + tipoCuenta;	    
         } else if(pleft_pressed){
            tipoCuenta = 1;
         } else if(pright_pressed){
            tipoCuenta = 2;
         } else{
            contador = 0;
         }
         digitalWrite(SPEAKER, 1);
         tone(SPEAKER,  100, 100);	
         digitalWrite(SPEAKER, 0);
         interrupts(); // Habilita las interrupciones nuevamente
      }
    }
}

void setNumber(int number, int pin){      
   PORTA = tabla_7segm[number];   
   digitalWrite(pin, 0);
}
