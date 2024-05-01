#include <Arduino.h>

#define pinInterruption 5
#define TRIAC 18
#define pinEntradaAnalogica A6

volatile int i = 0;  // Controla el tiempo de disparo del TRIAC
volatile boolean cruce_cero = false;
unsigned long previousTime = 0;
int T_int = 100;  // Tiempo de ejecución del Timer en uS
int alfa;

hw_timer_t *timer = NULL; // Declarar el objeto del temporizador

void IRAM_ATTR deteccion_Cruce_cero() {
  cruce_cero = true;
  i = 0;
  digitalWrite(TRIAC, LOW);
}

void IRAM_ATTR Dimer() {
  digitalWrite(LED_BUILTIN, HIGH);
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - previousTime;

  if (cruce_cero) {
    if (i >= alfa) {
      digitalWrite(TRIAC, HIGH);
      i = 0;
      cruce_cero = false;
    } else {
      i++;
    }
  }
  previousTime = currentTime;
}

/* Delay function replacement */
void delayReplacement(unsigned long timeDelay) {
  unsigned long timeInit = millis();
  while (millis() - timeInit < timeDelay);
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIAC, OUTPUT);
  attachInterrupt(pinInterruption, deteccion_Cruce_cero, RISING);
  pinMode(LED_BUILTIN, OUTPUT);
  // Configurar el temporizador para que se repita cada 100 microsegundos
  timer = timerBegin(0, 80, true); // timer 0, prescaler 80, contar ascendente
  timerAttachInterrupt(timer, &Dimer, true); // adjuntar la función de interrupción
  timerAlarmWrite(timer, 100, true); // establecer el valor del temporizador y permitir que se repita
  timerAlarmEnable(timer); // habilitar el temporizador
}

void loop() {
  // Leer el valor de la entrada analógica
  int valorEntrada = analogRead(pinEntradaAnalogica);

  // Mapear el valor de la entrada analógica del rango 0-4095 al rango 10-80
  alfa = map(valorEntrada, 0, 4095, 10, 80);
  Serial.println(alfa);
  delayReplacement(100);
}
