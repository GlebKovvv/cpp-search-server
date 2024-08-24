#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server)
    : search_server_(search_server) {}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    return AddFindRequest(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
        return document_status == status;
    });
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    return AddFindRequest(raw_query, DocumentStatus::ACTUAL);
}

int RequestQueue::GetNoResultRequests() const {
    return no_result_requests_;
}

void RequestQueue::AddRequest(int results_count) {
    ++current_time_;
    bool no_results = (results_count == 0);
    requests_.push_back({current_time_, no_results});

    if (no_results) {
        ++no_result_requests_;
    }

    while (!requests_.empty() && requests_.front().timestamp <= current_time_ - MINUTES_IN_DAY) {
        if (requests_.front().no_results) {
            --no_result_requests_;
        }
        requests_.pop_front();
    }
}
