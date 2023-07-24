#pragma once
#include <ostream>


using namespace std; // fix


template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
    {
        begin_ = begin;
        end_ = end;
    }
    Iterator begin() {
        return begin_;
    }
    Iterator end() {
        return end_;
    }


private:
    Iterator begin_;
    Iterator end_;
};




template <typename Iterator>
ostream& operator<<(ostream& out, IteratorRange<Iterator> iterator) {

    for (auto it = iterator.begin(); it != iterator.end(); it++) {
        out << *it;
    }

    return out;
}


template <typename Iterator>
class Paginator {
public:
    // тело класса
    Paginator(Iterator begin_, Iterator end_, size_t size_)
    {
        size_t count_confirm = 0;
        Iterator temp = begin_;
        for (Iterator it = begin_; it != end_; it++)
        {
            count_confirm++;

            if (count_confirm == size_)
            {
                pages.push_back(IteratorRange<Iterator>{temp, it + 1});
                temp = it + 1;
                count_confirm = 0;
            }
            else if (distance(it, end_) < size_)
            {
                pages.push_back(IteratorRange<Iterator>{it, end_});
            }

        }

    }

    auto begin() const {
        return pages.begin();
    }

    auto end() const {
        return pages.end();
    }

private:
    vector <IteratorRange<Iterator>> pages;

};



template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}
