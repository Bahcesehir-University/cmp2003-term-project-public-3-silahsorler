#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <array>
using namespace std;
// --- Data Structures ---
struct ZoneCount {
   string zone;
   long long count;
};
struct SlotCount {
   string zone;
   int hour;
   long long count;
};
// --- Splitting Lines ---
static bool splitLines(const string& line, vector<string>& cols) {
   cols.clear();
   string token;
   for (char c : line) {
       if (c == ',') {
           cols.push_back(token);
           token.clear();
       } else {
           token += c;
       }
   }
   cols.push_back(token);
   return cols.size() >= 6;
}
// --- Class Implementing ---
class TripAnalyzer {
private:
   unordered_map<string, long long> zoneTotals;
   unordered_map<string, array<long long, 24>> slotTotals;
public:
   void ingestFile(const string& filename) {
       ifstream fin(filename);
       if (!fin.is_open()) {
           cerr << "Error opening file: " << filename << "\n";
           return;
       }
       string line;
       vector<string> cols;
       // Skipping header row
       if (!getline(fin, line)) {
           fin.close();
           return;
       }
       while (getline(fin, line)) {
           if (line.empty()) continue;
           if (!splitLines(line, cols)) continue;
           // Extracting PickupZoneID
           string pickupZone = cols[1];
           size_t first = pickupZone.find_first_not_of(" ");
           if (first == string::npos) continue;
           size_t last = pickupZone.find_last_not_of(" ");
           pickupZone = pickupZone.substr(first, last - first + 1);
           // Extracting PickupDateTime
           string dateTime = cols[3];
           size_t firstDT = dateTime.find_first_not_of(" ");
           if (firstDT == string::npos) continue;
           size_t lastDT = dateTime.find_last_not_of(" ");
           dateTime = dateTime.substr(firstDT, lastDT - firstDT + 1);
           size_t spacePos = dateTime.find(' ');
           if (spacePos == string::npos) continue;
           size_t colonPos = dateTime.find(':', spacePos + 1);
           if (colonPos == string::npos) continue;
           try {
               int hour = stoi(dateTime.substr(spacePos + 1, colonPos - spacePos - 1));
               if (hour >= 0 && hour <= 23) {
                   zoneTotals[pickupZone]++;
                   if (slotTotals.find(pickupZone) == slotTotals.end()) {
                       slotTotals[pickupZone].fill(0);
                   }
                   slotTotals[pickupZone][hour]++;
               }
           } catch (...) {
               continue;
           }
       }
       fin.close();
   }
   vector<ZoneCount> topZones() {
       vector<ZoneCount> results;
       results.reserve(zoneTotals.size());
       for (auto it = zoneTotals.begin(); it != zoneTotals.end(); ++it) {
           ZoneCount z;
           z.zone = it->first;
           z.count = it->second;
           results.push_back(z);
       }
       sort(results.begin(), results.end(), [](const ZoneCount& a, const ZoneCount& b) {
           if (a.count != b.count) return a.count > b.count;
           return a.zone < b.zone;
       });
       if (results.size() > 10) results.resize(10);
       return results;
   }
   vector<SlotCount> topBusySlots() {
       vector<SlotCount> results;
       for (auto &it : slotTotals) {
           for (int h = 0; h < 24; ++h) {
               if (it.second[h] > 0) {
                   SlotCount s;
                   s.zone = it.first;
                   s.hour = h;
                   s.count = it.second[h];
                   results.push_back(s);
               }
           }
       }
       sort(results.begin(), results.end(), [](const SlotCount& a, const SlotCount& b) {
           if (a.count != b.count) return a.count > b.count;
           if (a.zone != b.zone) return a.zone < b.zone;
           return a.hour < b.hour;
       });
       if (results.size() > 10) results.resize(10);
       return results;
   }
};
// --- Main Func ---
int main() {
   TripAnalyzer analyzer;
   analyzer.ingestFile("Trips.csv");
   cout << "TOP_ZONES\n";
   for (auto& z : analyzer.topZones())
       cout << z.zone << "," << z.count << "\n";
   cout << "TOP_SLOTS\n";
   for (auto& s : analyzer.topBusySlots())
       cout << s.zone << "," << s.hour << "," << s.count << "\n";
   return 0;
}
