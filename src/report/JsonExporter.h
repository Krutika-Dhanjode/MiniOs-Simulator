#ifndef MINIOS_JSON_EXPORTER_H
#define MINIOS_JSON_EXPORTER_H

#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "report/JsonExportable.h"

// Aggregates JSON sections from all subsystems and writes a combined report.
class JsonExporter {
public:
    void addSection(const std::string& name, const JsonExportable& obj) {
        sections_.emplace_back(name, obj.toJson());
    }

    std::string build() const {
        std::ostringstream oss;
        oss << "{\n";
        for (std::size_t i = 0; i < sections_.size(); ++i) {
            oss << "  \"" << sections_[i].first << "\": " << sections_[i].second;
            if (i + 1 < sections_.size()) oss << ",";
            oss << "\n";
        }
        oss << "}\n";
        return oss.str();
    }

    bool writeToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        file << build();
        return true;
    }

private:
    std::vector<std::pair<std::string, std::string>> sections_;
};

#endif // MINIOS_JSON_EXPORTER_H
