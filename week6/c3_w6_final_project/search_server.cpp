#include "search_server.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <sstream>
#include <iostream>


vector<string> SplitIntoWords(const string &line) {
    istringstream words_input(line);
    return {istream_iterator<string>(words_input), istream_iterator<string>()};
}


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


// ============================== Inverted Index ==============================================

const map<DocId, Count> InvertedIndex::EMPTY = {};
const WordId InvertedIndex::WORD_NOT_PRESENT = -1;

InvertedIndex &InvertedIndex::operator=(InvertedIndex &&other) {
    words = move(other.words);
    index = move(other.index);
    docs = move(other.docs);
    return *this;
}

WordId InvertedIndex::GetWordIndexOrCreate(const string &word) {
    auto it = words.lower_bound(word);
    if (it != words.end() && it->first == word) {
        return it->second;
    } else {
        words.insert(it, {word, words.size()});
        return words.size() - 1;
    }
}

WordId InvertedIndex::GetWordIndexOrMinus1(const string &word) const {
    auto it = words.find(word);
    return it != words.end() ? it->second : WORD_NOT_PRESENT;
}

void InvertedIndex::Add(string &&document) {
    docs.push_back(move(document));

    const DocId docid = docs.size() - 1;
    for (string &word : SplitIntoWords(docs.back())) {
        WordId word_id = GetWordIndexOrCreate(word);

        // todo rework for vector of pairs;
        map<DocId, Count> &counts_of_cur_word = index[word_id];

        if (!counts_of_cur_word.empty() && prev(counts_of_cur_word.end())->first == docid) {
            prev(counts_of_cur_word.end())->second++;  // increase
        } else {
            counts_of_cur_word.insert(counts_of_cur_word.end(), {docid, 1});  // or insert
        }
    }
}

const map<DocId, Count> &InvertedIndex::Lookup(const string &word) const {
    WordId word_id = GetWordIndexOrMinus1(word);
    return (word_id != WORD_NOT_PRESENT) ? index[word_id] : EMPTY;
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


array<Count, SearchServer::MAX_DOC_AMOUNT> SearchServer::CalculateMetricsForEachDocument(const string &current_query) {
    array<Count, SearchServer::MAX_DOC_AMOUNT> res_counts_by_document;
    res_counts_by_document.fill(0);

    for (const auto &word : SplitIntoWords(current_query)) {
        // для каждого слова из запроса
        const map<DocId, Count> &counts_of_current_word = index.Lookup(word);
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

array<ResCountDocId, 5> SearchServer::getTop5(const array<Count, SearchServer::MAX_DOC_AMOUNT> &res_counts_by_document) {

    // Результат. Формируется как отсортированный массив из топ пяти документов среди обработанных.
    // Новый документ при добавлении вставляется на нужную позицию, отсортированность сохраняется.
    array<ResCountDocId, 5> res = {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}};

    for (int doc_id = 0; doc_id < DocsSize(); doc_id++) {
        ResCountDocId res_for_current_document{res_counts_by_document[doc_id], static_cast<DocId>(doc_id)};

        if (res_for_current_document < res[0] /* если текущий документ по метрике меньше, чем последний в отсортированном массиве топ-5 - игнорируем */
            || res_for_current_document.hitcount_metric == 0 /* если метрика равна 0 - игнорируем */ ) { continue; }

        // иначе вставляем документ в массив на нужную позицию, чтоб сохранить отсортированность. Топ-6 документ при такой вставке удаляется.
        InsertIntoSortedArray5(res, res_for_current_document);
    }
    return res;
}

array<ResCountDocId, 5> SearchServer::ServeOneQuery(const string &current_query) {
    array<Count, MAX_DOC_AMOUNT> hitcounts_by_document = CalculateMetricsForEachDocument(current_query);

    array<ResCountDocId, 5> res_top_5 = getTop5(hitcounts_by_document);

    return res_top_5;
}

void SearchServer::AddQueriesStream(istream &query_input, ostream &search_results_output) {
    for (string current_query; getline(query_input, current_query);) {
        array<ResCountDocId, 5> res = ServeOneQuery(current_query);

        search_results_output << current_query << ':';
        for (int i = 4; i >= 0 && res[i].hitcount_metric != 0; i--) {
            search_results_output << res[i];
        }
        search_results_output << endl;
    }
}

