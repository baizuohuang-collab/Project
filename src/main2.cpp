#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <thread>
#include <chrono>
#include <set>
#include <windows.h>

using namespace std;

// ===================== Event =====================
class Event {
public:
    int ID;
    int weekday; // 0=Sun ... 6=Sat
    string time; // HH:MM
    string description;

    static int counter;

    Event() { ID = -1; }

    Event(int wd, string t, string desc) {
        ID = counter++;
        weekday = wd;
        time = t;
        description = desc;
    }

    string toString() const {
        return to_string(ID) + "|" + to_string(weekday) + "|" + time + "|" + description;
    }

    static Event fromString(const string& line) {
        stringstream ss(line);
        string token;
        vector<string> parts;

        while (getline(ss, token, '|')) {
            parts.push_back(token);
        }

        Event e(stoi(parts[1]), parts[2], parts[3]);
        e.ID = stoi(parts[0]);

        if (e.ID >= counter) counter = e.ID + 1;

        return e;
    }
};

int Event::counter = 0;


// ===================== Manager =====================
class Manager {
private:
    string filename;
    set<int> triggered; // prevent duplicate popups

public:
    Manager(string fname) : filename(fname) {}

    vector<Event> readAll() {
        vector<Event> events;
        ifstream file(filename);
        string line;

        while (getline(file, line)) {
            if (!line.empty()) {
                events.push_back(Event::fromString(line));
            }
        }
        return events;
    }

    void addEvent() {
        int weekday;
        string time, desc;

        cout << "Enter weekday (0=Sun,1=Mon,...6=Sat): ";
        cin >> weekday;
        cin.ignore();

        cout << "Enter time (HH:MM): ";
        getline(cin, time);

        cout << "Enter description: ";
        getline(cin, desc);

        Event e(weekday, time, desc);

        ofstream file(filename, ios::app);
        file << e.toString() << endl;

        cout << "Added with ID: " << e.ID << endl;
    }

    void showPopup(const string& msg) {
        MessageBoxA(NULL, msg.c_str(), "Reminder", MB_OK | MB_ICONINFORMATION);
    }

    time_t getNextReminderTime(const Event& e) {
        time_t now = time(nullptr);
        tm* now_tm = localtime(&now);

        int currentDay = now_tm->tm_wday;

        int daysAhead = (e.weekday - currentDay + 7) % 7;

        tm event_tm = *now_tm;
        event_tm.tm_mday += daysAhead;

        sscanf(e.time.c_str(), "%d:%d", &event_tm.tm_hour, &event_tm.tm_min);
        event_tm.tm_sec = 0;

        time_t eventTime = mktime(&event_tm);

        return eventTime - 600; // 10 minutes before
    }

    void startReminderSystem() {
        while (true) {
            vector<Event> events = readAll();
            time_t now = time(nullptr);

            for (const auto& e : events) {
                time_t reminderTime = getNextReminderTime(e);

                // check within 30 sec window
                if (difftime(now, reminderTime) >= 0 &&
                    difftime(now, reminderTime) < 30) {

                    if (triggered.find(e.ID) == triggered.end()) {
                        showPopup("Reminder: " + e.description);
                        triggered.insert(e.ID);
                    }
                }
            }

            this_thread::sleep_for(chrono::seconds(10));
        }
    }
};


// ===================== Menu =====================
void showMenu() {
    cout << "\n===== MENU =====\n";
    cout << "1. Add Event\n";
    cout << "0. Exit\n";
    cout << "Choose: ";
}


// ===================== Main =====================
int main() {
    Manager manager("events.txt");

    // 🔥 Start background reminder system
    thread t(&Manager::startReminderSystem, &manager);
    t.detach();

    int choice;

    while (true) {
        showMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                manager.addEvent();
                break;

            case 0:
                cout << "Bye!\n";
                return 0;

            default:
                cout << "Invalid.\n";
        }
    }
}