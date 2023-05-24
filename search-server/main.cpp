#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <utility>
#include <numeric>

//#include "search_server.h" заголовочный файл из задания


using namespace std;


template<typename T>
ostream& operator << (ostream& os, const set<T>& numbers)
{

    if (numbers.empty())
        return os;

    size_t counter = numbers.size();

    os << "{"s;

    for (const auto& number : numbers) {
        if (counter == 1) {
            os << number << "}"s;
            break;
        }
        else
            os << number << ", ";

        counter--;
    }

    return os;
}

template<typename T, typename U>
ostream& operator << (ostream& os, const map<T, U>& numbers)
{
    if (numbers.empty())
        return os;

    size_t counter = numbers.size();

    os << "{"s;

    for (const auto& number : numbers) {
        if (counter == 1) {
            os << number.first + ": " << number.second << "}"s;
            break;
        }
        else
            os << number.first + ": " << number.second << ", ";

        counter--;
    }

    return os;
}


template<typename T>
ostream& operator << (ostream& os, const vector<T>& numbers)
{

    if (numbers.empty())
        return os;

    size_t counter = numbers.size();

    os << "["s;

    for (const auto& number : numbers) {
        if (counter == 1) {
            os << number << "]"s;
            break;
        }
        else
            os << number << ", ";

        counter--;
    }

    return os;
}


const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double EPSILON = 1e-6;

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
    int rating;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document, DocumentStatus status,
        const vector<int>& ratings) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
    }



    template <typename Func>
    vector<Document> FindTopDocuments(const string& raw_query,
        Func function)
        const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query, function);

        sort(matched_documents.begin(), matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                if (abs(lhs.relevance - rhs.relevance) < EPSILON) { //fix1
                    return lhs.rating > rhs.rating;
                }
                else {
                    return lhs.relevance > rhs.relevance;
                }
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }


    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status = DocumentStatus::ACTUAL)
        const {
        DocumentStatus state = status;
        const auto function = [state](int document_id, DocumentStatus status, int rating) { return  state == status; };
        return FindTopDocuments(raw_query, function);
    }



    int GetDocumentCount() const {
        return documents_.size();
    }

    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query,
        int document_id) const {
        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
        return { matched_words, documents_.at(document_id).status };
    }

private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;

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

    static int ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }

        int rating_sum = accumulate(ratings.begin(), ratings.end(), 0); // fix2


        return rating_sum / static_cast<int>(ratings.size());
    }

    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(string text) const {
        bool is_minus = false;
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return { text, is_minus, IsStopWord(text) };
    }

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    Query ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                }
                else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }


    double ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }


    template<typename Func>
    vector<Document> FindAllDocuments(const Query& query, Func function)
        const {

        map<int, double> document_to_relevance;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& buffdoc = documents_.at(document_id); // fix
                if (function(document_id, buffdoc.status, buffdoc.rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back(
                { document_id, relevance, documents_.at(document_id).rating });
        }
        return matched_documents;
    }
};



void PrintDocument(const Document& document) {
    cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s << endl;
}


void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line,
    const string& hint) {
    if (!value) {
        cout << file << "("s << line << "): "s << func << ": "s;
        cout << "ASSERT("s << expr_str << ") failed."s;
        if (!hint.empty()) {
            cout << " Hint: "s << hint;
        }
        cout << endl;
        abort();
    }
}

template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const string& t_str, const string& u_str, const string& file,
    const string& func, unsigned line, const string& hint) {

    if (t != u) {
        cout << boolalpha;
        cout << file << "("s << line << "): "s << func << ": "s;
        cout << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cout << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cout << " Hint: "s << hint;
        }
        cout << endl;
        abort();
    }
}

template <typename Func>
void RunTestImpl(Func func, const string& funcname) {
    /* Напишите недостающий код */
    func();
    cerr << funcname + " OK\n"s;

}


/*
 Реализация макросов
*/
#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))

#define RUN_TEST(func)  RunTestImpl(func, #func)


// -------- Начало модульных тестов поисковой системы ----------

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(),
            "Stop words must be excluded from documents"s);
    }
}


void TestAddDocument()
{
    const int doc_id = 43;
    const string content = "dog in the tower"s;
    const vector<int> ratings = { 1, 2, 3 };

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        vector <Document> docs = server.FindTopDocuments("dog"s);
        ASSERT_EQUAL(docs.size(), 1);
        server.AddDocument(doc_id + 1, content + "cat"s, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 5, content + "cats"s, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 4, content + "mise"s, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 2, content + "coat"s, DocumentStatus::ACTUAL, ratings);
        docs = server.FindTopDocuments("dog"s);
        ASSERT_EQUAL(docs.size(), 5);
    }


}


void TestWorkDocumentStatus() 
{
    const int doc_id = 43;
    const string content = "dog in the tower"s;
    const vector<int> ratings = { 1, 2, 3 };

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content + "cat"s, DocumentStatus::IRRELEVANT, ratings);
        server.AddDocument(doc_id + 5, content + "cats"s, DocumentStatus::IRRELEVANT, ratings);
        server.AddDocument(doc_id + 4, content + "mise"s, DocumentStatus::BANNED, ratings);
        server.AddDocument(doc_id + 2, content + "coat"s, DocumentStatus::REMOVED, ratings);
        vector <Document> docs = server.FindTopDocuments("dog"s);
        ASSERT_EQUAL(docs.size(), 1);
        docs = server.FindTopDocuments("in"s, DocumentStatus::IRRELEVANT);
        ASSERT_EQUAL(docs.size(), 2);
        docs = server.FindTopDocuments("in"s, DocumentStatus::REMOVED);
        ASSERT_EQUAL(docs.size(), 1);
        docs = server.FindTopDocuments("in"s, DocumentStatus::BANNED);
        ASSERT_EQUAL(docs.size(), 1);
        int count = server.GetDocumentCount();
        ASSERT_EQUAL(count, 5);
    }


}


