#pragma once
#include <string>
#include <iostream>
#include "document.h"


template<typename Iterator>
class IteratorRange
{
public:

    IteratorRange() = default;
    IteratorRange(Iterator beg, Iterator end, size_t size)
    {
        beg_ = beg;
        end_ = end;
        page_size_ = size;
    }
    auto begin()
        const {
        return beg_;
    }

    auto end()
        const {
        return end_;
    }

    auto size()
        const {
        return;
    }
private:
    Iterator beg_, end_;
    size_t page_size_;

};


std::string ReadLine();   
int ReadLineWithNumber();




std::ostream& operator << (std::ostream& ost, const Document& doc);




template <typename Iterator>
std::ostream& operator <<(std::ostream& os, const IteratorRange<Iterator>& page) {
    Iterator It = page.begin();
    for (; It != page.end(); It++) {
        os << *It;
    }

    return os;
}


