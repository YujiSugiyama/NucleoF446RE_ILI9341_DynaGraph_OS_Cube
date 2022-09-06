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
    virtual void graphClicked(AbstractDataGraph::GraphClickEvent value);
    virtual void graphPause();
    void handleTickEvent();
    virtual void putData(float* wave, int n);

protected:
private:
    int graphActive;
#ifndef SIMULATOR
    int waveReady;
    float temp[3*5];
    int tempCount;
#else
    float  sin_lut[360];
    int waveCount;
#endif
};

#endif // SCREEN1VIEW_HPP
