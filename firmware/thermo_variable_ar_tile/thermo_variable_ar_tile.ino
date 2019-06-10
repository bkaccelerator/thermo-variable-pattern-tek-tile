// Thermo-variable Pattern Tek-Tile by the Brooklyn Fashion and Design Accelerator

// This firmware implements a demo for the tile, which has two heaters and two
// buttons controlled by an Adafruit Metro Mini 328 microcontroller board.
// Once power is applied the heaters begin a heating and cooling cycle, changing
// the color of the thermochromic dye in different regions of the tile. If one
// of the buttons is pressed then the automatic control is temporarily paused
// and the buttons can be used to directly activate the heaters.

// How long to pause automatic heater control after last interaction (via buttons)
const int PAUSE_TIME = 5000;
const long ON_TIME = 20000;
const long OFF_TIME = 3 * 60000;

const int PIN_HEATER1 = 2;
const int PIN_BUTTON1 = 3;
const int PIN_HEATER2 = 4;
const int PIN_BUTTON2 = 5;

typedef enum HeatingState {
  HEATING1,
  COOLING1,
  HEATING2,
  COOLING2,
} HeatingState;

long lastEventTime = -OFF_TIME;
long pauseTime = -1;
HeatingState state = COOLING2;

void setup() {
  pinMode(PIN_HEATER1, OUTPUT);
  pinMode(PIN_HEATER2, OUTPUT);

  pinMode(PIN_BUTTON1, INPUT_PULLUP);
  pinMode(PIN_BUTTON2, INPUT_PULLUP);

  // Serial is used for debug messages
  Serial.begin(115200);
  Serial.println("Brooklyn Fashion and Design Accelerator");
  Serial.println("Thermo-Variable AR Tek-Tile");
}

bool buttonPressed(int button) {
  if (button > 1) {
    return false;
  }

  if (button == 0) {
    return !digitalRead(PIN_BUTTON1);
  } else if (button == 1) {
    return !digitalRead(PIN_BUTTON2);
  }

  return false;
}

bool isPaused() {
  if (pauseTime < 0) {
    // There have been no pauses so far
    return false;
  }

  return millis() - pauseTime < PAUSE_TIME;
}

void pause() {
  Serial.println("Paused");
  pauseTime = millis();
}

void checkButtons() {
  if (buttonPressed(0)) {
    if (!isPaused()) {
      pause();

      // Heater 2 might be on but the button not pressed
      digitalWrite(PIN_HEATER2, LOW);

      Serial.println("Button 1 pressed");
    }

    digitalWrite(PIN_HEATER1, HIGH);
  } else {
    // While paused turn the heater off if the button is not pressed
    if (isPaused()) {
      digitalWrite(PIN_HEATER1, LOW);
    }
  }

  if (buttonPressed(1)) {
    if (!isPaused()) {
      pause();

      // Heater 1 might be on but the button not pressed
      digitalWrite(PIN_HEATER1, LOW);

      Serial.println("Button 2 pressed");
    }

    digitalWrite(PIN_HEATER2, HIGH);
  } else {
    // While paused turn the heater off if the button is not pressed
    if (isPaused()) {
      digitalWrite(PIN_HEATER1, LOW);
    }
  }
}

void loop() {
  static bool wasPaused = false;
  checkButtons();

  long timeSinceLastEvent = millis() - lastEventTime;

  bool paused = isPaused();

  if (!paused && wasPaused) {
    Serial.println("Unpaused");
  }

  wasPaused = paused;

  if (!paused) {
    switch (state) {
      case HEATING1:
        digitalWrite(PIN_HEATER1, HIGH);
        
        // Transition if necessary
        if (timeSinceLastEvent > ON_TIME) {
          state = COOLING1;
          lastEventTime = millis();
          Serial.println("Cooling region 1");
        }
        break;
      case COOLING1:
        digitalWrite(PIN_HEATER1, LOW);
        
        // Transition if necessary
        if (timeSinceLastEvent > OFF_TIME) {
          state = HEATING2;
          lastEventTime = millis();
          Serial.println("Heating region 2");
        }
        break;
      case HEATING2:
        digitalWrite(PIN_HEATER2, HIGH);
        
        // Transition if necessary
        if (timeSinceLastEvent > ON_TIME) {
          state = COOLING2;
          lastEventTime = millis();
          Serial.println("Cooling region 2");
        }
        break;
      case COOLING2:
        digitalWrite(PIN_HEATER2, LOW);
        
        // Transition if necessary
        if (timeSinceLastEvent > OFF_TIME) {
          state = HEATING1;
          lastEventTime = millis();
          Serial.println("Heating region 1");
        }
        break;
    }
  }
}
