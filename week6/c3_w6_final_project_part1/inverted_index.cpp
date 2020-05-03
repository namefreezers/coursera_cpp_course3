#include <vector>
#include <string>

#include "inverted_index.h"
#include "utils.h"

using namespace std;


// ============================== Inverted Index ==============================================

const vector<InvertedIndex::WordDocumentStats> InvertedIndex::EMPTY = {};

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

//        if (word_id >= 12500) {throw out_of_range("line 64: " + to_string(word_id));} // todo remove

        if (word_id >= index.size()) { index.emplace_back(); index.back().reserve(AVERAGE_DOCUMENTS_INCLUDE_WORD); }
        vector<WordDocumentStats> &counts_of_cur_word = index[word_id];

        if (counts_of_cur_word.back().doc_id != docid) {
            counts_of_cur_word.push_back({docid, 0});
        }

        counts_of_cur_word.back().hitcount++;
    }
}

const vector<InvertedIndex::WordDocumentStats> &InvertedIndex::Lookup(const string &word) const {
    WordId word_id = GetWordIndexOrMinus1(word);
    return (word_id != WORD_NOT_PRESENT) ? index[word_id] : EMPTY;
}
