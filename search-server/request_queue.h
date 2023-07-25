#pragma once
#include <vector>
#include <deque>
#include "search_server.h"



class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server) : curr_serv(search_server)
    {

    }
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    int GetNoResultRequests() const;
private:
    struct QueryResult {
        QueryResult(bool state)
        {
            isEmptyreq = state;
        }
        // определите, что должно быть в структуре
        bool isEmptyreq;

    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& curr_serv;

    // возможно, здесь вам понадобится что-то ещё
};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {

    auto result_req = curr_serv.FindTopDocuments(raw_query, document_predicate);
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
