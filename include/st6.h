#pragma once

#include <cstddef>
#include <stdexcept>
#include <iterator>

#ifndef _POINTER_X
    #define _POINTER_X(T, A) T*
#endif
#ifndef _REFERENCE_X
    #define _REFERENCE_X(T, A) T&
#endif

namespace st6
{
    template<class _Ty>
    class allocator
    {
    public:
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef _Ty* pointer;
        typedef const _Ty* const_pointer;
        typedef _Ty& reference;
        typedef const _Ty& const_reference;
        typedef _Ty value_type;
        pointer address(reference _X) const { return (&_X); }
        const_pointer address(const_reference _X) const { return (&_X); }

        void construct(pointer _P, const _Ty& _V) { _Construct(_P, _V); }
        void destroy(pointer _P) { _Destroy(_P); }

        size_t max_size() const
        {
            size_t _N = (size_t)(-1) / sizeof(_Ty);
            return (0 < _N ? _N : 1);
        }
    };

    template<class _Ty, class _A = allocator<_Ty>>
    class vector
    {
    public:
        typedef vector<_Ty, _A> _Myt;
        typedef _A allocator_type;
        typedef typename _A::size_type size_type;
        typedef typename _A::difference_type difference_type;
        typedef typename _A::pointer _Tptr;
        typedef typename _A::const_pointer _Ctptr;
        typedef typename _A::reference reference;
        typedef typename _A::const_reference const_reference;
        typedef typename _A::value_type value_type;
        typedef _Tptr iterator;
        typedef _Ctptr const_iterator;

        iterator begin() { return (_First); }
        const_iterator begin() const { return ((const_iterator)_First); }
        iterator end() { return (_Last); }
        const_iterator end() const { return ((const_iterator)_Last); }

        size_type size() const { return (_First == 0 ? 0 : _Last - _First); }
        bool empty() const { return (size() == 0); }

        const_reference operator[](size_type _P) const { return (*(begin() + _P)); }
        reference operator[](size_type _P) { return (*(begin() + _P)); }

    protected:
        _A allocator;
        iterator _First, _Last, _End;
    };

    template <class _Ty, class _A = allocator<_Ty>>
    class list
    {
        protected:
            struct _Node;
            friend struct _Node;
            typedef _POINTER_X(_Node, _A) _Nodeptr;
            struct _Node
            {
                    _Nodeptr _Next, _Prev;
                    _Ty _Value;
            };
            struct _Acc;
            friend struct _Acc;
            struct _Acc
            {
                    typedef _REFERENCE_X(_Nodeptr, _A) _Nodepref;
                    typedef typename _A::reference _Vref;
                    static _Nodepref _Next(_Nodeptr _P) { return ((_Nodepref)(*_P)._Next); }
                    static _Nodepref _Prev(_Nodeptr _P) { return ((_Nodepref)(*_P)._Prev); }
                    static _Vref _Value(_Nodeptr _P) { return ((_Vref)(*_P)._Value); }
            };

        public:
            typedef list<_Ty, _A> _Myt;
            typedef _A allocator_type;
            typedef typename _A::size_type size_type;
            typedef typename _A::difference_type difference_type;
            typedef typename _A::pointer _Tptr;
            typedef typename _A::const_pointer _Ctptr;
            typedef typename _A::reference reference;
            typedef typename _A::const_reference const_reference;
            typedef typename _A::value_type value_type;
            // CLASS const_iterator
            class iterator;
            class const_iterator;
            friend class const_iterator;
            class const_iterator
            {
                public:
                    const_iterator() {}
                    const_iterator(_Nodeptr _P) : _Ptr(_P) {}
                    const_iterator(const iterator& _X) : _Ptr(_X._Ptr) {}
                    const_reference operator*() const { return (_Acc::_Value(_Ptr)); }
                    _Ctptr operator->() const { return (&**this); }
                    const_iterator& operator++()
                    {
                        _Ptr = _Acc::_Next(_Ptr);
                        return (*this);
                    }
                    const_iterator operator++(int)
                    {
                        const_iterator _Tmp = *this;
                        ++*this;
                        return (_Tmp);
                    }
                    const_iterator& operator--()
                    {
                        _Ptr = _Acc::_Prev(_Ptr);
                        return (*this);
                    }
                    const_iterator operator--(int)
                    {
                        const_iterator _Tmp = *this;
                        --*this;
                        return (_Tmp);
                    }
                    bool operator==(const const_iterator& _X) const { return (_Ptr == _X._Ptr); }
                    bool operator!=(const const_iterator& _X) const { return (!(*this == _X)); }
                    _Nodeptr _Mynode() const { return (_Ptr); }

                protected:
                    _Nodeptr _Ptr;
            };
            // CLASS iterator
            friend class iterator;
            class iterator : public const_iterator
            {
                public:
                    iterator() {}
                    iterator(_Nodeptr _P) : const_iterator(_P) {}
                    reference operator*() const { return (_Acc::_Value(this->_Ptr)); }
                    _Tptr operator->() const { return (&**this); }
                    iterator& operator++()
                    {
                        this->_Ptr = _Acc::_Next(this->_Ptr);
                        return (*this);
                    }
                    iterator operator++(int)
                    {
                        iterator _Tmp = *this;
                        ++*this;
                        return (_Tmp);
                    }
                    iterator& operator--()
                    {
                        this->_Ptr = _Acc::_Prev(this->_Ptr);
                        return (*this);
                    }
                    iterator operator--(int)
                    {
                        iterator _Tmp = *this;
                        --*this;
                        return (_Tmp);
                    }
                    bool operator==(const iterator& _X) const { return (this->_Ptr == _X._Ptr); }
                    bool operator!=(const iterator& _X) const { return (!(*this == _X)); }
            };

            iterator begin() { return (iterator(_Acc::_Next(_Head))); }
            const_iterator begin() const { return (const_iterator(_Acc::_Next(_Head))); }
            iterator end() { return (iterator(_Head)); }
            const_iterator end() const { return (const_iterator(_Head)); }

            _A allocator;
            _Nodeptr _Head;
            size_type _Size;
    };
}
