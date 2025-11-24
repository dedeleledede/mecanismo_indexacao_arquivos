#pragma once

#include <string>
#include "index.h"
#include "text_processor.h"

class Indexer {
private:
    Index& index;
    TextProcessor& text_processor;

    bool process_file(const std::string& filepath);

public:
    Indexer(Index& idx, TextProcessor& tp);

    bool build(const std::string& root_dir);
};