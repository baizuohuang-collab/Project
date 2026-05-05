#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <limits>
#include <chrono>

using namespace std;

// ===================== Event Class =====================
class Event {
public:
    int ID;
    string date;
    string time;
    string category;
    string description;

    static int counter;

    Event(string d, string t, string c, string desc) {
        ID = counter++;
        date = d;
        time = t;
        category = c;
        description = desc;
    }

    // Convert event to string (for file storage)
    string toString() const {
        return to_string(ID) + "|" + date + "|" + time + "|" + category + "|" + description;
    }

    // Convert string to Event
    static Event fromString(const string& line) {
        stringstream ss(line);
        string token;
        vector<string> parts;

        while (getline(ss, token, '|')) {
            parts.push_back(token);
        }

        Event e(parts[1], parts[2], parts[3], parts[4]);
        e.ID = stoi(parts[0]);

        if (e.ID >= counter) counter = e.ID + 1;

        return e;
    }
};

int Event::counter = 0;


// ===================== Manager Class =====================
class Manager {
private:
    string filename;

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

    void writeAll(const vector<Event>& events) {
        ofstream file(filename);
        for (const auto& e : events) {
            file << e.toString() << endl;
        }
    }

public:
    Manager(string fname) : filename(fname) {}

    void addEvent() {
        string date, time, category, description;

        cout << "Date: ";
        getline(cin, date);
        cout << "Time: ";
        getline(cin, time);
        cout << "Category: ";
        getline(cin, category);
        cout << "Description: ";
        getline(cin, description);

        Event e(date, time, category, description);

        ofstream file(filename, ios::app);
        file << e.toString() << endl;

        cout << "Added with ID: " << e.ID << endl;
    }

    void removeEvent(int id) {
        vector<Event> events = readAll();

        auto it = remove_if(events.begin(), events.end(),
            [id](const Event& e) { return e.ID == id; });

        if (it != events.end()) {
            events.erase(it, events.end());
            writeAll(events);
            cout << "Removed.\n";
        } else {
            cout << "ID not found.\n";
        }
    }

    void searchEvent(const string& keyword) {
        vector<Event> events = readAll();
        bool found = false;

        for (const auto& e : events) {
            if (e.description.find(keyword) != string::npos) {
                cout << "[" << e.ID << "] "
                     << e.date << " "
                     << e.time << " "
                     << e.category << " -> "
                     << e.description << endl;
                found = true;
            }
        }

        if (!found) cout << "No match found.\n";
    }

    void countEvents() {
        vector<Event> events = readAll();
        cout << "Total events: " << events.size() << endl;
    }

    void printAll() {
        vector<Event> events = readAll();

        for (const auto& e : events) {
            cout << "[" << e.ID << "] "
                 << e.date << " "
                 << e.time << " "
                 << e.category << " -> "
                 << e.description << endl;
        }
    }

    void linearSearch(const string& keyword) {
        vector<Event> events = readAll();
        int steps = 0;
        bool found = false;

        for (const auto& e : events) {
        steps++;
        if (e.description.find(keyword) != string::npos) {
            cout << "[" << e.ID << "] " << e.description << endl;
            found = true;
        }
        }

    cout << "Linear Search Steps: " << steps << endl;
    if (!found) cout << "No match found.\n";
    }

static bool compareDesc(const Event& a, const Event& b) {
    return a.description < b.description;
}

    void binarySearch(const string& keyword) {
        vector<Event> events = readAll();

    // MUST SORT FIRST
    sort(events.begin(), events.end(), compareDesc);

    int left = 0, right = events.size() - 1;
    int steps = 0;

    while (left <= right) {
        steps++;
        int mid = (left + right) / 2;

        if (events[mid].description == keyword) {
            cout << "Found: [" << events[mid].ID << "] "
                 << events[mid].description << endl;
            cout << "Binary Search Steps: " << steps << endl;
            return;
        }
        else if (events[mid].description < keyword) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    cout << "Not found.\n";
    cout << "Binary Search Steps: " << steps << endl;
    }

    void bubbleSortByDate() {
    vector<Event> events = readAll();
    int n = events.size();
    int steps = 0;

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            steps++;
            if (events[j].date > events[j + 1].date) {
                swap(events[j], events[j + 1]);
            }
        }
    }

    cout << "Bubble Sort Steps: " << steps << endl;

    for (const auto& e : events) {
        cout << e.date << " -> " << e.description << endl;
    }
    }

    void fastSortByDate() {
    vector<Event> events = readAll();

    int steps = 0;

    sort(events.begin(), events.end(),
        [&steps](const Event& a, const Event& b) {
            steps++;
            return a.date < b.date;
        });

    cout << "std::sort Steps (approx): " << steps << endl;

    for (const auto& e : events) {
        cout << e.date << " -> " << e.description << endl;
    }
    }


};


// ===================== Menu =====================
void showMenu() {
    cout << "\n===== MENU =====\n";
    cout << "1. Add Event\n";
    cout << "2. Remove Event\n";
    cout << "3. Search Event\n";
    cout << "4. Count Events\n";
    cout << "5. Show All\n";
    cout << "6. Linear Search\n";
    cout << "7. Binary Search\n";
    cout << "8. Bubble Sort\n";
    cout << "9. Fast Sort\n";
    cout << "0. Exit\n";
    cout << "Choose: ";
}


// ===================== Main =====================
int main() {
    int choice;

    Manager manager("events.txt");

    cout << "god bless me\n";

    while (true) {
        showMenu();
        cin >> choice;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                manager.addEvent();
                break;

            case 2: {
                int id;
                cout << "Enter ID to remove: ";
                cin >> id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                manager.removeEvent(id);
                break;
            }

            case 3: {
                string keyword;
                cout << "Enter keyword: ";
                getline(cin, keyword);
                manager.searchEvent(keyword);
                break;
            }

            case 4:
                manager.countEvents();
                break;

            case 5:
                manager.printAll();
                break;
            case 6: {
                string keyword;
                cout << "Keyword: ";
                getline(cin, keyword);
                manager.linearSearch(keyword);
                break;
                    }
            case 7: {
                string keyword;
                cout << "Keyword (exact match): ";
                getline(cin, keyword);
                manager.binarySearch(keyword);
                 break;
                    }
            case 8:
                manager.bubbleSortByDate();
                break;
            case 9:
                 manager.fastSortByDate();
                break;
            case 0:
                cout << "Bye!\n";
                return 0;

            default:
                cout << "Invalid choice.\n";
        }
    }

    auto start = chrono::high_resolution_clock::now();

    // algorithm

    auto end = chrono::high_resolution_clock::now();
    cout << "Time: "
        << chrono::duration<double, milli>(end - start).count()
        << " ms\n";
}