#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
#include <cstring>

using namespace std;

// ================= STRUCT =================
struct Record {

    string spotID;
    string vehicleNumber;
    string vehicleType;
    string ownerName;
    string status;
};

// ================= INITIALIZE DATABASE =================
void initializeDatabase() {

    ifstream file("parking_database (1).csv");

    if (!file.good()) {

        cout << "Database file not found.\n";
    }

    file.close();
}

// ================= CHECK UNIQUE ID =================
bool isUnique(string id) {

    ifstream file("parking_database (1).csv");

    string line;

    getline(file, line);

    while (getline(file, line)) {

        stringstream ss(line);

        string currentID;

        getline(ss, currentID, ',');

        if (currentID == id) {

            file.close();

            return false;
        }
    }

    file.close();

    return true;
}

// ================= APPEND RECORD =================
void appendRecord(string data) {

    ofstream file("parking_database (1).csv", ios::app);

    file << data << endl;

    file.close();

    cout << "Record added successfully.\n";
}

// ================= SEARCH BY ID =================
void searchByID(string id) {

    ifstream file("parking_database (1).csv");

    string line;

    bool found = false;

    getline(file, line);

    while (getline(file, line)) {

        stringstream ss(line);

        string currentID;

        getline(ss, currentID, ',');

        if (currentID == id) {

            cout << "\n===== RECORD FOUND =====\n";

            cout << line << endl;

            found = true;

            break;
        }
    }

    file.close();

    if (!found) {

        cout << "Record not found.\n";
    }
}

// ================= UPDATE RECORD =================
void updateRecord(string id, string newData) {

    ifstream file("parking_database (1).csv");

    ofstream tempFile("temp.csv");

    string line;

    bool found = false;

    getline(file, line);

    tempFile << line << endl;

    while (getline(file, line)) {

        stringstream ss(line);

        string currentID;

        getline(ss, currentID, ',');

        if (currentID == id) {

            tempFile << newData << endl;

            found = true;
        }

        else {

            tempFile << line << endl;
        }
    }

    file.close();

    tempFile.close();

    remove("parking_database (1).csv");

    rename("temp.csv", "parking_database (1).csv");

    if (found)
        cout << "Record updated successfully.\n";

    else
        cout << "Record not found.\n";
}

// ================= DISPLAY ALL RECORDS =================
void displayAll() {

    ifstream file("parking_database (1).csv");

    string line;

    cout << "\n===== ALL RECORDS =====\n\n";

    while (getline(file, line)) {

        cout << line << endl;
    }

    file.close();
}

// ================= CHECK ESP32 CONNECTION =================
bool checkESP32() {

    HANDLE hSerial;

    hSerial = CreateFile(
        "COM4",
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );

    if (hSerial == INVALID_HANDLE_VALUE) {

        return false;
    }

    // SERIAL SETTINGS
    DCB dcbSerialParams = {0};

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {

        CloseHandle(hSerial);

        return false;
    }

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {

        CloseHandle(hSerial);

        return false;
    }

    // TIMEOUT SETTINGS
    COMMTIMEOUTS timeouts = {0};

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 2000;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 2000;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(hSerial, &timeouts);

    // WAIT FOR ESP32 RESET
    Sleep(3000);

    // CLEAR BUFFER
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    // SEND COMMAND
    char command[] = "CHECK_ID\n";

    DWORD bytesWritten;

    WriteFile(
        hSerial,
        command,
        strlen(command),
        &bytesWritten,
        NULL
    );

    // WAIT RESPONSE
    Sleep(1000);

    char buffer[100] = {0};

    DWORD bytesRead;

    ReadFile(
        hSerial,
        buffer,
        sizeof(buffer) - 1,
        &bytesRead,
        NULL
    );

    CloseHandle(hSerial);

    if (bytesRead > 0) {

        return true;
    }

    return false;
}

