#pragma once
#include <vector>
#include <deque>
#include "document.h"
#include "search_server.h"

const int MINUTES_IN_DAY = 1440;

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    int GetNoResultRequests() const;

private:
    struct QueryResult {
        int timestamp;
        bool no_results;
    };
    std::deque<QueryResult> requests_;
    const SearchServer& search_server_;
    int current_time_ = 0;
    int no_result_requests_ = 0;

    void AddRequest(int results_count);
};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    const auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
    AddRequest(result.size());
    return result;
}
