#include "serializer.h"

#include <vector>
#include <fstream>
#include <cstdint>
#include <unordered_map>

using namespace std;

namespace {

    template<typename T>
    bool write_pod(ofstream& out, const T& value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(T));
        return out.good();
    }
    
    template<typename T>
    bool read_pod(ifstream& in, T& value) {
        in.read(reinterpret_cast<char*>(&value), sizeof(T));
        return in.good();
    }
    
    bool write_string(ofstream& out, const string& s) {
        uint32_t len = static_cast<uint32_t>(s.size());
        if (!write_pod(out, len)) return false;
        out.write(s.data(), len);
        return out.good();
    }
    
    bool read_string(ifstream& in, string& s) {
        uint32_t len = 0;
        if (!read_pod(in, len)) return false;
        s.resize(len);
        if (len > 0) {
            in.read(&s[0], len);
            if (!in.good()) return false;
        }
        return true;
    }
    
    }

bool Serializer::save(const Index& index, const string& filename) {
    ofstream out(filename, ios::binary);
    if (!out.is_open()) {
        return false;
    }

    const auto& id_to_filepath = index.get_id_to_filepath();
    
    uint32_t doc_count = static_cast<uint32_t>(id_to_filepath.size());
    if (!write_pod(out, doc_count))
        return false;

    for (const auto& [doc_id, path] : id_to_filepath) {
        int32_t id32 = static_cast<uint32_t>(doc_id);

        if (!write_pod(out, id32))
            return false;

        if (!write_string(out, path))
            return false;
    }
    
    const auto& inverted = index.get_inverted_index();
    uint32_t term_count = static_cast<uint32_t>(inverted.size());

    if (!write_pod(out, term_count))
        return false;

    for (const auto& [word, doc_set] : inverted) {
        if (!write_string(out, word))
            return false;
    
        uint32_t docset_size = static_cast<uint32_t>(doc_set.size());

        if (!write_pod(out, docset_size))
            return false;

        for (int doc_it : doc_set) {
            int32_t id32 = static_cast<uint32_t>(doc_it);

            if(!write_pod(out, id32))
                return false;
        }
    }

    return out.good();
}

bool Serializer::load(Index& index, const string& filename) {
    ifstream in(filename, ios::binary);
    if (!in.is_open()) {
        return false;
    }

    unordered_map<string, int> filepath_to_id;
    unordered_map<int, string> id_to_filepath;
    unordered_map<string, Index::doc_set_id> inverted;

    uint32_t doc_count = 0;
    if (!read_pod(in, doc_count))
        return false;

    // le documentos
    for (uint32_t i = 0; i < doc_count; ++i) {
        uint32_t id32 = 0;
        if (!read_pod(in, id32)) 
            return false;

        string path;

        if (!read_string(in, path)) 
            return false;

        int id = static_cast<int>(id32);
        filepath_to_id[path] = id;
        id_to_filepath[id] = path;
    }

    uint32_t term_count = 0;
    if (!read_pod(in, term_count)) 
        return false;

    // le linhas de termos
    for (uint32_t i = 0; i < term_count; ++i) {
        string word;

        if (!read_string(in, word)) 
            return false;
            
        uint32_t qty;
        if (!read_pod(in, qty)) 
            return false;

        Index::doc_set_id docs;
        for (uint32_t j = 0; j < qty; ++j) {
            uint32_t id32;

            if (!read_pod(in, id32)) 
                return false;

            docs.insert(static_cast<int>(id32));
        }

        inverted[word] = std::move(docs);
    }

    index.set_doc_maps(filepath_to_id, id_to_filepath);
    index.set_inverted_index(inverted);

    return true;
}