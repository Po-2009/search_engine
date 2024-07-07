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
    bool operator ==(const RelativeIndex& other) const {
        return (doc_id == other.doc_id && rank == other.rank);
    }
};

class ConverterJSON{
public:
    ConverterJSON() = default;

    static std::vector<std::string> GetTextDocuments() {
        nlohmann::json config_info;
        std::string binary_dir = PROJECT_BINARY_DIR;
        std::ifstream config_file(PathToJsonFiles(binary_dir) + "/config.json");
        config_file >> config_info;
        std::vector<std::string> all_values;
        for (std::string i : config_info["files"]) {
            std::ifstream file(i);
            if(file.is_open()) {

                std::string text;
                while (!file.eof()) {
                    std::string new_word;
                    file >> new_word;
                    text += new_word;
                    text += " ";
                }
                all_values.push_back(text);

            }else{
                throw std::runtime_error("file path is incorrect!");
            }
        }
        return all_values;
    }

    static int GetResponsesLimit(){
        std::string binary_dir = PROJECT_BINARY_DIR;
        std::ifstream config_file(PathToJsonFiles(binary_dir) + "/config.json");
        nlohmann::json config_info;
        config_file >> config_info;
        int max_responses = config_info["config"]["max_responses"];
        return max_responses;
    }

    static std::vector<std::string> GetRequests(){
        std::string binary_dir = PROJECT_BINARY_DIR;
        std::ifstream request_file(PathToJsonFiles(binary_dir) + "/requests.json");
        nlohmann::json request_info;
        request_file >> request_info;
        std::vector<std::string> all_requests;
        for(auto& i : request_info["requests"]){
            all_requests.push_back(i);
        }
        return all_requests;
    }

    static void putAnswers(std::vector<std::vector<RelativeIndex>> answers){
        nlohmann::json json_file;
        std::string binary_dir = PROJECT_BINARY_DIR;
        std::ofstream answers_file1(PathToJsonFiles(binary_dir) + "/answers.json");
        for(int i = 0; i < answers.size();i++){
            std::string i_str = std::to_string(i);
            i_str = std::string(3 - i_str.length(), '0') + i_str;
            if(answers[i].empty()){
                json_file["answers"]["request" + i_str]["result"] = "false";
            }else if(answers[i].empty()){
                json_file["answers"]["request" + i_str]["result"] = "true";
                json_file["answers"]["request" + i_str]["docid"] = answers[i][0].doc_id;
                json_file["answers"]["request" + i_str]["rank"] = answers[i][0].rank;
            }else{
                for(auto& j : answers[i]) {
                    std::pair<std::pair<std::string, int>,std::pair<std::string, float>> all;
                    all.first = {"docid", j.doc_id};
                    all.second.first = "rank";
                    all.second.second = std::round(j.rank * 1000.0) / 1000.0;
                    json_file["answers"]["request" + i_str]["result"] = "true";
                    json_file["answers"]["request" + i_str]["relevance"].emplace_back(all);
                }
            }
        }
        answers_file1 << json_file;

    }
};
