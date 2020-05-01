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
using Id = size_t;

class InvertedIndex {
public:
    void Add(string &&document);

    const map<Id, Count> &Lookup(const string &word) const;

    const string &GetDocument(Id id) const {
        return docs[id];
    }

private:
    map<string, map<Id, Count>> index;
    vector<string> docs;
    static const map<Id, Count> EMPTY;  // todo
};

class SearchServer {
public:
    SearchServer() = default;

    explicit SearchServer(istream &document_input);

    void UpdateDocumentBase(istream &document_input);

    void AddQueriesStream(istream &query_input, ostream &search_results_output);

private:
    void MergeMap(map<Id, Count>& to, const map<Id, Count>& from);

private:
    InvertedIndex index;
};
