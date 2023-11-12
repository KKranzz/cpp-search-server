#pragma once
#include "string_processing.h"
#include "read_input_functions.h"

template<typename Iterator>
class Paginator
{
public:

    Paginator(Iterator beg, Iterator end, size_t page_size)
    {
        auto start = beg;
        size_t delim = 0;
        while (beg != end)
        {
            if (delim == page_size)
            {
                data.push_back(IteratorRange<Iterator>(start, beg, page_size));
                delim = 0;
                start = beg;
            }

            advance(beg, 1);
            delim++;
        }
        if (delim <= page_size && delim != 0)
        {
            data.push_back(IteratorRange<Iterator>(start, beg, page_size));
        }
    }
    auto begin(const Iterator& container) {

        return  container.begin();
    }
    auto begin()
        const {
        return data.begin();
    }
    auto end(const Iterator& container) const {

        return  container.end();
    }
    auto end()
        const {
        return data.end();
    }

    vector <IteratorRange<Iterator>> data;
};
