#ifndef PROGRESSBAR_HPP
#define PROGRESSBAR_HPP

#include "parameter_permutator.hpp"

#include <sys/ioctl.h>
#include <unistd.h>
#include <chrono>
#include <sstream>

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;

int getTerminalColumns() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

template <typename PP>
class Progressbar
{
public:
    Progressbar():
        _start(std::chrono::steady_clock::now())
    {}


    void callback(PP* pp) {
        auto const elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - _start).count();
        auto const remainingSeconds = (pp->getNumberOfTotalPermutations() - pp->getCurrentIteration()) * elapsedSeconds / pp->getCurrentIteration();
        float const cyclesPerSecond = elapsedSeconds == 0 ? 0.0 : pp->getCurrentIteration() / elapsedSeconds;

        std::stringstream ss;
        ss
        << std::fixed << std::setprecision(1)
        << "\r[" << pp->getCurrentIteration() << "/" << pp->getNumberOfTotalPermutations()
        << "] [" << cyclesPerSecond << "it/s " << elapsedSeconds/60 << ">" << remainingSeconds/60 << " min] [best=" << pp->getBestScore() << "] ";
        int barwidth = getTerminalColumns() - ss.str().size() - 2;
        ss << "[";
        int pos = barwidth * pp->getCurrentIteration() / pp->getNumberOfTotalPermutations();
        for (int i = 0; i < barwidth; ++i) {
            if (i < pos) ss << "=";
            else if (i == pos) ss << ">";
            else ss << " ";
        }
        ss << "] ";

        cout << ss.str() << flush;
        if (_additionalCallback) {
            _additionalCallback(pp);
        }
    }

    void setAdditionalCallback(std::function<void(PP*)> const& callback) {
        _additionalCallback = callback;
    }


private:
    std::chrono::steady_clock::time_point _start;
    std::function<void(PP*)> _additionalCallback;
};


#endif // PROGRESSBAR_HPP