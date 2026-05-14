#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <ctime>
#include <thread>
#include <chrono>
#include <set>
#include <limits>
#include <windows.h>

using namespace std;

class Event {
public:
    int id;
    string date;
    string time;
    string desc;

    static int counter;

    Event() : id(0) {}

    Event(string d, string t, string de)
        : date(d), time(t), desc(de) {
        id = counter++;
    }

    string serialize() const {
        return to_string(id) + "| " + date + "| " + time + "| " + desc;
    }

    static Event deserialize(const string& line) {
        stringstream ss(line);

        string id_str, d, t, de;

        getline(ss, id_str, '|');
        getline(ss, d, '|');
        getline(ss, t, '|');
        getline(ss, de);

        Event e(d, t, de);

        e.id = stoi(id_str);

        if (e.id >= counter)
            counter = e.id + 1;

        return e;
    }
};

int Event::counter = 0;

class Manager {
private:
    string filename = "events.txt";

    set<int> triggeredToday;

    int lastDay = -1;

public:

    // ================= LOAD =================
    vector<Event> load() {
        vector<Event> v;

        ifstream file(filename);

        string line;

        while (getline(file, line)) {
            if (!line.empty()) {
                v.push_back(Event::deserialize(line));
            }
        }

        return v;
    }

    // ================= SAVE =================
    void save(const vector<Event>& v) {
        ofstream file(filename);

        for (const auto& e : v) {
            file << e.serialize() << endl;
        }
    }

    // ================= ADD =================
    void addEvent() {
        string d, t, de;

        cout << "Date (YYYY-MM-DD): ";
        getline(cin, d);

        cout << "Time (HH:MM): ";
        getline(cin, t);

        cout << "Description: ";
        getline(cin, de);

        Event e(d, t, de);

        ofstream file(filename, ios::app);

        file << e.serialize() << endl;

        cout << "Added with ID: " << e.id << endl;
    }

    // ================= REMOVE =================
    void removeEvent() {
        int id;

        cout << "Enter ID: ";

        cin >> id;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        vector<Event> v = load();

        auto it = remove_if(
            v.begin(),
            v.end(),
            [id](const Event& e) {
                return e.id == id;
            }
        );

        if (it != v.end()) {
            v.erase(it, v.end());

            save(v);

            cout << "Removed.\n";
        }
        else {
            cout << "Not found.\n";
        }
    }

    // ================= LINEAR SEARCH =================
    void linearSearch() {
        string key;

        cout << "Keyword: ";

        getline(cin, key);

        vector<Event> v = load();

        for (const auto& e : v) {
            if (e.desc.find(key) != string::npos) {
                cout << e.id << " -> "
                     << e.date << " "
                     << e.time << " "
                     << e.desc << endl;
            }
        }
    }

    // ================= BINARY SEARCH =================
    void binarySearch() {
        string key;

        cout << "Exact description: ";

        getline(cin, key);

        vector<Event> v = load();

        sort(
            v.begin(),
            v.end(),
            [](const Event& a, const Event& b) {
                return a.desc < b.desc;
            }
        );

        int l = 0;
        int r = (int)v.size() - 1;

        while (l <= r) {
            int m = l + (r - l) / 2;

            if (v[m].desc == key) {
                cout << "Found: "
                     << v[m].id
                     << " -> "
                     << v[m].desc
                     << endl;

                return;
            }
            else if (v[m].desc < key) {
                l = m + 1;
            }
            else {
                r = m - 1;
            }
        }

        cout << "Not found.\n";
    }

    // ================= MAP SEARCH =================
    void mapSearch() {
        int id;

        cout << "Enter ID: ";

        cin >> id;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        map<int, Event> m;

        vector<Event> v = load();

        for (const auto& e : v) {
            m[e.id] = e;
        }

        if (m.count(id)) {
            cout << "Found: "
                 << m[id].desc
                 << endl;
        }
        else {
            cout << "Not found.\n";
        }
    }

