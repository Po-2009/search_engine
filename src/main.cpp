#include "../include/SearchServer.h"
#include "../include/InvertedIndex.h"
#include "../include/JsonConverter.h"
#include <iostream>
#include <cstdlib>

void process(){
    ConverterJSON convert;
    InvertedIndex idx;
    SearchServer searchServer(idx);

    try{
        convert.GetTextDocuments();
    }
    catch (std::runtime_error ex){
        std::cerr << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::vector<std::string> doc = convert.GetTextDocuments();

    std::vector<std::string> requests = convert.GetRequests();

    idx.UpdateDocumentBase(doc);

    convert.putAnswers(searchServer.search(requests));
}

void check(std::string path_to_config){

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
        check("../JsonFiles/config.json");
    }
    catch (std::runtime_error ex){
        std::cerr << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return 0;
}
