/**
   José Alfredo Casal Salgueiro
   ID: 00000203970
   Proyecto_empotrados.io
*/

//Se importan las librerias necesarias.
//Libreria para el serial.
#include <string.h>
//Libreria para una mejor gestión del buzzer
#include<EasyBuzzer.h>
//Libreria para el control del botón.
#include <Bounce2.h>

//Se declaran los pines a utilizar
const unsigned int PIN_LED1 = 12;
const unsigned int PIN_LED2 = 13;
const unsigned int PIN_BOTON = 8;
const int EchoPin = 5;
const int TriggerPin = 6;
int sensorPin = A0;

//Se instancian los objetos y variables necesarios:
//Objeto para el botón.
Bounce debouncer = Bounce();
//Variable para el rango de edad.
unsigned int rangoEdad = 0;


//Se declaran las banderas a utilizar
//La bandera_1 se encarga de autorizar si la temperatura obtenida por el sensor infrarrojo es aceptable.
bool bandera_1 = false;
/*- - - - SE REALIZARÁN PRUEBAS CON UNA VARIABLE GLOBAL PRIMERO - - - -
  //La banderaEdad1 indica que el objetivo es un menor de edad.
  bool banderaEdad1 = true;
  //La banderaEdad2 indica que el objetivo es un un mayor de edad.
  bool banderaEdad2 = false;
  //La banderaEdad3 indica que el objetivo es una persona de la tercera edad.
  bool banderaEdad3 = false;
*/
//La banderaError se encarga de indicar si hay un error por parte del sistema.
bool banderaError = false;

void setup() {
  //Se establece el puerto serial, en este caso el 9600.
  Serial.begin(9600);
  //Se establece el pin y el tipo para el botón.
  pinMode(PIN_BOTON, INPUT);
  //Se le establece al pin del botón al objeto debouncer.
  debouncer.attach(PIN_BOTON);
  //Se establece un intervalo de espera en ms para eliminar el ruido.
  debouncer.interval(25);
  //Se establece el pin del zumbador para el easyBuzzer.
  EasyBuzzer.setPin(11);
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  //Se establece los leds a utilizar
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
}

void loop() {
  debouncer.update();
  if (debouncer.fell())
  {
    controlLedsEdad();
  }
  char comando[2];
  if (Serial.available() > 0)
  {
    // Lee a lo mas 5 caracteres del puero serie o hasta que se
    // presione la tecla Enter y los guarda en el arreglo comando.
    int n = Serial.readBytesUntil('\n', comando, 2);
    // Todas las cadenas en C/C++ terminan en el carácter de fin// de cadena, ‘\n’.
    comando[n] = '\0';
    // Escribe el comando al puerto serie
//    Serial.println(comando);
    if (!strcmp(comando, "tm"))
    {
      controlTemperaturaObjeto(controlDistancia());
    }
    if(!strcmp(comando, "st"))
    {
     Serial.println(rangoEdad);
    }
  }
  EasyBuzzer.stopBeep();
}

/**
   Función encargada de imprimir la temperatura en el Serial
*/
void controlTemperaturaObjeto(bool bandera) {
  if (bandera == true)
  {
    float tmpC = leerTemperaturaObjeto(sensorPin);
    if (rangoEdad == 0)
    {
      if (tmpC >= 36.6 and tmpC <= 37.2)
      {
//        Serial.print("La temperatura es: ");
        Serial.println(tmpC);
        bandera_1 = true;
        controlLedsSemaforo();
      }
      else
      {
//        Serial.print("La temperatura es: ");
        Serial.println(tmpC);
        controlLedsSemaforo();
      }
    }
    else if (rangoEdad == 1)
    {
      if (tmpC >= 36.1 and tmpC <= 37.5)
      {
//        Serial.print("La temperatura es: ");
        Serial.println(tmpC);
        bandera_1 = true;
        controlLedsSemaforo();
      }
      else
      {
//        Serial.print("La temperatura es: ");
        Serial.println(tmpC);
        controlLedsSemaforo();
      }
    }
    else if (rangoEdad == 2)
    {
      if (tmpC >= 35.2 and tmpC <= 36.2)
      {
//        Serial.print("La temperatura es: ");
        Serial.println(tmpC);
        bandera_1 = true;
        controlLedsSemaforo();
      }
      else
      {
//        Serial.print("La temperatura es: ");
        Serial.println(tmpC);
        controlLedsSemaforo();
      }
    }
  }
  else
  {
//    Serial.println("Disntancia no valida");
  }
}

/**
   Función encargada de leer la temperatura del objetivo y devolverla en un valor flotante.
*/
float leerTemperaturaObjeto(int sensorPin) {
  //Se obtiene la lectura directamente del ADC del Arduino
  int lectura = analogRead(sensorPin);

  // Se escala la lectura al valor de voltaje del arduino
  float voltaje = lectura * 5.0;

  // Se determina el valor de voltaje dividiendo el valor entre 1024 porque es el rango de valores del ADC (10 bits) en el arduino
  voltaje /= 1024.0;

  // Se calcula la temperatura a partir del valor de voltaje
  // 10 mv por grado con un offset de 500 mV para temperaturas negativas
  float temperaturaC = (voltaje - 0.5) * 100 ;
  return temperaturaC;
}

/**
   Función encargada de gestionar si la distancia del "objeto" esta en un rango de 0 a 10 cm.
*/
bool controlDistancia() {
  int distancia = calculaDistancia(TriggerPin, EchoPin);
  if (distancia >= 0 and distancia <= 10)
  {
    //Serial.print("Distancia: ");
//    Serial.println(distancia);
    return true;
  }
  else
  {
    return false;
  }
}

/**
   Función encargada de obtener la distancia de un "objeto" mediante el sensor ultrasonico y devolverla en centimetros.
*/
int calculaDistancia(int TriggerPin, int EchoPin) {
  long duration, distanceCm;

  digitalWrite(TriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);

  duration = pulseIn(EchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos

  distanceCm = duration * 10 / 292 / 2;  //convertimos a distancia, en cm
  return distanceCm;
}

/**
   Función que se encarga de que el buzzer realize dos pitidos en una frecuencia de 2000.
   para indicar que la temperatura del "objeto" es aceptable.
*/
void zumbaAceptacion() {
  EasyBuzzer.singleBeep(700, 500);
}
/**
   Función que se encarga de que el buzzer realize 1 pitido en una frecuencia de 4000.
   para indicar que la temperatura del "objeto" no es aceptable.
*/
void zumbaNegacion() {
  EasyBuzzer.singleBeep(200, 500);
}

/**
   Función encargada de gestionar los leds del semáforo.
*/
void controlLedsSemaforo() {
  if (bandera_1 == true)
  {
    /*Que encienda el led verde*/
    digitalWrite(PIN_LED1, HIGH);
    zumbaAceptacion();
    delay(3000);
    digitalWrite(PIN_LED1, LOW);
    bandera_1 = false;

  }
  else
  {
    /*Que encienda el led rojo*/
    digitalWrite(PIN_LED2, HIGH);
    zumbaNegacion();
    delay(3000);
    digitalWrite(PIN_LED2, LOW);
  }
}
/**
   Función encargada de gestionar los leds de el rango de edad.
*/
void controlLedsEdad() {
  rangoEdad += 1;
  if (rangoEdad == 3)
  {
    rangoEdad = 0;
  }
  /*  if()
    {
      Logica para prender los leds
    }
  */
}

void reiniciaBanderas() {

}
