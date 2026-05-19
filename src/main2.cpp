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
#include <iomanip>

using namespace std;


    // ================= SHOW GUI WINDOW =================

    std::string globalText = "Hello from your GUI window!\nYou can put your weekly calendar here.";

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            DrawTextA(hdc, globalText.c_str(), -1, &rect, DT_LEFT | DT_TOP | DT_WORDBREAK);

            EndPaint(hwnd, &ps);
        }
        return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    void showGuiWindow(const std::string& text) {
    globalText = text;

    HINSTANCE hInstance = GetModuleHandle(NULL);

    const char CLASS_NAME[] = "MyWindowClass";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 199, 240));


    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Weekly Calendar",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        600, 500,   // <-- WINDOW SIZE HERE
        NULL,
        NULL,
        hInstance,
        NULL
    );

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    }



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

    cout << "Enter ID to remove: ";

    cin >> id;

    cin.ignore(
        numeric_limits<streamsize>::max(),
        '\n'
    );

    vector<Event> v = load();

    bool found = false;

    // ================= FIND EVENT =================

    for (const auto& e : v) {

        if (e.id == id) {

            found = true;

            cout << "\nEvent Found:\n";

            cout << "Date: "
                 << e.date
                 << endl;

            cout << "Time: "
                 << e.time
                 << endl;

            cout << "Description: "
                 << e.desc
                 << endl;

            break;
        }
    }

    if (!found) {

        cout << "Event not found.\n";

        return;
    }

    // ================= CONFIRM DELETE =================

    char confirm;

    cout << "\nAre you sure you want to delete this event? (y/n): ";

    cin >> confirm;

    cin.ignore(
        numeric_limits<streamsize>::max(),
        '\n'
    );

    if (confirm != 'y' &&
        confirm != 'Y') {

        cout << "Deletion cancelled.\n";

        return;
    }

    // ================= DELETE EVENT =================

    auto it = remove_if(
        v.begin(),
        v.end(),
        [id](const Event& e) {

            return e.id == id;
        }
    );

    v.erase(it, v.end());

    // ================= REBUILD IDS =================

    for (size_t i = 0; i < v.size(); i++) {

        v[i].id = i;
    }

    // IMPORTANT:
    // reset counter
    Event::counter = v.size();

    // save updated data
    save(v);

    cout << "Event deleted successfully.\n";
    }

    // ================= LINEAR SEARCH =================
    void linearSearch() {
    string key;
    cout << "Keyword: ";
    getline(cin, key);

    vector<Event> v = load();

    for (const auto& e : v) {
        string word;
        stringstream ss(e.desc);
        bool found = false;

        while (ss >> word) {
            if (word == key) {
                found = true;
                break;
            }
        }

        if (found) {
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

        // Reassign IDs in sorted order
        for (size_t i = 0; i < v.size(); i++) {
             v[i].id = i + 1;   // IDs become 1,2,3,...
        }

        save(v);

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

            // Reassign IDs in sorted order
        for (size_t i = 0; i < v.size(); i++) {
            v[i].id = i + 1;   // IDs become 1,2,3,...
        }

        save(v);

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
    std::vector<Event> events = load();

    const std::string weekdays[7] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };

    std::map<int, std::vector<Event>> weeklyEvents;

    // ================= GET CURRENT WEEK =================
    time_t now = time(0);
    tm currentTime;
    localtime_s(&currentTime, &now);

    // Current weekday (0 = Sunday, 1 = Monday, ...)
    int currentWeekday = currentTime.tm_wday;

    // Make Monday = 0, Tuesday = 1, ..., Sunday = 6
    int mondayOffset = (currentWeekday == 0) ? 6 : currentWeekday - 1;

    // Calculate this week's Monday date
    currentTime.tm_mday -= mondayOffset;
    
    // Clear out hours/minutes/seconds to make comparisons purely date-based
    currentTime.tm_hour = 0;
    currentTime.tm_min = 0;
    currentTime.tm_sec = 0;
    currentTime.tm_isdst = -1; // Let the system determine daylight saving time

    // Normalize Monday time once to get a clean, solid timestamp
    time_t mondayTimestamp = mktime(&currentTime);

    // ================= CHECK EVENTS =================
    for (const auto& e : events) {
        int y = 0, m = 0, d = 0;

        // Parse date strings safely
        if (sscanf_s(e.date.c_str(), "%d-%d-%d", &y, &m, &d) != 3) {
            continue; // Skip invalid date strings to avoid garbage data
        }

        tm eventTm = {};
        eventTm.tm_year = y - 1900;
        eventTm.tm_mon = m - 1;
        eventTm.tm_mday = d;
        eventTm.tm_hour = 0;
        eventTm.tm_min = 0;
        eventTm.tm_sec = 0;
        eventTm.tm_isdst = -1;

        // Normalize event time once to get its timestamp
        time_t eventTimestamp = mktime(&eventTm);

        // Compare week difference using stable timestamps
        double secondsDiff = difftime(eventTimestamp, mondayTimestamp);
        double daysDiff = secondsDiff / 86400.0;

        // Account for potential slight rounding bugs or daylight savings time shifts
        // Checking from Day 0 (Monday) through Day 6 (Sunday) inclusively
        if (daysDiff >= -0.1 && daysDiff < 6.9) {
            weeklyEvents[eventTm.tm_wday].push_back(e);
        }
    }

    // ================= BUILD TEXT =================
    std::string calendarText;
    calendarText += "CURRENT WEEK EVENTS\n\n";

    // Display order: Monday through Sunday
    int order[7] = { 1, 2, 3, 4, 5, 6, 0 };

    for (int i = 0; i < 7; i++) {
        int day = order[i];

        calendarText += "========== ";
        calendarText += weekdays[day];
        calendarText += " ==========\n";

        if (weeklyEvents[day].empty()) {
            calendarText += "No Events\n\n";
        }
        else {
            // Sort events of the day chronologically by their time string
            std::sort(
                weeklyEvents[day].begin(),
                weeklyEvents[day].end(),
                [](const Event& a, const Event& b) {
                    return a.time < b.time;
                }
            );

            for (const auto& e : weeklyEvents[day]) {
                calendarText += e.date;
                calendarText += " ";
                calendarText += e.time;
                calendarText += "\n";
                calendarText += e.desc;
                calendarText += "\n\n";
            }
        }
    }

    // ================= SHOW WINDOW =================
    /*MessageBoxA(
        NULL,
        calendarText.c_str(),
        "Weekly Calendar",
        MB_OK | MB_SETFOREGROUND
    );*/
    showGuiWindow(calendarText);

    }

    //================= DAYS IN MONTH =================
    int getDaysInMonth(int year, int month) {

    if (month == 2) {

        bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);

        return leap ? 29 : 28;
    }

    if (month == 4 || month == 6 || month == 9 || month == 11)
        return 30;

    return 31;
    }

    // ================= FIRST WEEKDAY =================
    int getFirstWeekday(int year, int month) {

    tm timeStruct = {};

    timeStruct.tm_year = year - 1900;
    timeStruct.tm_mon = month - 1;
    timeStruct.tm_mday = 1;

    mktime(&timeStruct);

    return timeStruct.tm_wday;
    }

    // ================= HAS EVENT =================
    bool hasEvent(int year, int month, int day) {

    vector<Event> events = load();

    for (const auto& e : events) {

        int y, m, d;

        sscanf_s(
            e.date.c_str(),
            "%d-%d-%d",
            &y,
            &m,
            &d
        );

        if (y == year && m == month && d == day)
            return true;
    }

    return false;
    }

    // ================= SHOW EVENTS OF DATE =================
    void showEventsOfDate(int year, int month, int day) {

    vector<Event> events = load();

    cout << "\n===== EVENTS =====\n";

    bool found = false;

    for (const auto& e : events) {

        int y, m, d;

        sscanf_s(
            e.date.c_str(),
            "%d-%d-%d",
            &y,
            &m,
            &d
        );

        if (y == year && m == month && d == day) {

            cout << e.time
                 << " -> "
                 << e.desc
                 << endl;

            found = true;
        }
    }

    if (!found)
        cout << "No events\n";
    }

    // ================= SHOW CALENDAR =================
    void showCalendar(int year, int month) {

    system("cls");

    const string months[12] = {
        "January", "February", "March",
        "April", "May", "June",
        "July", "August", "September",
        "October", "November", "December"
    };

    cout << "\n========== "
         << months[month - 1]
         << " "
         << year
         << " ==========\n\n";

    cout << "Sun Mon Tue Wed Thu Fri Sat\n";

    int firstDay = getFirstWeekday(year, month);

    int totalDays = getDaysInMonth(year, month);

    for (int i = 0; i < firstDay; i++) {
        cout << "    ";
    }

    for (int day = 1; day <= totalDays; day++) {

        bool eventExists = hasEvent(year, month, day);

        if (eventExists) {
            cout << "[" << setw(2) << day << "]";
        }
        else {
            cout << setw(4) << day;
        }

        if ((firstDay + day) % 7 == 0)
            cout << endl;
    }

    cout << "\n\n";

    cout << "[ ] means this date has events\n";
    }

    // ================= CALENDAR MENU =================
    void calendarMenu() {
    time_t now = time(0);

    tm localTime;

    localtime_s(&localTime, &now);

    int year = localTime.tm_year + 1900;
    int month = localTime.tm_mon + 1;

    while (true) {

        showCalendar(year, month);

        cout << "\nOptions:\n";
        cout << "1. Next Month\n";
        cout << "2. Previous Month\n";
        cout << "3. View Date Events\n";
        cout << "0. Exit Calendar\n";

        int choice;

        cin >> choice;

        cin.ignore();

        if (choice == 1) {

            month++;

            if (month > 12) {
                month = 1;
                year++;
            }
        }

        else if (choice == 2) {

            month--;

            if (month < 1) {
                month = 12;
                year--;
            }
        }

        else if (choice == 3) {

            int day;

            cout << "Enter day: ";

            cin >> day;

            cin.ignore();

            showEventsOfDate(year, month, day);

            system("pause");
        }

        else if (choice == 0) {
            break;
        }
    }
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
        cout << "5. Search ID\n";
        cout << "6. Bubble Sort\n";
        cout << "7. Fast Sort\n";
        cout << "8. Show Weekly Calendar\n";
        cout << "9. Calendar\n";
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

        case 9:
            m.calendarMenu();
            break;

        case 0:
            return 0;

        default:
            cout << "Invalid choice.\n";
        }
    }
}