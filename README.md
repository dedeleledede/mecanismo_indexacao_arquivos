# Construção de Um Mecanismo de Indexação de Arquivos

## O programa tem dois modos:

``indice construir <caminho_do_diretorio>``
``indice buscar <termo1> <termo2> ... <termoN>``

## Blocos principais de responsabilidade são:

- Index - guarda o indice invertido + mapeamento ID <> arquivo
- Text Processor - limpa texto, tokeniza, tira pontuação/stopwords
- Indexer - percorre diretorio, le arquivos, alimenta o Index
- Serializer - salva/carrega o Index no arquivo binário index.dat
- Query Processor - recebe termos de busca e devolve arquivos
- Command Line Interface - interpreta argc/argv e chama o resto

processo de pensamento:
como representar o indice invertido em c++ usando stl
tipo da chave tipo do valor
como mapearia nome_arquivo > id e id > nome_arquivo

construir ambiente basico e makefile compilar "hello from index world"

mecanismo_indexacao_arquivos/
  src/
    main.cpp
    index.cpp
    indexer.cpp
    text_processor.cpp
    serializer.cpp
    query_processor.cpp
    cli.cpp
  include/
    index.h
    indexer.h
    text_processor.h
    serializer.h
    query_processor.h
    cli.h
  machado/
  Makefile
  stopwords.txt

  - commit 1