#include <nlohmann/json.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <thread>
#include <mutex>
#include <algorithm>
#include <set>
#include <iomanip>



static std::string PathToJsonFiles(std::string build_dir_path){
    std::string config_path;
    std::filesystem::path path = build_dir_path;
    while(true){
        if(path.filename().string() == EXECUTABLE_NAME){
            config_path = path.string();
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
        std::ifstream config_file(PathToJsonFiles(PROJECT_BINARY_DIR) + "/config.json");
        config_file >> config_info;
        std::vector<std::string> all_values;
        for (std::string i : config_info["files"]) {
            std::ifstream file(i);
            std::string text;
            while (!file.eof()){
                std::string new_word;
                file >> new_word;
                text += new_word;
                text += " ";
            }
            all_values.push_back(text);
        }
        return all_values;
    }

    static int GetResponsesLimit(){
        std::ifstream config_file(PathToJsonFiles(PROJECT_BINARY_DIR) + "/config.json");
        nlohmann::json config_info;
        config_file >> config_info;
        int max_responses = config_info["config"]["max_responses"];
        return max_responses;
    }

    static std::vector<std::string> GetRequests(){
        std::ifstream request_file(PathToJsonFiles(PROJECT_BINARY_DIR) + "/requests.json");
        nlohmann::json request_info;
        request_file >> request_info;
        std::vector<std::string> all_requests;
        for(auto& i : request_info["requests"]){
            all_requests.push_back(i);
        }
        return all_requests;
    }

    void putAnswers(std::vector<std::vector<RelativeIndex>> answers){
        nlohmann::json json_file;
        std::ofstream answers_file1(PathToJsonFiles(PROJECT_BINARY_DIR) + "/answers.json");
        for(int i = 0; i < answers.size();i++){
            std::string i_str = std::to_string(i);
            i_str = std::string(3 - i_str.length(), '0') + i_str;
            if(answers[i].size() ==0){
                json_file["answers"]["request" + i_str]["result"] = "false";
            }else if(answers[i].size() ==0){
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

struct Entry{
    size_t doc_id, count;

    bool operator ==(const Entry& other) const{
        return (doc_id == other.doc_id && count == other.count);
    }
};

class InvertedIndex{
public:
    std::vector<std::string> docs;
    std::map<std::string, std::vector<Entry>> freq_dictionary;


    InvertedIndex() = default;
    void UpdateDocumentBase(std::vector<std::string> input_docs){
        docs.clear();
        for(auto& i : input_docs){
            docs.push_back(i);
        }

        for(auto& i : docs){
            std::stringstream v(i);
            std::string word;
            while(v >> word) {
                std::vector<Entry> c = GetWordCount(word);
                std::pair<std::string, std::vector<Entry>> pair = {word, c};
                freq_dictionary.insert(pair);
            }
        }
    }

    static void ProcessDocument(const std::string& word, const std::string& doc, size_t doc_id, std::mutex& mtx, std::vector<Entry>& all) {
        std::vector<Entry> result;
        std::istringstream ss(doc);
        std::string one_word;
        size_t count = 0;
        while (ss >> one_word) {
            if (one_word == word) {
                count++;
            }
        }
        if (count > 0) {
            std::lock_guard<std::mutex> lock(mtx);
            all.push_back({doc_id, count});
        }
    }

    std::vector<Entry> GetWordCount(const std::string& word) {
        std::vector<Entry> all;
        std::mutex mtx;

        std::vector<std::thread> threads;

        for (size_t i = 0; i < docs.size(); ++i) {
            threads.emplace_back(ProcessDocument, word, docs[i], i, std::ref(mtx), std::ref(all));
        }

        for (auto& thread : threads) {
            thread.join();
        }

        return all;
    }

};


class SearchServer{
public:
    SearchServer(InvertedIndex& idx) : _index(idx){};

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input) {
        std::vector<std::vector<RelativeIndex>> result;
        for (auto &i: queries_input) {
            std::vector<Entry> document_entrys;
            std::vector<int> documents_number;
            std::stringstream ss(i);
            std::string one_word;
            int words_count =0;
            bool is_find = true;
            while (ss >> one_word) {
                words_count++;
                bool check = false;
                for (auto &j: _index.freq_dictionary[one_word]) {
                    check = true;
                    documents_number.push_back(j.doc_id);
                    document_entrys.push_back(j);
                }
                if (!check) {
                    is_find = false;
                    break;
                }
            }
            if (!is_find) {
                result.push_back({});
            } else {

                std::vector<RelativeIndex> document_relative_index;
                std::vector<Entry> all_find;

                std::set<int> set(documents_number.begin(), documents_number.end());
                int find_summ =0;
                for(int j : set){
                    for(auto& g : document_entrys){
                        if(g.doc_id == j){
                            find_summ += g.count;
                        }
                    }
                    size_t t_j = j;
                    size_t t_summ = find_summ;
                    all_find.push_back({t_j, t_summ});
                    find_summ = 0;
                }

                int count =1;
                int max_responses = ConverterJSON::GetResponsesLimit();
                int max_count=all_find[0].count;

                for(auto& i : all_find){
                    if(i.count>max_count) max_count = i.count;
                }

                for(auto& j : all_find){
                    float rank = (float) j.count / (float) max_count;
                    document_relative_index.push_back({j.doc_id, rank});
                    count++;
                }
                std::sort(document_relative_index.begin(), document_relative_index.end(),[](const RelativeIndex& a_1, const RelativeIndex& a_2){
                    return a_1.rank > a_2.rank;
                });

                if (document_relative_index.size() > 6){
                    document_relative_index.erase(document_relative_index.begin()+5, document_relative_index.end());
                }

                result.push_back(document_relative_index);
            }
        }
        return result;
    }
private:
    InvertedIndex& _index;
};