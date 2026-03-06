#include "NameInputScreen.h"
#include "ui/Theme.h"
#include "config/Config.h"
#include "hal/Display.h"

void NameInputScreen::draw(LGFX_TDeck& gfx) {
    int cx = Theme::SCREEN_W / 2;

    // Ratspeak branding
    gfx.setTextSize(2);
    gfx.setTextColor(Theme::PRIMARY, Theme::BG);
    const char* brand = "RATSPEAK";
    int bw = strlen(brand) * 12;
    gfx.setCursor(cx - bw / 2, 30);
    gfx.print(brand);

    // .org subtitle
    gfx.setTextSize(1);
    gfx.setTextColor(Theme::ACCENT, Theme::BG);
    const char* sub = "ratspeak.org";
    int sw = strlen(sub) * 6;
    gfx.setCursor(cx - sw / 2, 52);
    gfx.print(sub);

    // Prompt
    gfx.setTextColor(Theme::SECONDARY, Theme::BG);
    const char* prompt = "Enter your display name";
    int pw = strlen(prompt) * 6;
    gfx.setCursor(cx - pw / 2, 85);
    gfx.print(prompt);

    gfx.setTextColor(Theme::MUTED, Theme::BG);
    const char* opt = "(Optional - press Enter to skip)";
    int ow = strlen(opt) * 6;
    gfx.setCursor(cx - ow / 2, 100);
    gfx.print(opt);

    // Text input field
    int fieldW = 200;
    int fieldH = 20;
    int fieldX = cx - fieldW / 2;
    int fieldY = 125;
    gfx.drawRect(fieldX, fieldY, fieldW, fieldH, Theme::PRIMARY);
    gfx.fillRect(fieldX + 1, fieldY + 1, fieldW - 2, fieldH - 2, Theme::SELECTION_BG);

    // Name text
    gfx.setTextSize(1);
    gfx.setTextColor(Theme::PRIMARY, Theme::SELECTION_BG);
    gfx.setCursor(fieldX + 6, fieldY + 6);
    gfx.print(_name);

    // Cursor blink (simple solid cursor)
    int cursorX = fieldX + 6 + _nameLen * 6;
    if (cursorX < fieldX + fieldW - 8) {
        bool blink = (millis() / 500) % 2 == 0;
        if (blink) {
            gfx.fillRect(cursorX, fieldY + 4, 6, 12, Theme::PRIMARY);
        }
    }

    // OK hint
    gfx.setTextColor(Theme::ACCENT, Theme::BG);
    const char* hint = "[Enter] OK";
    int hw = strlen(hint) * 6;
    gfx.setCursor(cx - hw / 2, 160);
    gfx.print(hint);

    // Version at bottom
    gfx.setTextColor(Theme::MUTED, Theme::BG);
    char ver[32];
    snprintf(ver, sizeof(ver), "Ratdeck v%s", RATDECK_VERSION_STRING);
    int vw = strlen(ver) * 6;
    gfx.setCursor(cx - vw / 2, 190);
    gfx.print(ver);
}

bool NameInputScreen::handleKey(const KeyEvent& event) {
    if (event.enter || event.character == '\n' || event.character == '\r') {
        if (_doneCb) _doneCb(String(_name));
        return true;
    }

    if (event.del || event.character == 8) {
        if (_nameLen > 0) {
            _nameLen--;
            _name[_nameLen] = '\0';
            markDirty();
        }
        return true;
    }

    // Printable characters
    if (event.character >= 0x20 && event.character <= 0x7E && _nameLen < MAX_NAME_LEN) {
        _name[_nameLen] = event.character;
        _nameLen++;
        _name[_nameLen] = '\0';
        markDirty();
        return true;
    }

    return true; // Consume all keys on this screen
}
