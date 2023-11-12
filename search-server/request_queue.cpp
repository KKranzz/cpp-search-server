#include "read_input_functions.h"
#include "document.h"

   std::string ReadLine() {
        std::string s;
        std::getline(std::cin, s);
        return s;
    }

    int ReadLineWithNumber() {
        int result;
        std::cin >> result;
        ReadLine();
        return result;
    }

    std::ostream& operator << (std::ostream& ost, const Document& doc)
    {
        return ost << "{ document_id = " << doc.id
            << ", relevance = " << doc.relevance
            << ", rating = " << doc.rating << " }";

        return ost;
    }

    
    
