#pragma once

#include "nlohmann/json.hpp"
#include <vector>
#include <fstream>




static std::string PathToJsonFiles(std::string &build_dir_path){
    std::string config_path;
    std::filesystem::path path = build_dir_path;
    while(true){
        if(path.filename().string() == EXECUTABLE_NAME){
            config_path = path.string() + "/JsonFiles";
            return config_path;
        }
        path = path.parent_path();
    }
}


struct RelativeIndex{
    size_t doc_id;
    float rank;
    bool operator ==(const RelativeIndex& other) const;
};

class ConverterJSON{
public:
    ConverterJSON() = default;

    static std::vector<std::string> GetTextDocuments();

    static int GetResponsesLimit();

    static std::vector<std::string> GetRequests();

    static void putAnswers(std::vector<std::vector<RelativeIndex>> answers);
};
