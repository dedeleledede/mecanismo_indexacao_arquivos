#include <iostream>
#include <string>

#include "index.h"
#include "indexer.h"
#include "text_processor.h"
#include "serializer.h"
#include "query_processor.h"

using namespace std;

enum Modo {construir, busca, invalid};
Modo parse(const char* s){
    string modo = s;
    if (modo == "construir") return construir;
    if (modo == "busca") return busca;
    return invalid;
}

int main(int argc, char* argv[]){

    if (argc < 2){
        cerr << "ERRO: Argumentos insuficientes\n(Modelo de uso: indice modo(construir/busca) <dir>/<termo>)" << endl;
        return -1;
    }

    Modo m = parse(argv[1]);

    switch(m){
        case construir:{        
            if (argc < 3){
                cerr << "ERRO: Argumentos insuficientes\n(indice construir <dir>)" << endl;
                return -1;
            }

            cout << "entrando em modo construcao com dir " << argv[2] << endl;
            //indexacao aqui
            //criar objetos, varrer diretorio, construir indice, salvar index.dat etc

            string dir = argv[2];
            Index index;
            TextProcessor text_processor;
            text_processor.load_stopwords("../data/stopwords.txt");

            Indexer indexer(index, text_processor);
            indexer.build(dir);

            bool saved = Serializer::save(index, "../data/index.dat");

            if (!saved){
                cerr << "ERRO: Falha ao salvar o indice em index.dat" << endl;
                return -1;
            }

            break;
        }

        case busca:{
            if (argc < 3){
                cerr << "ERRO: Argumentos insuficientes\n(indice busca <termo1> <termo2> ...)" << endl;
                return -1;
            }

            cout << "entrando em modo busca com argumentos: ";
            
            string query;

            for(int i = 2; i < argc; i++){
                cout << argv[i] << " ";
                if (query.size() > 0)
                    query += " ";
                query += argv[i];
            }
            cout << "\n";
            
            Index index;
            
            TextProcessor text_processor;
            text_processor.load_stopwords("../data/stopwords.txt");
            
            vector<string> terms = text_processor.process(query);
            
            bool loaded = Serializer::load(index, "../data/index.dat");
            
            if (!loaded){
                cerr << "ERRO: indice.dat nao encontrado. Rode a indexacao primeiro." << endl;
                return -1;
            }
            
            QueryProcessor query_processor(index);
            auto doc_ids = query_processor.process_terms(terms);

            if (doc_ids.empty()){
                cout << "Nenhum documento encontrado para essa busca." << endl;
            }
            else {
                cout << "Documentos encontrados:" << endl;
                for (int id : doc_ids){
                    string path = index.get_filepath(id);

                    if (!path.empty())
                        cout << path << endl;
                }
            }

            break;
        }
        default:
            cerr << "ERRO: Argumentos de modo invalidos \n(modelo: indice modo(construir/busca) <dir>/<termo>)" << endl;
            return -1;
    }
    return 0;
}   