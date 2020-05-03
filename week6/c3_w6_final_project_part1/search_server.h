#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>

#include "inverted_index.h"
#include "utils.h"

using namespace std;


// ============================== struct ResCountDocId ==============================================
// Пара из "метрики релевантности" и "ID документа"

struct ResCountDocId {
    Count hitcount_metric;
    DocId doc_id;
};

bool operator<(const ResCountDocId &lhs, const ResCountDocId &rhs);

ostream &operator<<(ostream &os, const ResCountDocId &lhs);




// ============================== Search Server ==============================================

class SearchServer {
public:
    SearchServer() = default;

    explicit SearchServer(istream &document_input);

    void UpdateDocumentBase(istream &document_input);

    void AddQueriesStream(istream &query_input, ostream &search_results_output) const;

    [[nodiscard]] DocId DocsSize() const { return index.DocsSize(); }

private:
    static const DocId MAX_DOC_AMOUNT = 50'000;

    InvertedIndex index;

    [[nodiscard]] array<Count, MAX_DOC_AMOUNT> CalculateMetricsForEachDocument(const string &current_query) const;

    static void InsertIntoSortedArray5(array<ResCountDocId, 5> &to, ResCountDocId cur_count_docId);

    static array<ResCountDocId, 5> getTop5(const array<Count, MAX_DOC_AMOUNT> &hitcounts_by_document, size_t array_size);

    [[nodiscard]] array<ResCountDocId, 5> ServeOneQuery(const string &current_query) const;

    static void OutputOnequery(ostream &search_results_output, const string &current_query, const array<ResCountDocId, 5> &res);
};
