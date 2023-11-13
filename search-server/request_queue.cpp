#include "request_queue.h"


vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    // напишите реализацию
    auto search_result = server_.FindTopDocuments(raw_query, status);
    if (!search_result.empty())
    {
        requests_.push_back({ false });
    }
    else requests_.push_back({ true });
    if (requests_.size() > min_in_day_)
        requests_.pop_front();
    return search_result;
}
std::vector<Document>  RequestQueue::AddFindRequest(const string& raw_query) {
    // напишите реализацию
    auto search_result = server_.FindTopDocuments(raw_query);
    if (!search_result.empty())
    {
        requests_.push_back({ false });
    }
    else requests_.push_back({ true });
    if (requests_.size() > min_in_day_)
        requests_.pop_front();
    return search_result;
}
int  RequestQueue::GetNoResultRequests() const {
    // напишите реализацию
    return count_if(requests_.begin(), requests_.end(), [](auto& request)
        {
            if (request.empty)
                return true;

            return false;
     });
}
