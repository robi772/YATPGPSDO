/*
   (c) Copyright Szabolcs Szigeti 2020-2021

   Redistribution under GPL V3. See the LICENSE file for details.

*/
/*
  Handle the button presses. Both buttons, the mode and the select buttons
  are managed. They are debounced that is only presses longer than the DBOUNCE time are detected.

  Once for the Mode andonce for the Select button, the code is identical.
  The buttons' press fucntions are called giving the lenght of press as parameter
*/

void handleButtons() {

  if (digitalRead(BTN_MODE) == LOW && lastBtnMode == 1) {
    backlTime = lastBtnModeTime = millis();
    lastBtnMode = 0;
  }

  if (digitalRead(BTN_MODE) == HIGH && lastBtnMode == 0) {
    lastBtnMode = 1;
    btnPress = millis() - lastBtnModeTime;
    if (btnPress > DEBOUNCE) {
      PressBtnMode(btnPress);
    }
  }


  if (digitalRead(BTN_SEL) == LOW && lastBtnSel == 1) {
    backlTime = lastBtnSelTime = millis();
    lastBtnSel = 0;
  }

  if (digitalRead(BTN_SEL) == HIGH && lastBtnSel == 0) {
    lastBtnSel = 1;
    btnPress = millis() - lastBtnSelTime;
    if (btnPress > DEBOUNCE) {
      PressBtnSel(btnPress);
    }
  }

}
/*
   Handle press of the Mode button.
   A short press will always select a new display.
   Long press action depends on display mode, currently none.
*/
void PressBtnMode(int len) {
  if (len < SHORTPRESS) {
    nextDisplay();
  }
  else {
    ;
  }
}

/*
   Handle pressing of the Select button.
   This is a big ugly code to do whatever is needed on the current display mode
*/
void PressBtnSel(int len) {
  switch (displayMode) {
    case POSITION:
      if (len < SHORTPRESS) {
        if (menuMode == SHOWSURVEY)
          menuMode = SETSURVEY;
        else if (++surveySetTime > 24)
          surveySetTime = 1;
      }
      else {
        menuMode = SHOWSURVEY;
        refresh = true;
        Serial2.print("$SURVEY ");
        Serial2.println(surveySetTime);
      }
      break;
    case CONFIG:
      if (len < SHORTPRESS) {
        switch (menuMode) {
          case SETOFFSET: menuMode = SETBACKLIGHT; break;
          case SETBACKLIGHT: menuMode = SAVECONFIG; break;
          case SAVECONFIG: menuMode = LOADPOS; break;
          case LOADPOS:
          default: menuMode = SETOFFSET; break;
        }
      }
      else {
        switch (menuMode) {
          case SETOFFSET:
            if (++gpsdoConfig.timeoffset > 12)
              gpsdoConfig.timeoffset = -12;
            break;
          case SETBACKLIGHT:
            if (++gpsdoConfig.backlight > 2)
              gpsdoConfig.backlight = 0;
            break;
          case SAVECONFIG:
            gpsdoConfig.saved = true;         // this is a kludge for the display
            saveConfig();
            break;
          case LOADPOS:                       // May not need this, the TP seems to save its last position by itself anyway
            Serial2.print("$SETPOS ");
            Serial2.print(gpsdoConfig.lat);
            Serial2.print(" ");
            Serial2.print(gpsdoConfig.lon);
            Serial2.print(" ");
            Serial2.println(gpsdoConfig.elev);
            displayMode = POSITION;
            break;
        }
      }
      break;

    default: ;
  }
  refresh = true;
}
