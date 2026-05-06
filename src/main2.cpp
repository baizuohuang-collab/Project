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
    Event(string d, string t, string de) : date(d), time(t), desc(de) {
        id = counter++;
    }

    string serialize() const {
        return to_string(id) + "|" + date + "|" + time + "|" + desc;
    }

    static Event deserialize(const string& line) {
        stringstream ss(line);
        string part, id_str, d, t, de;
        getline(ss, id_str, '|');
        getline(ss, d, '|');
        getline(ss, t, '|');
        getline(ss, de, '|');

        Event e(d, t, de);
        e.id = stoi(id_str);
        if (e.id >= counter) counter = e.id + 1;
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
    vector<Event> load() {
        vector<Event> v;
        ifstream file(filename);
        string line;
        while (getline(file, line)) if (!line.empty()) v.push_back(Event::deserialize(line));
        return v;
    }

    void save(const vector<Event>& v) {
        ofstream file(filename);
        for (const auto& e : v) file << e.serialize() << endl;
    }

    void addEvent() {
        string d, t, de;
        cout << "Date (YYYY-MM-DD): "; getline(cin, d);
        cout << "Time (HH:MM): "; getline(cin, t);
        cout << "Description: "; getline(cin, de);
        Event e(d, t, de);
        ofstream file(filename, ios::app);
        file << e.serialize() << endl;
        cout << "Added with ID: " << e.id << endl;
    }

    void removeEvent() {
        int id;
        cout << "Enter ID: "; cin >> id; cin.ignore();
        vector<Event> v = load();
        auto it = remove_if(v.begin(), v.end(), [id](const Event& e) { return e.id == id; });
        if (it != v.end()) { v.erase(it, v.end()); save(v); cout << "Removed.\n"; }
        else cout << "Not found.\n";
    }

    void linearSearch() {
        string key;
        cout << "Keyword: "; getline(cin, key);
        for (auto& e : load()) if (e.desc.find(key) != string::npos) cout << e.id << " -> " << e.desc << endl;
    }

    void binarySearch() {
        string key;
        cout << "Exact description: "; getline(cin, key);
        vector<Event> v = load();
        sort(v.begin(), v.end(), [](const Event& a, const Event& b) { return a.desc < b.desc; });
        int l = 0, r = (int)v.size() - 1;
        while (l <= r) {
            int m = l + (r - l) / 2;
            if (v[m].desc == key) { cout << "Found: " << v[m].id << endl; return; }
            else if (v[m].desc < key) l = m + 1;
            else r = m - 1;
        }
        cout << "Not found.\n";
    }

    void mapSearch() {
        int id;
        cout << "Enter ID: "; cin >> id; cin.ignore();
        map<int, Event> m;
        for (auto& e : load()) m[e.id] = e;
        if (m.count(id)) cout << "Found: " << m[id].desc << endl;
        else cout << "Not found.\n";
    }

    void bubbleSort() {
        vector<Event> v = load();
        for (size_t i = 0; i < v.size(); i++)
            for (size_t j = 0; j < v.size() - i - 1; j++)
                if (v[j].date > v[j + 1].date) swap(v[j], v[j + 1]);
        for (auto& e : v) cout << e.date << " " << e.time << " -> " << e.desc << endl;
    }

    void fastSort() {
        vector<Event> v = load();
        sort(v.begin(), v.end(), [](const Event& a, const Event& b) { return a.date < b.date; });
        for (auto& e : v) cout << e.date << " " << e.time << " -> " << e.desc << endl;
    }

    time_t toTime(string date, string time) {
        tm t = {};
        sscanf_s(date.c_str(), "%d-%d-%d", &t.tm_year, &t.tm_mon, &t.tm_mday);
        sscanf_s(time.c_str(), "%d:%d", &t.tm_hour, &t.tm_min);
        t.tm_year -= 1900; t.tm_mon -= 1;
        return mktime(&t);
    }

void reminderLoop() {
    vector<Event> events = load(); // load once

    while (true) {
        time_t now = time(0);
        tm t;
        localtime_s(&t, &now);

        if (t.tm_mday != lastDay) {
            triggeredToday.clear();
            lastDay = t.tm_mday;
        }

        for (auto& e : events) {
            time_t et = toTime(e.date, e.time);

            // Trigger only once within a 3-second window
            if (abs(difftime(now, et - 600)) < 3 && !triggeredToday.count(e.id)) {
                MessageBoxA(NULL, e.desc.c_str(), "Reminder", MB_OK | MB_SETFOREGROUND);
                triggeredToday.insert(e.id);
            }
        }

        this_thread::sleep_for(chrono::seconds(5));
    }
}

};

int main() {
    Manager m;
    thread t(&Manager::reminderLoop, &m);
    t.detach();
    int choice;
    while (true) {
        cout << "\n1.Add 2.Remove 3.Linear 4.Binary 5.Map 6.Bubble 7.Fast 0.Exit\nChoose: ";
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (choice) {
            case 1: m.addEvent(); break;
            case 2: m.removeEvent(); break;
            case 3: m.linearSearch(); break;
            case 4: m.binarySearch(); break;
            case 5: m.mapSearch(); break;
            case 6: m.bubbleSort(); break;
            case 7: m.fastSort(); break;
            case 0: return 0;
        }
    }
}