// ================= GET KEY FROM ESP32 =================
string getESP32Key() {

    HANDLE hSerial;

    hSerial = CreateFile(
        "COM4",
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );

    if (hSerial == INVALID_HANDLE_VALUE) {

        return "";
    }

    // SERIAL SETTINGS
    DCB dcbSerialParams = {0};

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    GetCommState(hSerial, &dcbSerialParams);

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    SetCommState(hSerial, &dcbSerialParams);

    // TIMEOUT SETTINGS
    COMMTIMEOUTS timeouts = {0};

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 2000;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 2000;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(hSerial, &timeouts);

    // WAIT FOR ESP32 RESET
    Sleep(3000);

    // CLEAR BUFFER
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    // SEND COMMAND
    char command[] = "SEND_KEY\n";

    DWORD bytesWritten;

    WriteFile(
        hSerial,
        command,
        strlen(command),
        &bytesWritten,
        NULL
    );

    Sleep(1000);

    // RECEIVE DATA
    char buffer[100] = {0};

    DWORD bytesRead;

    ReadFile(
        hSerial,
        buffer,
        sizeof(buffer) - 1,
        &bytesRead,
        NULL
    );

    CloseHandle(hSerial);

    string receivedID(buffer);

    if (!receivedID.empty()) {

        receivedID.erase(
            receivedID.find_last_not_of("\r\n") + 1
        );
    }

    return receivedID;
}

// ================= LOGIN SYSTEM =================
bool loginSystem() {

    string username;
    string password;

    cout << "\n===== LOGIN SYSTEM =====\n";

    cout << "Enter Username: ";
    getline(cin, username);

    cout << "Enter Password: ";
    getline(cin, password);

    if (username == "shahzaib" &&
        password == "2501528") {

        cout << "\nLogin Successful.\n";

        return true;
    }

    else {

        cout << "\nInvalid Username or Password.\n";

        return false;
    }
}

// ================= MAIN FUNCTION =================
int main() {

    cout << "Checking ESP32 Connection...\n";

    if (!checkESP32()) {

        cout << "ESP32 NOT Connected.\n";
        cout << "Program Cannot Execute.\n";

        system("pause");

        return 0;
    }

    cout << "ESP32 Connected Successfully.\n";

    // LOGIN
    if (!loginSystem()) {

        return 0;
    }

    // DATABASE
    initializeDatabase();

    int choice;

    do {

        cout << "\n===== SMART PARKING SYSTEM =====\n";

        cout << "1. Add Record\n";
        cout << "2. Search Record Manually\n";
        cout << "3. Search Record Using ESP32 Key\n";
        cout << "4. Update Record\n";
        cout << "5. Display All Records\n";
        cout << "6. Exit\n";

        cout << "Enter Choice: ";

        cin >> choice;

        cin.ignore();

        // ADD RECORD
        if (choice == 1) {

            string id;
            string vehicle;
            string type;
            string owner;
            string status;

            cout << "Enter Spot ID (P001): ";
            getline(cin, id);

            if (!isUnique(id)) {

                cout << "Duplicate ID Not Allowed.\n";

                continue;
            }

            cout << "Enter Vehicle Number: ";
            getline(cin, vehicle);

            cout << "Enter Vehicle Type: ";
            getline(cin, type);

            cout << "Enter Owner Name: ";
            getline(cin, owner);

            cout << "Enter Status (IN/OUT): ";
            getline(cin, status);

            string data =
                id + "," +
                vehicle + "," +
                type + "," +
                owner + "," +
                status;

            appendRecord(data);
        }

        // SEARCH MANUAL
        else if (choice == 2) {

            string id;

            cout << "Enter Spot ID: ";

            getline(cin, id);

            searchByID(id);
        }

        // SEARCH USING ESP32
        else if (choice == 3) {

            string receivedID = getESP32Key();

            if (receivedID == "") {

                cout << "Failed To Receive ID From ESP32.\n";
            }

            else {

                cout << "\nID Received From ESP32: "
                     << receivedID << endl;

                searchByID(receivedID);
            }
        }

        // UPDATE RECORD
        else if (choice == 4) {

            string id;
            string vehicle;
            string type;
            string owner;
            string status;

            cout << "Enter Existing Spot ID: ";
            getline(cin, id);

            cout << "Enter New Vehicle Number: ";
            getline(cin, vehicle);

            cout << "Enter New Vehicle Type: ";
            getline(cin, type);

            cout << "Enter New Owner Name: ";
            getline(cin, owner);

            cout << "Enter New Status (IN/OUT): ";
            getline(cin, status);

            string newData =
                id + "," +
                vehicle + "," +
                type + "," +
                owner + "," +
                status;

            updateRecord(id, newData);
        }

        // DISPLAY
        else if (choice == 5) {

            displayAll();
        }

        else if (choice != 6) {

            cout << "Invalid Choice.\n";
        }

    } while (choice != 6);

    cout << "\nProgram Terminated Successfully.\n";

    return 0;
}


