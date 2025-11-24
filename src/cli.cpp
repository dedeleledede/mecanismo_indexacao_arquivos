#include "cli.h"

#include <iostream>
#include <string>
#include <vector>

#include "index.h"
#include "text_processor.h"
#include "indexer.h"
#include "serializer.h"
#include "query_processor.h"

using namespace std;

enum Modo {
    MODO_CONSTRUIR,
    MODO_BUSCA
};

int run_cli(int argc, char* argv[]) {
    // nenhum modo informado
    if (argc < 2) {
        cerr << "ERRO: argumentos invalidos\n"
             << "(modelo: indice modo(construir/busca) <dir>/<termo>)" << endl;
        return -1;
    }

    string modo_str = argv[1];
    Modo m;

    if (modo_str == "construir") {
        m = MODO_CONSTRUIR;
    } else if (modo_str == "buscar" || modo_str == "busca") {
        m = MODO_BUSCA;
    } else {
        cerr << "ERRO: argumentos invalidos\n"
             << "(modelo: indice modo(construir/busca) <dir>/<termo>)" << endl;
        return -1;
    }

    switch (m) {

    // ---------------- MODO CONSTRUIR ----------------
    case MODO_CONSTRUIR: {
        if (argc < 3) {
            cerr << "ERRO: Argumentos insuficientes\n(indice construir <dir>)" << endl;
            return -1;
        }

        cout << "Entrando em modo construcao com dir: " << argv[2] << endl;

        string dir = argv[2];

        Index index;
        TextProcessor text_processor;
        text_processor.load_stopwords("data/stopwords.txt");

        Indexer indexer(index, text_processor);
        indexer.build(dir);

        // salva o indice em disco
        if (!Serializer::save(index, "data/index.dat")) {
            cerr << "ERRO: nao foi possivel salvar o indice em ../data/index.dat" << endl;
            return -1;
        }

        cout << "Indice salvo em data/index.dat" << endl;
        break;
    }

    // ---------------- MODO BUSCA ----------------
    case MODO_BUSCA: {
        if (argc < 3) {
            cerr << "ERRO: Argumentos insuficientes\n(indice busca <termo1> <termo2> ...)" << endl;
            return -1;
        }

        cout << "Entrando em modo busca com argumentos: ";

        string query;
        for (int i = 2; i < argc; ++i) {
            cout << argv[i] << ' ';
            if (!query.empty()) {
                query += ' ';
            }
            query += argv[i];
        }
        cout << endl;

        // carrega o indice de disco
        Index index;
        if (!Serializer::load(index, "data/index.dat")) {
            cerr << "ERRO: indice /data/index.dat nao encontrado. Rode a indexacao primeiro." << endl;
            return -1;
        }

        // processa a query (normalizacao + stopwords)
        TextProcessor text_processor;
        text_processor.load_stopwords("../data/stopwords.txt");
        vector<string> terms = text_processor.process(query);

        if (terms.empty()) {
            cout << "Nenhum termo de busca valido apos normalizacao/stopwords." << endl;
            return 0;
        }

        // executa a busca
        QueryProcessor query_processor(index);
        Index::doc_set_id doc_ids = query_processor.process_terms(terms);

        if (doc_ids.empty()) {
            cout << "Nenhum documento encontrado para essa busca." << endl;
        } else {
            cout << "Documentos encontrados:" << endl;
            for (int id : doc_ids) {
                string path = index.get_filepath(id);
                if (!path.empty()) {
                    cout << path << endl;
                }
            }
        }

        break;
    }
    }

    return 0;
}