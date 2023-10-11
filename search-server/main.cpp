#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string::size_type start = 0;
    while (start < text.size()) {
        string::size_type space = text.find(' ', start);
        if (space != string::npos) {
            words.push_back(text.substr(start, space - start));
            start = space + 1;
        } else {
            words.push_back(text.substr(start));
            break;
        }
    }
    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    SearchServer() : document_count_(0) {}

    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        unordered_map<string, int> word_counts;
        for (const string& word : words) {
            inverted_index_[word].insert(document_id);
            word_counts[word]++;
        }
        for (const auto& [word, count] : word_counts) {
            word_frequencies_[document_id][word] = static_cast<double>(count) / words.size();
        }
        document_count_++;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const auto query_words_pair = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words_pair);
        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    unordered_map<string, unordered_set<int>> inverted_index_;
    unordered_set<string> stop_words_;
    int document_count_;
    unordered_map<int, unordered_map<string, double>> word_frequencies_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    pair<unordered_set<string>, unordered_set<string>> ParseQuery(const string& text) const {
        unordered_set<string> plus_words;
        unordered_set<string> minus_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-') {
                minus_words.insert(word.substr(1));
            } else {
                plus_words.insert(word);
            }
        }
        return {plus_words, minus_words};
    }

    double ComputeIDF(const string& word) const {
        auto it = inverted_index_.find(word);
        if (it != inverted_index_.end()) {
            double doc_count_with_word = it->second.size();
            return log(document_count_ / doc_count_with_word);
        }
        return 0.0;
    }

    vector<Document> FindAllDocuments(const pair<unordered_set<string>, unordered_set<string>>& query_words_pair) const {
        unordered_map<int, double> doc_relevance;
        for (const string& word : query_words_pair.first) {
            auto it = inverted_index_.find(word);
            if (it != inverted_index_.end()) {
                double idf = ComputeIDF(word);
                for (const int doc_id : it->second) {
                    auto freq_it = word_frequencies_.find(doc_id);
                    if (freq_it != word_frequencies_.end()) {
                        auto word_freq_it = freq_it->second.find(word);
                        if (word_freq_it != freq_it->second.end()) {
                            doc_relevance[doc_id] += word_freq_it->second * idf;
                        }
                    }
                }
            }
        }

        for (const string& word : query_words_pair.second) {
            auto it = inverted_index_.find(word);
            if (it != inverted_index_.end()) {
                for (const int doc_id : it->second) {
                    doc_relevance.erase(doc_id);
                }
            }
        }

        vector<Document> matched_documents;
        matched_documents.reserve(doc_relevance.size());
        for (const auto& [doc_id, relevance] : doc_relevance) {
            matched_documents.push_back({doc_id, relevance});
        }
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& document : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document.id << ", "
             << "relevance = "s << document.relevance << " }"s << endl;
    }
}
