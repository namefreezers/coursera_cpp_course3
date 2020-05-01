#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>


vector<string> SplitIntoWords(const string &line) {
    istringstream words_input(line);
    return {istream_iterator<string>(words_input), istream_iterator<string>()};
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

void SearchServer::MergeMap(map<Id, Count> &to, const map<Id, Count> &from) {
    if (from.empty()) { return; }
    for (auto[k, v] : from) {
        auto it_to = to.lower_bound(k);
        if (it_to != to.end() && it_to->first == k) {
            it_to->second += v;
        } else {
            to.insert(it_to, {k, v});
        }
    }
}

void SearchServer::AddQueriesStream(istream &query_input, ostream &search_results_output) {
    for (string current_query; getline(query_input, current_query);) {
        const auto words = SplitIntoWords(current_query);

        map<Id, Count> docid_count;
        for (const auto &word : words) {
            const map<Id, Count>& docid_count_word = index.Lookup(word);
            MergeMap(docid_count, docid_count_word);
        }

        vector<pair<size_t, size_t>> search_results(
                docid_count.begin(), docid_count.end()
        );
        sort(
                begin(search_results),
                end(search_results),
                [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
                    int64_t lhs_docid = lhs.first;
                    auto lhs_hit_count = lhs.second;
                    int64_t rhs_docid = rhs.first;
                    auto rhs_hit_count = rhs.second;
                    return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
                }
        );

        search_results_output << current_query << ':';
        for (auto[docid, hitcount] : Head(search_results, 5)) {
            search_results_output << " {"
                                  << "docid: " << docid << ", "
                                  << "hitcount: " << hitcount << '}';
        }
        search_results_output << endl;
    }
}


const map<Id, Count> InvertedIndex::EMPTY = {};

void InvertedIndex::Add(string &&document) {
    docs.push_back(move(document));

    const Id docid = docs.size() - 1;
    for (string &word : SplitIntoWords(docs.back())) {
        map<Id, Count> &counts_of_cur_word = index[move(word)];

        if (!counts_of_cur_word.empty() && prev(counts_of_cur_word.end())->first == docid) {
            prev(counts_of_cur_word.end())->second++;  // increase
        } else {
            counts_of_cur_word.insert(counts_of_cur_word.end(), {docid, 1});  // or insert
        }
    }
}

const map<Id, Count> &InvertedIndex::Lookup(const string &word) const {
    if (auto it = index.find(word); it != index.end()) {
        return it->second;
    } else {
        return EMPTY;
    }
}
