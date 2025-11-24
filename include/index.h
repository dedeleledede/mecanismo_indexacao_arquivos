#pragma once

#include <set>
#include <string>
#include <vector>
#include <unordered_map>

//using namespace std;

class Index{
private:
  std::unordered_map<std::string, std::set<int>> inverted_index_map; //word > id set
  std::unordered_map<std::string, int> path_to_id; // path > id
  std::unordered_map<int, std::string> id_to_path; // id > path

  int create_doc(const std::string& filepath);

public:
  Index() = default;  
  using doc_set_id = std::set<int>;

  void add_word(const std::string& word, const std::string& filepath);
  
  //getters
  std::string get_filepath(int doc_id) const;
  
  const doc_set_id& get_docs_from_word(const std::string& word) const;
  
  const std::unordered_map<std::string, doc_set_id>& get_inverted_index() const;  
  
  const std::unordered_map<std::string, int>& get_filepath_to_id() const;
  
  const std::unordered_map<int, std::string>& get_id_to_filepath() const;

  //setters
  void set_inverted_index(const std::unordered_map<std::string, doc_set_id>& mapa);

  void set_doc_maps(const std::unordered_map<std::string, int>& filepath_to_id, const std::unordered_map<int, std::string>& id_to_filepath);
};