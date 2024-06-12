#include "search_engine.h"
#include <iostream>

void process(){
    ConverterJSON convert;
    InvertedIndex idx;
    SearchServer searchServer(idx);

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
    std::string config_file_path = PathToJsonFiles(PROJECT_BINARY_DIR) + "/config.json";
    try{
        check(config_file_path);
    }
    catch (std::runtime_error ex){
        std::cout << ex.what() << std::endl;
    }



    return 0;
}
