#pragma once
#include "nlohmann/json.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>
#include <iomanip>
#include "JsonConverter.h"
#include "InvertedIndex.h"


class SearchServer{
public:
    explicit SearchServer(InvertedIndex& idx) : _index(idx){};

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input) noexcept;
private:
    InvertedIndex& _index;
};