void TestMinusWordWork()
{

    const int doc_id = 43;
    string content = "dog in the tower"s;
    const vector<int> ratings = { 1, 2, 3 };

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        content = "cat in the tower"s;
        server.AddDocument(doc_id+1, content, DocumentStatus::ACTUAL, ratings);
        vector <Document> docs = server.FindTopDocuments("in the"s);
        ASSERT_EQUAL(docs.size(), 2);
        ASSERT(docs[0].id == doc_id);
        ASSERT(docs[1].id == doc_id + 1);
        docs = server.FindTopDocuments("-dog in the"s);
        ASSERT_EQUAL(docs.size(), 1);
        
        ASSERT(docs[0].id == doc_id+1);
        

    }


}


void TestMatching()
{
    const int doc_id = 43;
    const string content = "dog in the tower"s;
    const vector<int> ratings = { 1, 2, 3 };

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        tuple<vector<string>, DocumentStatus> res = server.MatchDocument("dog in at me"s, doc_id);
        const auto& [documentidentity, status] = res;
        vector <string> buff = documentidentity;
        ASSERT_EQUAL(buff.size(), 2);
        res = server.MatchDocument("-dog in at me"s, doc_id);
        buff = documentidentity;
        ASSERT(buff.empty());
    }



}


void TestCalcSequenceRelevance()
{
    const int doc_id = 43;
    const string content1 = "dog in the tower"s;
    const string content2 = "dog in at the tower"s;
    const string content3 = "dog in at big the tower"s;
    const vector<int> ratings = { 1, 2, 3 };



    {
        
        double relevance = DBL_MAX;
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 2, content3, DocumentStatus::ACTUAL, ratings);
        vector <Document> res = server.FindTopDocuments("dog in at big"s);
      
        
        for (const auto& doc : res) {
            ASSERT(doc.relevance < relevance);
            relevance = doc.relevance;
        }
        

    }



}

void TestRating()
{
    const int doc_id = 43;
    const string content1 = "dog in the tower"s;
    const vector<int> ratings = { 1, 2, 3 };

    int mid = accumulate(ratings.begin(), ratings.end(), 0);
    mid /= ratings.size();

    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        const auto res = server.FindTopDocuments("dog"s);
        ASSERT_EQUAL(res[0].rating, mid);
    }


}

void TestFuncPredicat()
{
    const int doc_id = 43;
    const string content1 = "dog in the tower"s;
    const vector<int> ratings1 = { 1, 1, 1 };
    const vector<int> ratings2 = { 1, 1, 2 };
    DocumentStatus state = DocumentStatus::IRRELEVANT;
    const auto function = [state](int document_id, DocumentStatus status, int rating)
    {
        return status == state && rating == 1;
    };
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings2);
        vector<Document> res = server.FindTopDocuments("dog"s, function);
        ASSERT(res.empty());
        server.AddDocument(doc_id + 1, content1, DocumentStatus::IRRELEVANT, ratings1);
        res = server.FindTopDocuments("dog"s, function);
        ASSERT_EQUAL(res.size(), 1);
    }


}

void TestCalculateTFIDF() 
{
    const int doc_id = 43;
    const string content1 = "dog in the tower"s;
    const string content2 = "dog in at the tower"s;
    const string content3 = "dog in at big the tower"s;
    const vector<int> ratings = { 1, 2, 3 };
    
    {
       
        double relevance = DBL_MAX;
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 2, content3, DocumentStatus::ACTUAL, ratings);
        vector <Document> res = server.FindTopDocuments("dog in at big"s);
        
        double rel1 = log(server.GetDocumentCount() * 1.0 / 3) * (1.0 / 4) +
            log(server.GetDocumentCount() * 1.0 / 3) * (1.0 / 4);
        double rel2 = log(server.GetDocumentCount() * 1.0 / 3) * (1.0 / 5) +
            log(server.GetDocumentCount() * 1.0 / 3) * (1.0 / 5)+
            log(server.GetDocumentCount() * 1.0 / 2) * (1.0 / 5);

        double rel3 = log(server.GetDocumentCount() * 1.0 / 3) * (1.0 / 6) +
            log(server.GetDocumentCount() * 1.0 / 3) * (1.0 / 6) +
            log(server.GetDocumentCount() * 1.0 / 2) * (1.0 / 6) +
            log(server.GetDocumentCount() * 1.0 / 1) * (1.0 / 6);

      ASSERT(res[0].relevance == rel3);
      ASSERT(res[1].relevance == rel2);
      ASSERT(res[2].relevance == rel1);

    
    }

}


// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestAddDocument);
    RUN_TEST(TestMinusWordWork);
    RUN_TEST(TestMatching);
    RUN_TEST(TestCalcSequenceRelevance);
    RUN_TEST(TestRating);
    RUN_TEST(TestFuncPredicat);
    RUN_TEST(TestWorkDocumentStatus);
    RUN_TEST(TestCalculateTFIDF);
}

// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    // Если вы видите эту строку, значит все тесты прошли успешно
    cout << "Search server testing finished"s << endl;
}
