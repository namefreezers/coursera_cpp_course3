#include "search_server.h"

#include <algorithm>
#include <array>
#include <future>
#include <iostream>



// ============================== struct ResCountDocId ==============================================
// Пара из "метрики релевантности" и "ID документа"

bool operator<(const ResCountDocId &lhs, const ResCountDocId &rhs) {
    return lhs.hitcount_metric < rhs.hitcount_metric || (lhs.hitcount_metric == rhs.hitcount_metric && lhs.doc_id > rhs.doc_id);
}

ostream &operator<<(ostream &os, const ResCountDocId &r) {
    return os << " {"
              << "docid: " << r.doc_id << ", "
              << "hitcount: " << r.hitcount_metric << '}';
}


// ============================== Search Server ==============================================

SearchServer::SearchServer(istream &document_input) {
    UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream &document_input) {
    InvertedIndex new_index;

    for (string current_document; getline(document_input, current_document);) {
        new_index.Add(move(current_document));
    }

    index = move(new_index);
}


array<Count, SearchServer::MAX_DOC_AMOUNT> SearchServer::CalculateMetricsForEachDocument(const string &current_query) const {
    array<Count, SearchServer::MAX_DOC_AMOUNT> res_counts_by_document;
    res_counts_by_document.fill(0);

    for (const auto &word : SplitIntoWords(current_query)) {
        // для каждого слова из запроса
        const vector<InvertedIndex::WordDocumentStats> &counts_of_current_word = index.Lookup(word);
        for (auto[doc_id, count_of_word_in_doc_id] : counts_of_current_word) {
            res_counts_by_document[doc_id] += count_of_word_in_doc_id;
        }
    }
    return res_counts_by_document;
}

void SearchServer::InsertIntoSortedArray5(array<ResCountDocId, 5> &to, ResCountDocId cur_count_docId) {
    int possibly_less_that_idx;
    for (possibly_less_that_idx = 1; possibly_less_that_idx < 5; possibly_less_that_idx++) {
        if (to[possibly_less_that_idx] < cur_count_docId) {
            to[possibly_less_that_idx - 1] = to[possibly_less_that_idx];
        } else {
            break;
        }
    }
    to[possibly_less_that_idx - 1] = cur_count_docId;
}

array<ResCountDocId, 5> SearchServer::getTop5(const array<Count, SearchServer::MAX_DOC_AMOUNT> &hitcounts_by_document, size_t array_size) {

    // Результат. Формируется как отсортированный массив из топ пяти документов среди обработанных.
    // Новый документ при добавлении вставляется на нужную позицию, отсортированность сохраняется.
    array<ResCountDocId, 5> res = {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}};

    for (int doc_id = 0; doc_id < array_size; doc_id++) {
        ResCountDocId res_for_current_document{hitcounts_by_document[doc_id], static_cast<DocId>(doc_id)};

        if (res_for_current_document < res[0] /* если текущий документ по метрике меньше, чем последний в отсортированном массиве топ-5 - игнорируем */
            || res_for_current_document.hitcount_metric == 0 /* если метрика равна 0 - игнорируем */ ) { continue; }

        // иначе вставляем документ в массив на нужную позицию, чтоб сохранить отсортированность. Топ-6 документ при такой вставке удаляется.
        InsertIntoSortedArray5(res, res_for_current_document);
    }
    return res;
}

array<ResCountDocId, 5> SearchServer::ServeOneQuery(const string &current_query) const {
    array<Count, MAX_DOC_AMOUNT> hitcounts_by_document = CalculateMetricsForEachDocument(current_query);

    array<ResCountDocId, 5> res_top_5 = getTop5(hitcounts_by_document, DocsSize());

    return res_top_5;
}

void SearchServer::OutputOnequery(ostream &search_results_output, const string &current_query, const array<ResCountDocId, 5> &res) {
    search_results_output << current_query << ':';
    for (int i = 4; i >= 0 && res[i].hitcount_metric != 0; i--) {
        search_results_output << res[i];
    }
    search_results_output << endl;
}

void SearchServer::AddQueriesStreamSingle(istream &query_input, ostream &search_results_output) const {
    for (string current_query; getline(query_input, current_query);) {
        array<ResCountDocId, 5> res = ServeOneQuery(current_query);

        OutputOnequery(search_results_output, current_query, res);
    }
}

void SearchServer::AddQueriesStream(istream &query_input, ostream &search_results_output) const {
    size_t threads = 6;
    vector<string> queries;
    for (string current_query; getline(query_input, current_query);) {
        queries.push_back(move(current_query));
    }
    vector<vector<array<ResCountDocId, 5>>> results(threads);
    {
        vector<future<void>> futures;
        futures.reserve(threads);

        for (size_t i = 0; i < threads; i++) {
            futures.push_back(async([&queries, &results, i, threads, this] {
                for (size_t q = i; q < queries.size(); q += threads) {
                    results[i].push_back(ServeOneQuery(queries[q]));
                }
            }));
        }
    }

    bool break_flag = false;
    for (size_t i = 0; !break_flag; i++) {
        for (size_t j = 0; j < threads; j++) {
            if (i >= results[j].size()) {
                break_flag = true;
                break;
            }

            OutputOnequery(search_results_output, queries[i * threads + j], results[j][i]);
        }
    }
}

