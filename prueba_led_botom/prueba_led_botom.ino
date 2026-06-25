#include <Arduino.h>
#include <LedControl.h>

#define DIN_PIN 11
#define CLK_PIN 13
#define CS_PIN 10
#define BUTTON_PIN 2

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);

int estadoMatriz = 0;
unsigned long ultimoBoton = 0;

void setup()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    lc.shutdown(0, false);
    lc.setIntensity(0, 8);
    lc.clearDisplay(0);
}

void loop()
{
    if (digitalRead(BUTTON_PIN) == LOW)
    {
        if (millis() - ultimoBoton > 250)
        {
            ultimoBoton = millis();
            estadoMatriz = !estadoMatriz;
            if (estadoMatriz)
            {
                encenderMatriz();
            }
            else
            {
                apagarMatriz();
            }
        }
    }
}
void encenderMatriz()
{
    int fila;
    int columna;
    for (fila = 0; fila < 8; fila++)
    {
        for (columna = 0; columna < 8; columna++)
        {
            lc.setLed(0, fila, columna, true);
        }
    }
}
void apagarMatriz()
{
    lc.clearDisplay(0);
}