#include "indexer.h"
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

Indexer::Indexer(Index& idx, TextProcessor& tp) : index(idx), text_processor(tp) {}

bool Indexer::process_file(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) 
        return false;

    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    auto words = text_processor.process(content);

    for (const auto& word : words) 
        index.add_word(word, filepath);

    return true;
}

bool Indexer::build(const string& root_dir) {
    if (!fs::exists(root_dir)) {
        cerr << "ERRO: diretorio nao existe: " << root_dir << endl;
        return false;
    }

    int files_processed = 0;
    int files_failed = 0;

    for (const auto& entry : fs::recursive_directory_iterator(root_dir)) {
        if (entry.is_regular_file()) {
            const string path = entry.path().string();

            if (process_file(path)) {
                ++files_processed;
            }
            else{
                ++files_failed;
                cerr << "AVISO: nao foi possivel processar arquivo: " << path << endl;
            }
        }
    }

    if (files_processed == 0) {
        cerr << "AVISO: nenhum arquivo de texto foi indexado em: " << root_dir << endl;
        return false;
    }

    return true;
}
