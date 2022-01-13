

// TODO: how to deal with this situation:
//  hazard flasher is on and we get a turn left|right command?

/*
 * primary function: start flash light cycles based on configuration or turn off high beam headlights
 * secondary function: toggle high beam headlights
 */
void handleButtonHeadLeft(int holdTime) {
  if (holdTime < HEAD_LEFT_BUTTON_LISTEN_SHORT) {
    if (isRelayOn(MCP_PIN_RELAY_BEAM_HEADLIGHTS)) {
      setRelayToggleForPin(MCP_PIN_RELAY_BEAM_HEADLIGHTS);
      return;
    }
    flashLightLoopStart = millis();
    return;
  }
  setRelayToggleForPin(MCP_PIN_RELAY_BEAM_HEADLIGHTS);
}

/*
 * primary function: loop through wiper modes off|slow|medium|fast
 * secondary function: fountain solution for X seconds
 */
void handleButtonHeadRight(int holdTime) {
  if (holdTime < HEAD_RIGHT_BUTTON_LISTEN_SHORT) {
    loopWiperModes();
    return;  
  }
  fountainLoopUntil = millis() + FOUNTAIN_SOLUTION_DURATION;
}

/*
 * primary function: turn left signal for X seconds & disable turn right signal
 * secondary function: turn left signal forever & disable turn right signal
 * tertiary function: hazard flasher & disable turn left+right signals
 */
void handleButtonWristLeft(int holdTime) {
  if (holdTime < TURN_SIGNAL_LISTEN_LONG) {
    handleTurnSignalButton(holdTime, MCP_PIN_RELAY_TURN_LEFT, MCP_PIN_RELAY_TURN_RIGHT);
    return;
  }

  // ensure turn signals are both off
  setRelayOffForPin(MCP_PIN_RELAY_TURN_LEFT);
  setRelayOffForPin(MCP_PIN_RELAY_TURN_RIGHT);
  setRelayToggleForPin(MCP_PIN_RELAY_HAZARD_FLASHER);
}

/*
 * primary function: turn right signal for X seconds & disable turn left signal
 * secondary function: turn right signal forever & disable turn left signal
 * tertiary function: toggle rear fog lamp
 */
void handleButtonWristRight(int holdTime) {
  if (holdTime < TURN_SIGNAL_LISTEN_LONG) {
    handleTurnSignalButton(holdTime, MCP_PIN_RELAY_TURN_RIGHT, MCP_PIN_RELAY_TURN_LEFT);
    return;
  }
  setRelayToggleForPin(MCP_PIN_RELAY_REAR_FOG_LAMP);
}

/*
 * multiple flashing cycles with a single trigger
 */
void flashLightLoop() {
  if (flashLightLoopStart == 0) {
    return;
  }
  unsigned long diff = millis() - flashLightLoopStart;

  uint16_t on = FLASH_LIGHT_ON;
  uint16_t off = FLASH_LIGHT_OFF;
  uint16_t cycles = FLASH_LIGHT_CYCLES;

  if (diff > cycles*(on+off)) {
    setRelayOffForPin(MCP_PIN_RELAY_FLASH_LIGHT);
    flashLightLoopStart = 0;
    return;
  }

  for(uint8_t i = 0; i< cycles; i++) {
    if (diff > on*(i+1)+off*i && diff < on*(i+1)+off*(i+1)
    && isRelayOn(MCP_PIN_RELAY_FLASH_LIGHT)) {
      setRelayOffForPin(MCP_PIN_RELAY_FLASH_LIGHT);
      return;
    }
    if (diff > (on+off)*i && diff < (on+off)*(i+1) - off
    && isRelayOff(MCP_PIN_RELAY_FLASH_LIGHT)) {
      setRelayOnForPin(MCP_PIN_RELAY_FLASH_LIGHT);
      return;
    }
  }
  
}
