#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();  // Für periodische Updates

protected:
    LineChart chart;
    static const int DATA_POINTS = 100;
    int16_t iData[DATA_POINTS];
    int16_t qData[DATA_POINTS];
    int dataIndex = 0;
};

#endif // SCREEN1VIEW_HPP
