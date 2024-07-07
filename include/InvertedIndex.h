#pragma once

#include <vector>
#include <iostream>
#include <map>
#include <thread>
#include <mutex>


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
    void UpdateDocumentBase(const std::vector<std::string>& input_docs){
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
