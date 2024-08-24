#include "../include/SearchServer.h"
#include <iostream>
#include <cstdlib>

void process(){
    InvertedIndex idx;
    SearchServer searchServer(idx);

    try{
        ConverterJSON::GetTextDocuments();
    }
    catch (std::runtime_error& ex){
        std::cerr << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::vector<std::string> doc = ConverterJSON::GetTextDocuments();

    std::vector<std::string> requests = ConverterJSON::GetRequests();

    idx.UpdateDocumentBase(doc);

    ConverterJSON::putAnswers(searchServer.search(requests));
}

void check(std::string& path_to_config){

    std::ifstream config(path_to_config);
    if(!config.is_open()){
        throw std::runtime_error("config file is empty!");
    }
    nlohmann::json config_json;
    config >> config_json;
    if(config_json["config"].empty()){
        throw std::runtime_error("in config file empty config!");
    }

    std::cout << "Welcome to " << config_json["config"]["name"] << " !" << std::endl;
    process();

}

int main() {


    try{
        std::string path = "../JsonFiles/config.json";
        check(path);
    }
    catch (std::runtime_error& ex){
        std::cerr << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return 0;
}
