#include "../include/SearchServer.h"


std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string> &queries_input) noexcept{
    std::vector<std::vector<RelativeIndex>> result;

    for (auto &i: queries_input) {
        std::vector<Entry> document_entry;
        std::set<size_t> document_number;

        std::stringstream ss(i);
        std::string one_word;
        int words_count =0;
        bool is_find = true;
        while (ss >> one_word) {
            words_count++;
            bool check = false;
            for (auto &j: _index.freq_dictionary[one_word]) {
                check = true;
                document_number.insert(j.doc_id);
                document_entry.push_back(j);
            }
            if (!check) {
                is_find = false;
                break;
            }
        }
        if (!is_find) {
            result.emplace_back();
        } else {

            std::vector<RelativeIndex> document_relative_index;
            std::vector<Entry> all_find;

            size_t find_sum =0;
            for(size_t j : document_number){
                for(auto& g : document_entry){
                    if(g.doc_id == j){
                        find_sum += g.count;
                    }
                }
                size_t t_j = j;
                size_t t_sum = find_sum;
                all_find.push_back({t_j, t_sum});
                find_sum = 0;
            }
            int max_responses = ConverterJSON::GetResponsesLimit();
            size_t max_count= all_find[0].count;

            for(auto& j : all_find){
                if(j.count>max_count) max_count = j.count;
            }

            for(auto& j : all_find){
                float rank = (float) j.count / (float) max_count;
                auto* relativeIndex = new RelativeIndex;
                relativeIndex->doc_id = j.doc_id;
                relativeIndex->rank = rank;
                document_relative_index.push_back(*relativeIndex);
                delete relativeIndex;
            }


            std::sort(document_relative_index.begin(), document_relative_index.end(),[](const RelativeIndex& a_1, const RelativeIndex& a_2){
                return a_1.rank > a_2.rank;
            });


            if (document_relative_index.size() > max_responses){
                document_relative_index.erase(document_relative_index.begin()+max_responses, document_relative_index.end());
            }
            
            result.push_back(document_relative_index);
        }
    }
    return result;
}