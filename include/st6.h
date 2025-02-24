#include <cstddef>
#include <stdexcept>
#include <iterator>

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
        pointer allocate(size_type _N, const void*) { return (_Allocate((difference_type)_N, (pointer)0)); }

        char* _Charalloc(size_type _N) { return (_Allocate((difference_type)_N, (char*)0)); }

        void deallocate(void* _P, size_type) { st6_free(_P); }
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

        size_type size() const { return (_First == 0 ? 0 : _Last - _First); }

        const_reference operator[](size_type _P) const { return (*(begin() + _P)); }
        reference operator[](size_type _P) { return (*(begin() + _P)); }

    protected:
        _A allocator;
        iterator _First, _Last, _End;
    };
}
