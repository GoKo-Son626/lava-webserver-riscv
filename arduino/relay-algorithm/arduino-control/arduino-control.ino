/* For turning on relay 1, send the command `echo "SET 2 0" > /dev/ttyACM0` through the terminal

Relay and pin connections:

| Pin | Relay |
| 2   |   1   |
| 3   |   2   |
| 4   |   3   |
| 5   |   4   |

*/


int relayPins[] = {2, 3, 4, 5};  // Adjust as needed
bool relayStates[4] = {0, 0, 0, 0};  // Initial states: all OFF
String inputString = "";
bool stringComplete = false;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
  }
  inputString.reserve(32);
}

void loop() {
  if (stringComplete) {
    handleCommand(inputString);
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}

void handleCommand(String cmd) {
  cmd.trim(); // remove \r or spaces

  if (cmd.startsWith("SET")) {
    int pinNum, state;
    sscanf(cmd.c_str(), "SET %d %d", &pinNum, &state);

    // Find pin index
    int relayIndex = -1;
    for (int i = 0; i < 4; i++) {
      if (relayPins[i] == pinNum) {
        relayIndex = i;
        break;
      }
    }

    if (relayIndex != -1 && (state == 0 || state == 1)) {
      digitalWrite(relayPins[relayIndex], state ? HIGH : LOW);
      relayStates[relayIndex] = state;
      Serial.println("OK");
    } else {
      Serial.println("ERROR");
    }
  } else if (cmd == "GET") {
    for (int i = 0; i < 4; i++) {
      Serial.print(relayStates[i]);
      if (i < 3) Serial.print(" ");
    }
    Serial.println();
  } else {
    Serial.println("UNKNOWN COMMAND");
  }
}

