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

bool operator<(const ResCountDocId &lhs, const ResCountDocId &rhs) {
    return lhs.count < rhs.count || (lhs.count == rhs.count && lhs.doc_id > rhs.doc_id);
}

ostream &operator<<(ostream &os, const ResCountDocId &r) {
    return os << " {"
              << "docid: " << r.doc_id << ", "
              << "hitcount: " << r.count << '}';
}

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

void SearchServer::MergeWordResIntoResByDocument(array<Count, MAX_DOC_AMOUNT> &to, const map<DocId, Count> &from) {
    if (from.empty()) { return; }
    for (auto[doc_id, count_of_word] : from) {
        to[doc_id] += count_of_word;
    }
}

void SearchServer::InsertToSortedArray(array<ResCountDocId, 5> &to, ResCountDocId cur_count_docId) {
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

void SearchServer::AddQueriesStream(istream &query_input, ostream &search_results_output) {
    array<Count, MAX_DOC_AMOUNT> res_counts_by_document;
    for (string current_query; getline(query_input, current_query);) {
        res_counts_by_document.fill(0);

        const auto words = SplitIntoWords(current_query);

        for (const auto &word : words) {
            const map<DocId, Count> &docid_count_word = index.Lookup(word);
            MergeWordResIntoResByDocument(res_counts_by_document, docid_count_word);
        }

        const ResCountDocId absolute_min = {0, static_cast<DocId>(-1)};
        array<ResCountDocId, 5> res = {{absolute_min, absolute_min, absolute_min, absolute_min, absolute_min}};

        for (int doc_id = 0; doc_id < DocsSize(); doc_id++) {
            ResCountDocId cur_docId_count_for_word{res_counts_by_document[doc_id], static_cast<DocId>(doc_id)};
            if (cur_docId_count_for_word < res[0] || cur_docId_count_for_word.count == 0) { continue; }

            InsertToSortedArray(res, cur_docId_count_for_word);
        }

        search_results_output << current_query << ':';
        for (int i = 4; i >= 0 && res[i].doc_id != static_cast<DocId>(-1); i--) {
            search_results_output << res[i];
        }
        search_results_output << endl;
    }
}


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
