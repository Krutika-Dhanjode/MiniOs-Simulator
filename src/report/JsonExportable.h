#ifndef MINIOS_JSON_EXPORTABLE_H
#define MINIOS_JSON_EXPORTABLE_H

#include <string>

// Abstract interface for subsystems that can export their state as JSON.
class JsonExportable {
public:
    virtual ~JsonExportable() = default;
    virtual std::string toJson() const = 0;
};

#endif // MINIOS_JSON_EXPORTABLE_H
