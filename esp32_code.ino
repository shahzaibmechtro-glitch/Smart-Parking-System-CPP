// ======================================================
// ESP32 SMART PARKING SYSTEM
// ======================================================

String parkingID = "P001";

void setup() {

    Serial.begin(115200);

    delay(3000);

    Serial.println("ESP32 READY");
}

void loop() {

    if (Serial.available() > 0) {

        String command =
            Serial.readStringUntil('\n');

        command.trim();

        // SEND ID
        if (command == "SEND_KEY") {

            Serial.println(parkingID);
        }

        // CHECK CONNECTION
        else if (command == "CHECK_ID") {

            Serial.println("CONNECTED");
        }

        // CHANGE ID
        else if (command.startsWith("SET_ID:")) {

            parkingID = command.substring(7);

            parkingID.trim();

            Serial.println("NEW ID SET");
            Serial.println(parkingID);
        }

        else {

            Serial.println("INVALID COMMAND");
        }
    }
}
