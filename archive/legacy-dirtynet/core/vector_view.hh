#include <vector>
#include <stdexcept>

template<typename Data>
class VectorView
{
public:

    struct Iterator 
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Data;
        using pointer           = Data*;
        using reference         = Data&;

        Iterator(pointer ptr) : m_ptr(ptr) {}

        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }
        Iterator& operator++() { m_ptr++; return *this; }  
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
        friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };  

    private:
        pointer m_ptr;
    };


    VectorView(std::vector<Data> & vec, size_t begin, size_t length)
    {
        // error out if we are too big
        if(begin + length > vec.size())
        {
            return;
        }
        // Data* tmp = vec.data();
        _internal = &vec[begin];
        _length = length;
        // _max = vec.size();

        _begin = Iterator(&_internal[0]);
        _end = Iterator(&_internal[_length]);
    }

    Iterator begin() { return _begin; }
    Iterator end() { return _end;}

    bool set(Data const& value, size_t index)
    {
        if(index >= _length)
        {
            return false;
        }
        _internal[index] = value;
        return true;
    }

    Data* get(size_t index)
    {
        if(index >= _length)
        {
            return nullptr;
        }
        return &_internal[index];
    }
    Data& at(size_t index)
    {
        if(index >= _length)
        {
            throw std::out_of_range("bad index: " + std::to_string(index) + " vs length: " + std::to_string(_length));
        }
        return _internal[index];
    }

    Data const& at(size_t index) const
    {
        if(index >= _length)
        {
            throw std::out_of_range("bad index: " + std::to_string(index) + " vs length: " + std::to_string(_length));
        }
        return _internal[index];
    }


private:
    size_t _length{0};
    // size_t _max{0};
    Data* _internal{nullptr};
    Iterator _begin{nullptr};
    Iterator _end{nullptr};

};