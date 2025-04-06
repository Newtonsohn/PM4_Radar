#ifndef LOGINVIEW_HPP
#define LOGINVIEW_HPP

#include <gui_generated/login_screen/loginViewBase.hpp>
#include <gui/login_screen/loginPresenter.hpp>
#include <touchgfx/widgets/pin_txt.hpp>

class LoginView : public LoginViewBase
{
public:
    LoginView();
    virtual ~LoginView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void onDigitPressed(uint8_t digit);
    void onClearPressed();
    void onOkPressed();

private:
    Unicode::UnicodeChar pinBuffer[5]; // 4 Stellen + Null
    uint8_t pinIndex;
};

