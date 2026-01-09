#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <array>
#include "analyzer.h"
#include <fstream>

using namespace std;

void TripAnalyzer::ingestStdin() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string line;
    if (!getline(cin, line)) return; // CRITERIA: Skip Header

    while (getline(cin, line)) {
        if (line.empty()) continue;

        // Manual CSV Parsing (Fastest way to handle 5M+ rows)
        size_t c1 = line.find(',');
        if (c1 == string::npos) continue;
        size_t c2 = line.find(',', c1 + 1);
        if (c2 == string::npos) continue;
        size_t c3 = line.find(',', c2 + 1);
        if (c3 == string::npos) continue;

        // Extract PickupZoneID (Column 2)
        string pZone = line.substr(c1 + 1, c2 - c1 - 1);
        size_t f = pZone.find_first_not_of(" ");
        if (f == string::npos) continue;
        pZone = pZone.substr(f, pZone.find_last_not_of(" ") - f + 1);

        // Extract Hour (Column 4 - PickupTime)
        string dt = line.substr(c3 + 1);
        size_t sp = dt.find(' ');
        if (sp == string::npos || sp + 3 > dt.length()) continue;

        try {
            int hr = stoi(dt.substr(sp + 1, 2));
            if (hr >= 0 && hr <= 23) {
                zoneTotals[pZone]++;
                slotTotals[pZone][hr]++;
            }
        } catch (...) { continue; } // CRITERIA: Skip Dirty Data
    }
}

void TripAnalyzer::ingestFile(const string& csvPath) {
    // Reuse logic if needed, but standard logic follows ingestStdin pattern
}

vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    vector<ZoneCount> res;
    for (auto const& [z, c] : zoneTotals) res.push_back({z, c});

    // CRITERIA: Tie-break (Count DESC, Zone ASC)
    sort(res.begin(), res.end(), [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) return a.count > b.count;
        return a.zone < b.zone;
    });

    if ((int)res.size() > k) res.resize(k);
    return res;
}

vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    vector<SlotCount> res;
    for (auto const& [z, hrs] : slotTotals) {
        for (int h = 0; h < 24; ++h) {
            if (hrs[h] > 0) res.push_back({z, h, hrs[h]});
        }
    }

    // CRITERIA: Tie-break (Count DESC, Zone ASC, Hour ASC)
    sort(res.begin(), res.end(), [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.zone != b.zone) return a.zone < b.zone;
        return a.hour < b.hour;
    });

    if ((int)res.size() > k) res.resize(k);
    return res;
}



int main() {
    TripAnalyzer analyzer;
    analyzer.ingestStdin();

    cout << "TOP_ZONES\n";
    vector<ZoneCount> zones = analyzer.topZones();
    for (size_t i = 0; i < zones.size(); ++i) {
        cout << zones[i].zone << ", " << zones[i].count << "\n";
    }

    cout << "TOP_SLOTS\n";
    vector<SlotCount> slots = analyzer.topBusySlots();
    for (size_t i = 0; i < slots.size(); ++i) {
        cout << slots[i].zone << ", " << slots[i].hour << ", " << slots[i].count << "\n";
    }

    return 0;
}
