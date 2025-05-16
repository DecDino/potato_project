#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <ctime>
#include <fstream>
#include <random>
#include <map>
#include <dirent.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define mkdir _mkdir
#else
#include <sys/stat.h>
#endif

using namespace std;

void showMenu() {
    cout << "请选择功能：" << endl;
    cout << "1. 经典窑烤" << endl;
    cout << "2. 切片烤箱" << endl;
    cout << "3. 蜜红薯" << endl;
    cout << "4. 双倍蜜红薯" << endl;
    cout << "5. 代糖红薯" << endl;
    cout << "6. 查看桌上红薯" << endl;
}

void prepareTableFolder() {
    mkdir("table");
}

string getTimestampedFilename(const string& type) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", ltm);
    return "table/" + type + "_" + string(buffer) + ".png";
}

int randomDelay() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(100, 500);
    return dis(gen);
}

void showProgressBar() {
    const int total = 10;
    for (int i = 1; i <= total; ++i) {
        int percent = (i * 100) / total;
        cout << "\r[";
        for (int j = 0; j < i; ++j) cout << "█";
        for (int j = i; j < total; ++j) cout << " ";
        cout << "] " << (percent < 10 ? " " : "") << percent << "%" << flush;
        this_thread::sleep_for(chrono::milliseconds(randomDelay()));
    }
    cout << endl;
}

bool copyFile(const string& src, const string& dest) {
    ifstream in(src, ios::binary);
    ofstream out(dest, ios::binary);
    return in && out && (out << in.rdbuf());
}

void showProgressSteps(int version) {
    vector<vector<string>> steps = {
        {"清洗地瓜", "装窑升温", "窑烤进行中"},
        {"清洗地瓜", "切片", "烤箱加热中"},
        {"清洗地瓜", "切块", "熬煮糖浆", "闷煮地瓜"},
        {"清洗地瓜", "切块", "熬煮更多糖浆", "闷煮地瓜"},
        {"清洗地瓜", "切块", "加糖闷煮地瓜"}
    };

    const vector<string>& selectedSteps = steps[version - 1];

    for (size_t i = 0; i < selectedSteps.size(); ++i) {
        cout << "步骤 " << (i + 1) << ": " << selectedSteps[i] << endl;
        showProgressBar();
    }

    cout << "[完成]" << endl;

    string assetFile = "assets/" + to_string(version) + "_";
    string typeName;
    if (version == 1) { assetFile += "kiln_baked.png"; typeName = "kiln_baked"; }
    else if (version == 2) { assetFile += "oven_sliced.png"; typeName = "oven_sliced"; }
    else if (version == 3) { assetFile += "sugar_regular.png"; typeName = "sugar_regular"; }
    else if (version == 4) { assetFile += "sugar_double.png"; typeName = "sugar_double"; }
    else if (version == 5) { assetFile += "sugarfree.png"; typeName = "sugarfree"; }

    prepareTableFolder();
    string destPath = getTimestampedFilename(typeName);
    if (copyFile(assetFile, destPath)) {
        cout << "已将成品保存在 " << destPath << endl;
    } else {
        cout << "复制图片失败，文件可能不存在：" << assetFile << endl;
    }
}

void checkTable() {
    DIR* dir = opendir("table");
    if (!dir) {
        cout << "桌子上空空如也" << endl;
        return;
    }

    vector<pair<string, string>> types = {
        {"kiln_baked", "1. 经典窑烤"},
        {"oven_sliced", "2. 切片烤箱"},
        {"sugar_regular", "3. 蜜红薯"},
        {"sugar_double", "4. 双倍蜜红薯"},
        {"sugarfree", "5. 代糖红薯"}
    };

    map<string, int> countMap;
    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;
        for (const auto& [key, label] : types) {
            if (filename.find(key) == 0) {
                countMap[key]++;
            }
        }
    }
    closedir(dir);

    if (countMap.empty()) {
        cout << "桌子上空空如也" << endl;
        return;
    }

    for (const auto& [key, label] : types) {
        if (countMap[key] > 0) {
            cout << label << " -- " << countMap[key] << "个" << endl;
        }
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    while (true) {
        showMenu();
        cout << "输入选项编号或 /exit 退出: ";

        string input;
        cin >> input;
        if (input == "/exit") {
            cout << "感谢使用，再见！" << endl;
            break;
        }

        int version = 0;
        try {
            version = stoi(input);
        } catch (...) {
            cout << "无效输入，请输入数字或 /exit" << endl;
            continue;
        }

        if (version >= 1 && version <= 5) {
            showProgressSteps(version);
        } else if (version == 6) {
            checkTable();
        } else {
            cout << "无效选项。" << endl;
        }
    }

    return 0;
}

