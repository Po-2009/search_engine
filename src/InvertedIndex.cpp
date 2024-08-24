#include "../include/InvertedIndex.h"


bool Entry::operator==(const Entry &other) const {
    return (doc_id == other.doc_id && count == other.count);
}


void InvertedIndex::UpdateDocumentBase(const std::vector<std::string> &input_docs) noexcept {
    docs.clear();
    for(auto& i : input_docs){
        docs.push_back(i);
    }

    for(auto& i : docs){
        std::stringstream v(i);
        std::string word;
        while(v >> word) {
            if(freq_dictionary.find(word) == freq_dictionary.end()) {
                std::vector<Entry> wordCount = GetWordCount(word);
                std::pair<std::string, std::vector<Entry>> pair = {word, wordCount};
                freq_dictionary.insert(pair);
            }
        }
    }
}

void InvertedIndex::ProcessDocument(const std::string &word, const std::string &doc, size_t doc_id, std::mutex &mtx,
                                    std::vector<Entry> &all) noexcept {
    std::vector<Entry> result;
    std::stringstream ss(doc);
    std::string one_word;
    size_t count = 0;
    while (ss >> one_word) {
        if (one_word == word) {
            count++;
        }
    }
    if (count > 0) {
        auto* new_entry = new Entry;
        new_entry->doc_id = doc_id;
        new_entry->count = count;
        std::lock_guard<std::mutex> lock(mtx);
        all.push_back(*new_entry);
        delete new_entry;
    }
}


std::vector<Entry> InvertedIndex::GetWordCount(const std::string &word) noexcept {
    std::vector<Entry> all;
    std::mutex mtx;

    std::vector<std::thread> threads;
    threads.reserve(docs.size());

    for (size_t i = 0; i < docs.size(); ++i) {
        threads.emplace_back(ProcessDocument, word, docs[i], i, std::ref(mtx), std::ref(all));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::sort(all.begin(),all.end(), [](const Entry& a1, const Entry& a2){
        return a1.doc_id < a2.doc_id;
    });

    return all;
}