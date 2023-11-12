#pragma once
#include "search_server.h"
#include <deque>
using namespace std;
class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server) : server_(search_server) {
        // напишите реализацию   

    }
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
        // напишите реализацию

        auto search_result = server_.FindTopDocuments(raw_query, document_predicate);
        if (!search_result.empty())
        {
            requests_.push_back({ false });
        }
        else requests_.push_back({ true });
        if (requests_.size() > min_in_day_)
            requests_.pop_front();
        return search_result;
    }
    vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status);
    vector<Document> AddFindRequest(const string& raw_query);

    int GetNoResultRequests() const;

private:
    struct QueryResult {
        // определите, что должно быть в структуре
        bool empty = true;
    };
    deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& server_;
    // возможно, здесь вам понадобится что-то ещё
};
