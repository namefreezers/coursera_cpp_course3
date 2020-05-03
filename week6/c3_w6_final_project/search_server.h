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


// ============================== struct ResCountDocId ==============================================
// Пара из "метрики релевантности" и "ID документа"

struct ResCountDocId {
    Count hitcount_metric;
    DocId doc_id;
};

bool operator<(const ResCountDocId &lhs, const ResCountDocId &rhs);

ostream &operator<<(ostream &os, const ResCountDocId &lhs);


// ============================== Inverted Index ==============================================

class InvertedIndex {
public:
    InvertedIndex() {
        index.reserve(MAX_WORD_AMOUNT);
    }

    InvertedIndex &operator=(InvertedIndex &&);

    void Add(string &&document);

    [[nodiscard]] const map<DocId, Count> &Lookup(const string &word) const;

    [[nodiscard]] const string &GetDocument(DocId id) const { return docs[id]; }

    [[nodiscard]] DocId DocsSize() const { return docs.size(); }

private:
    WordId GetWordIndexOrCreate(const string &word);

    [[nodiscard]] WordId GetWordIndexOrMinus1(const string &word) const;

    vector<string> docs;
    // каждому слову сопоставлен индекс WordId, от 0 до 9999.
    // Индекс ново-встретившегося слова после добавления равен (word.size() - 1)
    map<string, WordId> words;
    // Для каждого из 10'000 слов сопоставляем словарь, в котором для каждого документа считаем вхождение слова WordId в него.
    // а именно в index[word_id][doc_id] хранится количество вхождений слов 'word_id' в 'doc_id'
    vector<map<DocId, Count>> index;  // index - WordId.

    static const map<DocId, Count> EMPTY;  // todo
    static const WordId WORD_NOT_PRESENT = -1;
    static const WordId MAX_WORD_AMOUNT = 15'000;
};


// ============================== Search Server ==============================================

class SearchServer {
public:
    SearchServer() = default;

    explicit SearchServer(istream &document_input);

    void UpdateDocumentBase(istream &document_input);

    void AddQueriesStream(istream &query_input, ostream &search_results_output);

    [[nodiscard]] DocId DocsSize() const { return index.DocsSize(); }

private:
    static const DocId MAX_DOC_AMOUNT = 50'000;

    InvertedIndex index;

    array<Count, MAX_DOC_AMOUNT> CalculateMetricsForEachDocument(const string &current_query);

    void InsertIntoSortedArray5(array<ResCountDocId, 5> &to, ResCountDocId cur_count_docId);

    array<ResCountDocId, 5> getTop5(const array<Count, MAX_DOC_AMOUNT> &hitcounts_by_document);

    array<ResCountDocId, 5> ServeOneQuery(const string &current_query);

};
