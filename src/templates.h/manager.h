#include<iostream>
#include<string>
#include<vector>

using namespace std;

template <typename T>
class Manager {
private:
    vector<T> data;   // 用來存資料

public:
    // 新增資料
    void add(const T& item) 
    {
        data.push_back(item);
    }

    // 刪除資料（刪除第一個符合的）
    bool remove(const T& item) 
    {
        auto it = find(data.begin(), data.end(), item);
        if (it != data.end()) 
        {
            data.erase(it);
            return true;
        }
        return false;
    }

    // 搜尋資料（回傳是否存在）
    bool search(const T& item) const 
    {
        return find(data.begin(), data.end(), item) != data.end();
    }

    // 統計資料（回傳資料筆數）
    size_t count() const 
    {
        return data.size();
    }

    // 顯示所有資料（方便測試）
    void printAll() const 
    {
        for (const auto& item : data) 
        {
            cout << item << "\n";
        }
    }
};