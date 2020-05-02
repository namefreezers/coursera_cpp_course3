#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"

#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>

using namespace std;

using Count = int;
using WordId = uint16_t;
using DocId = uint16_t;

struct ResCountDocId {
    Count count;
    DocId doc_id;
};

bool operator<(const ResCountDocId &lhs, const ResCountDocId &rhs);

ostream &operator<<(ostream &os, const ResCountDocId &lhs);

class InvertedIndex {
public:
    void Add(string &&document);

    const map<DocId, Count> &Lookup(const string &word) const;

    const string &GetDocument(DocId id) const { return docs[id]; }

    DocId DocsSize() const { return docs.size(); }

private:
    WordId GetWordIndexOrCreate(const string &word);

    WordId GetWordIndexOrMinus1(const string &word) const;

private:
    map<string, WordId> words;
    vector<map<DocId, Count>> index = vector<map<DocId, Count>>(10000);  // index - WordId

    vector<string> docs;

    static const map<DocId, Count> EMPTY;  // todo
    static const WordId WORD_NOT_PRESENT;
};

class SearchServer {
public:
    SearchServer() = default;

    explicit SearchServer(istream &document_input);

    void UpdateDocumentBase(istream &document_input);

    void AddQueriesStream(istream &query_input, ostream &search_results_output);

    DocId DocsSize() const { return index.DocsSize(); }

private:
    InvertedIndex index;
    static const DocId MAX_DOC_AMOUNT = 50'000;

    void MergeRes(array<Count, MAX_DOC_AMOUNT> &to, const map<DocId, Count> &from);

    void InsertToSortedArray(array<ResCountDocId, 5> &to, ResCountDocId cur_count_docId);
};

#pragma clang diagnostic pop