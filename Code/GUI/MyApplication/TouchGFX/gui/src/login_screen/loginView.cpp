#include <gui/login_screen/loginView.hpp>

LoginView::LoginView()
    : pinIndex(0)
{
    pinBuffer[0] = '\0';
}

void LoginView::setupScreen()
{
    LoginViewBase::setupScreen();
    pinIndex = 0;
    pinBuffer[0] = '\0';

    // Beispiel: setze den PIN-Text beim Start
    textAreaPIN.setWildcard(pinBuffer);
    textAreaPIN.invalidate();
}

void LoginView::onDigitPressed(uint8_t digit)
{
    if (pinIndex < 4)
    {
        pinBuffer[pinIndex++] = '0' + digit;
        pinBuffer[pinIndex] = '\0';
        textAreaPIN.invalidate();
    }
}

void LoginView::onClearPressed()
{
    pinIndex = 0;
    pinBuffer[0] = '\0';
    textAreaPIN.invalidate();
}

void LoginView::onOkPressed()
{
    if (strcmp((const char*)pinBuffer, "1234") == 0)
    {
        // ✅ Richtig – hier z. B. Screenwechsel machen
    }
    else
    {
        // ❌ Falsch – zurücksetzen
        onClearPressed();
    }
}
