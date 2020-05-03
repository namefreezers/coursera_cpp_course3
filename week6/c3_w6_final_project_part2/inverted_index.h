#pragma once

#include <map>
#include <string>
#include <vector>

#include "utils.h"

using namespace std;


// ============================== Inverted Index ==============================================

class InvertedIndex {
public:
    struct WordDocumentStats {
        DocId doc_id;
        Count hitcount;
    };

    InvertedIndex() {
        index.reserve(EMPIRICAL_MAX_WORD_AMOUNT);
    }

    InvertedIndex &operator=(InvertedIndex &&);

    void Add(string &&document);

    [[nodiscard]] const vector<WordDocumentStats> &Lookup(const string &word) const;

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
    vector<vector<WordDocumentStats>> index;  // index - WordId.

    static const vector<WordDocumentStats> EMPTY;  // todo
    static const WordId WORD_NOT_PRESENT = -1;
    static const WordId EMPIRICAL_MAX_WORD_AMOUNT = 15'000;
    static const Count AVERAGE_DOCUMENTS_INCLUDE_WORD = 50'000 * 1000 / 10'000;

};
