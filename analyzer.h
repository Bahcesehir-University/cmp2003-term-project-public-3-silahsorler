#ifndef ANALYZER_H
#define ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <array>

// --- Data Structures ---
struct ZoneCount {
    std::string zone;
    long long count;
};

struct SlotCount {
    std::string zone;
    int hour;
    long long count;
};

class TripAnalyzer {
private:
    // O(1) aggregation logic
    std::unordered_map<std::string, long long> zoneTotals;
    std::unordered_map<std::string, std::array<long long, 24>> slotTotals;

public:
    // Core Functions
    void ingestStdin();
    void ingestFile(const std::string& csvPath);

    // Reporting Functions (with deterministic tie-breaking)
    std::vector<ZoneCount> topZones(int k = 10) const;
    std::vector<SlotCount> topBusySlots(int k = 10) const;
};

#endif
