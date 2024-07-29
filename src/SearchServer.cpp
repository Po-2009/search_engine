#include "../include/SearchServer.h"


std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string> &queries_input) noexcept{
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
            int max_count= all_find[0].count;

            for(auto& j : all_find){
                if(j.count>max_count) max_count = j.count;
            }

            for(auto& j : all_find){
                float rank = (float) j.count / (float) max_count;
                RelativeIndex relativeIndex;
                relativeIndex.doc_id = j.doc_id;
                relativeIndex.rank = rank;
                document_relative_index.push_back(relativeIndex);
                count++;
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