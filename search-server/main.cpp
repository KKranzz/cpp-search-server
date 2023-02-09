

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




class SearchServer {
public:

    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }


    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
     
        double weight = 0;
        for (const string& word : words) 
        {
            weight =  static_cast <double> (1) / static_cast<double> (words.size());
        
            word_to_document_freqs_[word].insert({ document_id, +weight });
        }
        document_count++;

    }




    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query request = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(request);

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
      int document_count = 0;
  
    struct Query
    {
        set <string> minus_w;
        set <string> plus_w;
    };

    map<string, map<int, double>> word_to_document_freqs_;

    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) { 
            if (word[0] == '-' && !IsStopWord(word.substr(1)))
                words.push_back(word);
            else if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const {
        Query request;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-')
                request.minus_w.insert(word.substr(1));
            else
                request.plus_w.insert(word);
        }
        return request;
    }

     double IdfCalculate(const string& plsword, const int& document_count, const size_t& size)
        const  {
       return log( static_cast<double> (document_count) / static_cast<double> (size));
    }


    vector<Document> FindAllDocuments(const Query& query_words) const {
        vector<Document> matched_documents;
        map<int, double> document_to_relevance, buff;
        double idf = 0;
      // log((double)document_count / (double)word_to_document_freqs_.at(plsword).size());
        for (const string& plsword : query_words.plus_w) 
        {

            if (word_to_document_freqs_.find(plsword) != word_to_document_freqs_.end()) 
            {
                idf = IdfCalculate(plsword, document_count, word_to_document_freqs_.at(plsword).size());
                for (const auto& buff : word_to_document_freqs_.at(plsword))
                {
                   
                    document_to_relevance[buff.first] += buff.second * idf;
                }
            }
        }

        for (const string& minword : query_words.minus_w)
        {
            if (word_to_document_freqs_.find(minword) != word_to_document_freqs_.end())
            {
                for (const auto& buff : word_to_document_freqs_.at(minword))
                {
                  
                    document_to_relevance.erase(buff.first);
                }
            }
        }



        for (const auto& document : document_to_relevance) {

            matched_documents.push_back({ document.first, document.second });
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
