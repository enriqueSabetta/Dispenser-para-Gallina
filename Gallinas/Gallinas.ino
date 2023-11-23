/*
  ITES - Microcontroladores 2
  15/05/23 - Trabajo Practico Integrador

  Autor: Sabetta Enrique
  Profesor: Ing. Alejo Giles

  Enunciado: Dispenser Automatico de Alimento Balanceado para Gallinas

  Descripción del problema:

  En el marco de un proyecto de control para realizar un Dispenser Automatico de Alimento Balanceado para Gallinas, se solicita desarrollar un sistema utilizando Arduino
  
  El sistema permitirá controlar el motor de un dispenser de alimentos para gallinas, teniendo en cuenta que debara solo funcionar mientras sea de dias y cada ciertos 
  intervalos de tiempo, hasta tanto se haga de noche nuevamente.

  El sistema constará de los siguientes elementos: Motor 12v: que permitirá realizar la dispercion del alimento a baja velocidad ya que se necesita que el alimento 
  caiga en un comedero.
  Interfaz de usuario: La comunicación con el sistema se realizará a través del puerto serie (UART) del Arduino.
   Se utilizará un programa externo en la computadora para enviar comandos y recibir notificaciones desde el Arduino.
   Programa Arduino: El programa desarrollado en Arduino deberá gestionar los eventos sincrónicos y asincrónicos, recibir comandos por el puerto serie y controlar el motor 
   para realizar un test del motor.
  Requerimientos funcionales:Evento sincrónico: Cuando se reciba un comando por el puerto serie desde la interfaz de usuario para testear el funcionamiento del motor,
*/

#define MS_INTERVALO_LED_TEST            250 //Function LedTest()
#define LED_TEST                         13
#define CONFIG_LED_TEST                  pinMode(LED_TEST, OUTPUT)
#define ACTUALIZAR_LED_TEST(x)           digitalWrite(LED_TEST, x)

#define PIN_MOTOR                        11
#define CONFIG_PIN_MOTOR                 pinMode(PIN_MOTOR, OUTPUT)
#define ENCENDER_MOTOR                   analogWrite(PIN_MOTOR, 140)
#define APAGAR_MOTOR                     analogWrite(PIN_MOTOR, 0)

#define PIN_LDR                          A0
#define CONFIG_PIN_LDR                   pinMode(PIN_LDR, INPUT)
#define LEER_LDR                         analogRead(PIN_LDR)

#define PIN_ESTACION                     10
#define CONFIG_PIN_ESTACION              pinMode(PIN_ESTACION, INPUT)
#define LEER_PIN_ESTACION                digitalRead(PIN_ESTACION)
#define INVIERNO                         4
#define VERANO                           7

#define MENSAJE(m)                       Serial.println(m)
#define MSJCONC(mc)                      Serial.print(mc)

#define TPO_ENCENDIDO                    10 // tiempo de funcionamiento del motor en segundos
#define TPO_APAGADO                      20000 // tiempo del motor en estado apagado. Tiempo en milisegundos
#define TPO_TEST                         5  // tiempo para testeo de funcionamiento del motor. Tiempo en segundos

//Variables Globales

int lectura_ldr;
int f_mensaje = 0;
int pasada = 0;
int Tiempo_Motor = 0;
int estacion = 0;
unsigned long Millis_apagado_ant = millis();

void setup() { 
  Serial.begin(9600);
  CONFIG_LED_TEST; 
  CONFIG_PIN_MOTOR;
  CONFIG_PIN_LDR;
  
  estacion = LEER_PIN_ESTACION ? VERANO : INVIERNO; 

  ACTUALIZAR_LED_TEST(0); 
  APAGAR_MOTOR;
  MENSAJE("Hola!!. Iniciando Sistema");
  MENSAJE("Motor Apagado");
  MENSAJE("Para testeo de motor presione 't'");
}

void loop()
 {
  LedTest();  
  Leer_LDR();
  RecepcionSerie();
  Ctrl_Motor();
 }


void LedTest()
{
  static int ledState= 0;
  static unsigned long antMillis = 0;
 
  if(millis() - antMillis < MS_INTERVALO_LED_TEST) return;    
  antMillis = millis();

  ledState = !ledState;
  ACTUALIZAR_LED_TEST(ledState);
}

void Leer_LDR()
{    
  if(LEER_LDR < 600) {pasada = 0; return; }
  if(pasada > estacion) return;
  if(Tiempo_Motor) {Millis_apagado_ant = millis(); return;}    
 
  if(millis() - Millis_apagado_ant < TPO_APAGADO) return;  
   f_mensaje = 0; pasada++;
   Tiempo_Motor = TPO_ENCENDIDO;
   MSJCONC("Ronda : ");  
   MENSAJE(pasada);
}

void RecepcionSerie()
{
  //Preguntar si hay datos en el puerto Serie, esperando a que sean leidos
  //Si no hay datos, salir de la función
  //Si hay datos, leer el dato y procesarlo
  char dato;
  
  if(!Serial.available()) return;
  if(Tiempo_Motor != 0) return;
  
  dato = Serial.read();
  f_mensaje = 0;
  if(dato !='t') return;
    Tiempo_Motor = TPO_TEST; 
    MENSAJE("Iniciando testeo");
    f_mensaje = 1;
  }

void Ctrl_Motor(){
  static unsigned long millis_ant=0, Tiempo_Motor_ant=0;

  if(millis() - millis_ant < 1000) return;
    millis_ant = millis();

  //EVENTO ENCENDER MOTOR
  if(Tiempo_Motor && !Tiempo_Motor_ant) {ENCENDER_MOTOR; MENSAJE("Motor Encendido");}

  //EVENTO FIN DEL TESTEO
  if(!Tiempo_Motor && f_mensaje == 1) {MENSAJE("Fin del Testeo"); f_mensaje = 0;}

  //EVENTO APAGAR MOTOR
  if(!Tiempo_Motor && Tiempo_Motor_ant) {APAGAR_MOTOR; MENSAJE("Motor Apagado");}

  

  Tiempo_Motor_ant=Tiempo_Motor;

  if(Tiempo_Motor) Tiempo_Motor--;  
}
