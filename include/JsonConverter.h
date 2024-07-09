#pragma once

#include "nlohmann/json.hpp"
#include <vector>
#include <fstream>



struct RelativeIndex{
    size_t doc_id;
    float rank;
    bool operator ==(const RelativeIndex& other) const;
};

class ConverterJSON{
public:
    ConverterJSON() = default;

    static std::vector<std::string> GetTextDocuments();

    static int GetResponsesLimit() noexcept;

    static std::vector<std::string> GetRequests() noexcept;

    static void putAnswers(std::vector<std::vector<RelativeIndex>> answers) noexcept;
};
