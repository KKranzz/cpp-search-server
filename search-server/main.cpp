#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

struct Query
{
    set<string> plus_words; // word 1 2 7 map <string , set <int> >
    set<string> minus_words;

};

class SearchServer {
public:

    void SetStopWords(const string& text)  {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
       
        double size = words.size();
        map <string, double> word_tf;
    
        for (const auto& word : words) 
        {
            word_tf[word] += 1 / size;
        }

        for (const auto& word : words)
        {
            documents_[word].insert( pair(document_id, word_tf[word]) );
        }

        document_count_++;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query);

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
    int document_count_ = 0;
   
    map <string, map<int, double>> documents_; 

    set<string> stop_words_;

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

    Query ParseQuery(const string& text) const {

        Query processed_request;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-')
                processed_request.minus_words.insert(word.substr(1));
            else
                processed_request.plus_words.insert(word);
        }
        return processed_request;
    }

    double CalculateIDF(const double document_count, const double number_repeat) 
    const {
        return log(document_count / number_repeat);
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        vector<Document> matched_documents;
        map <int, double> no_filtered_docs;
            
        for (const auto& word : query_words.plus_words) {
            if (documents_.count(word) > 0) 
            {
                double idf = CalculateIDF(static_cast<double> (document_count_), static_cast<double>(documents_.at(word).size()));
                for (const auto& [id, tdf] : documents_.at(word))
                {
                    no_filtered_docs[id] += idf * tdf;
                }
            }
        }


        for (const auto& word : query_words.minus_words) {
            if (documents_.count(word) > 0)
            {
                for (const auto& id : documents_.at(word))
                {
                    if(no_filtered_docs.count(id.first) > 0)
                    no_filtered_docs.erase(id.first);
                }
            }
        }     

        for (const auto& [id, relevance] : no_filtered_docs)
        {

            matched_documents.push_back({ id, relevance });
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
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
            << "relevance = "s << relevance << " }"s << endl;
    }
}
