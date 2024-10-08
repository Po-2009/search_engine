#include "../include/JsonConverter.h"
#include <iostream>
bool RelativeIndex::operator==(const RelativeIndex &other) const {
    return (doc_id == other.doc_id && rank == other.rank);
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    nlohmann::json config_info;
    std::ifstream config_file("../JsonFiles/config.json");
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

int ConverterJSON::GetResponsesLimit() noexcept{

    std::ifstream config_file("../JsonFiles/config.json");
    if(config_file.is_open()) {
        nlohmann::json config_info;
        config_file >> config_info;
        int max_responses = config_info["config"]["max_responses"];
        return max_responses;
    }
    return 5;
}

std::vector<std::string> ConverterJSON::GetRequests() noexcept {
    std::ifstream request_file("../JsonFiles/requests.json");
    nlohmann::json request_info;
    request_file >> request_info;
    std::vector<std::string> all_requests;
    for(auto& i : request_info["requests"]){
        all_requests.push_back(i);
    }
    return all_requests;
}

void ConverterJSON::putAnswers(std::vector<std::vector<RelativeIndex>> answers) noexcept{
    nlohmann::json json_file;
    std::ofstream answers_file("../JsonFiles/answers.json");

    for (int i = 0; i < answers.size(); i++) {
        std::string i_str = std::string(3 - std::to_string(i).length(), '0') + std::to_string(i);

        if (answers[i].empty()) {
            json_file["answers"]["request" + i_str]["result"] = "false";
        } else if (answers[i].size() == 1) {
            json_file["answers"]["request" + i_str]["result"] = "true";
            json_file["answers"]["request" + i_str]["docid"] = answers[i][0].doc_id;
            json_file["answers"]["request" + i_str]["rank"] = std::round(answers[i][0].rank * 1000.0) / 1000.0;
        } else {
            json_file["answers"]["request" + i_str]["result"] = "true";
            for (auto& j : answers[i]) {
                nlohmann::json relevance_entry;
                relevance_entry["docid"] = j.doc_id;
                relevance_entry["rank"] = std::round(j.rank * 1000.0) / 1000.0;
                json_file["answers"]["request" + i_str]["relevance"].push_back(relevance_entry);
            }
        }
    }
    answers_file << json_file.dump(4);
    answers_file.close();
}