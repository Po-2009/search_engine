#pragma once
#include <strstream>
#include <vector>
#include <iostream>
#include <map>
#include <thread>
#include <mutex>
#include <sstream>

struct Entry{
    size_t doc_id, count;

    bool operator ==(const Entry& other) const;
};

class InvertedIndex{
public:
    std::vector<std::string> docs;
    std::map<std::string, std::vector<Entry>> freq_dictionary;


    InvertedIndex() = default;
    void UpdateDocumentBase(const std::vector<std::string>& input_docs) noexcept;

    static void ProcessDocument(const std::string& word, const std::string& doc, size_t doc_id, std::mutex& mtx, std::vector<Entry>& all) noexcept;

    std::vector<Entry> GetWordCount(const std::string& word) noexcept;

};