    // ================= BUBBLE SORT =================
    void bubbleSort() {
        vector<Event> v = load();

        for (size_t i = 0; i < v.size(); i++) {
            for (size_t j = 0; j < v.size() - i - 1; j++) {
                if (v[j].date > v[j + 1].date) {
                    swap(v[j], v[j + 1]);
                }
            }
        }

        for (const auto& e : v) {
            cout << e.date
                 << " "
                 << e.time
                 << " -> "
                 << e.desc
                 << endl;
        }
    }

    // ================= FAST SORT =================
    void fastSort() {
        vector<Event> v = load();

        sort(
            v.begin(),
            v.end(),
            [](const Event& a, const Event& b) {
                return a.date < b.date;
            }
        );

        for (const auto& e : v) {
            cout << e.date
                 << " "
                 << e.time
                 << " -> "
                 << e.desc
                 << endl;
        }
    }

    // ================= TIME CONVERT =================
    time_t toTime(string date, string time) {
        tm t = {};

        sscanf_s(
            date.c_str(),
            "%d-%d-%d",
            &t.tm_year,
            &t.tm_mon,
            &t.tm_mday
        );

        sscanf_s(
            time.c_str(),
            "%d:%d",
            &t.tm_hour,
            &t.tm_min
        );

        t.tm_year -= 1900;
        t.tm_mon -= 1;
        t.tm_sec = 0;

        return mktime(&t);
    }

    // ================= REMINDER LOOP =================
void reminderLoop() {

    vector<Event> events = load();

    const string weekdays[7] = {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"
    };

    // store formatted calendar text
    string calendarText;

    // map weekday -> list of events
    map<int, vector<Event>> weeklyEvents;

    // ================= GROUP EVENTS =================
    for (const auto& e : events) {

        time_t eventTime = toTime(e.date, e.time);

        tm localEventTime;

        localtime_s(&localEventTime, &eventTime);

        int weekday = localEventTime.tm_wday;

        weeklyEvents[weekday].push_back(e);
    }

    // ================= BUILD CALENDAR TEXT =================
    for (int day = 0; day < 7; day++) {

        calendarText += "======== ";
        calendarText += weekdays[day];
        calendarText += " ========\n";

        if (weeklyEvents[day].empty()) {

            calendarText += "No Events\n\n";
        }
        else {

            for (const auto& e : weeklyEvents[day]) {

                calendarText += e.time;
                calendarText += "  -  ";
                calendarText += e.desc;
                calendarText += "\n";
            }

            calendarText += "\n";
        }
    }

    // ================= SHOW POPUP =================
    MessageBoxA(
        NULL,
        calendarText.c_str(),
        "Weekly Calendar",
        MB_OK | MB_SETFOREGROUND
    );
}

};

// ================= MAIN =================
int main() {

    Manager m;

    m.reminderLoop();

    /*thread t(
        &Manager::reminderLoop,
        &m
    );

    t.detach();*/

    int choice;

    while (true) {

        cout << "\n===== MENU =====\n";

        cout << "1. Add Event\n";
        cout << "2. Remove Event\n";
        cout << "3. Linear Search\n";
        cout << "4. Binary Search\n";
        cout << "5. Map Search\n";
        cout << "6. Bubble Sort\n";
        cout << "7. Fast Sort\n";
        cout << "8. Show Weekly Calendar\n";
        cout << "0. Exit\n";

        cout << "Choose: ";

        if (!(cin >> choice)) {

            cin.clear();

            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );

            continue;
        }

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );

        switch (choice) {

        case 1:
            m.addEvent();
            break;

        case 2:
            m.removeEvent();
            break;

        case 3:
            m.linearSearch();
            break;

        case 4:
            m.binarySearch();
            break;

        case 5:
            m.mapSearch();
            break;

        case 6:
            m.bubbleSort();
            break;

        case 7:
            m.fastSort();
            break;

        case 8:
            m.reminderLoop();
            break;
        case 0:
            return 0;

        default:
            cout << "Invalid choice.\n";
        }
    }
}