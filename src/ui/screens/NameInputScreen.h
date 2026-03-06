#pragma once

#include "ui/UIManager.h"
#include <functional>

class NameInputScreen : public Screen {
public:
    bool handleKey(const KeyEvent& event) override;
    const char* title() const override { return "Setup"; }
    void draw(LGFX_TDeck& gfx) override;

    void setDoneCallback(std::function<void(const String&)> cb) { _doneCb = cb; }

    static constexpr int MAX_NAME_LEN = 16;

private:
    char _name[MAX_NAME_LEN + 1] = {0};
    int _nameLen = 0;
    std::function<void(const String&)> _doneCb;
};
