#include "request_queue.h"


std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    // напишите реализацию

    auto result_req = curr_serv.FindTopDocuments(raw_query, status);
    if (result_req.empty())
        requests_.push_back(QueryResult(true));
    else
    {
        requests_.push_back(QueryResult(false));
    }
    if (requests_.size() > min_in_day_)
    {
        requests_.pop_front();
    }
    return result_req;

}
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    // напишите реализацию
    auto result_req = curr_serv.FindTopDocuments(raw_query);
    if (result_req.empty())
        requests_.push_back(QueryResult(true));
    else
    {
        requests_.push_back(QueryResult(false));
    }
    if (requests_.size() > min_in_day_)
    {
        requests_.pop_front();
    }
    return result_req;
}
int RequestQueue::GetNoResultRequests() const {
    // напишите реализацию
    int number_empty_req = 0;
    for (auto req : requests_) {
        if (req.isEmptyreq) {
            number_empty_req++;
        }
    }

    return number_empty_req;
}
