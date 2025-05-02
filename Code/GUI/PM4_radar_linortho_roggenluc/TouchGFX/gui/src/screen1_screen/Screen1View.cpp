#include <gui/screen1_screen/Screen1View.hpp>
#include <math.h>  // Für sin()/cos()

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
    // Chart konfigurieren
       chart.setPosition(10, 10, 460, 300);
       chart.setRange(0, 100, -1000, 1000);
       chart.setLineWidth(2);
       chart.setColor(touchgfx::Color::getColorFromRGB(0, 0, 255));
       chart.setAdditionalLineColor(touchgfx::Color::getColorFromRGB(255, 0, 0));

       // Daten initialisieren
       for(int i = 0; i < DATA_POINTS; i++) {
           iData[i] = 0;
           qData[i] = 0;
       }

       // Chart zur View hinzufügen
       add(chart);

       // Tick-Intervall setzen
       setTickInterval(50);  // 50ms Update-Intervall
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::handleTickEvent()
{
    // Simulierte Daten (später durch echte Sensordaten ersetzen)
    static float angle = 0;
    const float step = 2 * 3.14159 / 50;

    // Daten nach links schieben
    for(int i = 0; i < DATA_POINTS - 1; i++) {
        iData[i] = iData[i + 1];
        qData[i] = qData[i + 1];
    }

    // Neue Daten berechnen
    iData[DATA_POINTS - 1] = static_cast<int16_t>(800 * sin(angle));
    qData[DATA_POINTS - 1] = static_cast<int16_t>(800 * cos(angle));

    angle += step;

    // Chart aktualisieren
    chart.setDataContainer(0, iData, DATA_POINTS);
    chart.setDataContainer(1, qData, DATA_POINTS);
    chart.invalidate();  // Neuzeichnen erzwingen
}
