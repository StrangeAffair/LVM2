#ifndef _Array_
#define _Array_ 1

template<class T>
struct Array
{
    T* start;
    T* finish;
public:
    size_t capacity() const
    {return finish - start;}
    size_t size    () const
    {return finish - start;}
};

template<class T>
auto ArrayCtor(Array<T>& a) -> size_t
{
    a.start  = nullptr;
    a.finish = nullptr;
    return 0;
}

template<class T>
auto ArrayCtor(Array<T>& a, size_t n) -> size_t
{
    if (n == 0)
        return ArrayCtor(a);

    T*  pointer = (T*) calloc(n, sizeof(T));
    if (pointer == nullptr)
    {
        ArrayCtor(a);
        return 1;
    }

    a.start  = pointer;
    a.finish = pointer + n;

    if (!std::is_trivially_default_constructible<T>::value)
        for(size_t i = 0; i < n; ++i)
            new (a.start + i) T();
    return 0;
}

template<class T>
auto ArrayDtor(Array<T>& a) -> size_t
{
    free(a.start);
    a.start  = nullptr;
    a.finish = nullptr;
    return 0;
}

template<class T>
auto ArrayFromIL(Array<T>& a, std::initializer_list<T> il) -> size_t
{
    if (il.size() == 0)
    {
        ArrayCtor(a);
        return 0;
    }

    if (ArrayCtor(a, il.size()) != 0)
        return 1;

    if (std::is_trivially_assignable<T, T>::value)
    {
        memcpy(a.start, il.begin(), il.size() * sizeof(T));
        return 0;
    }
    else
    {
              T* first  = a .start;
        const T* second = il.begin();
        while(second != il.end())
        {
            *first = *second;
            ++first;
            ++second;
        }
        return 0;
    }
}

template<class T>
auto ArrayCopy(Array<T>& dest, const Array<T>& src) -> size_t
{
    if (ArrayCtor(dest, src.size()) != 0)
        return 1;

    if (std::is_trivially_assignable<T, T>::value)
    {
        memcpy(dest.start, src.start, src.size() * sizeof(T));
        return 0;
    }
    else
    {
              T* first  = dest.start;
        const T* second = src .start;
        while(second != src.finish)
        {
            *first = *second;
            ++first;
            ++second;
        }
        return 0;
    }
}


#endif // _Array_

#ifndef _Vector_
#define _Vector_ 1

template<class T>
struct Vector
{
    T* start;
    T* finish;
    T* current;
public:
    size_t capacity() const
    {return finish  - start;}
    size_t size    () const
    {return current - start;}
};

template<class T>
auto VectorCtor(Vector<T>& v) -> size_t
{
    v.start   = nullptr;
    v.finish  = nullptr;
    v.current = nullptr;
    return 0;
}

template<class T>
auto VectorCtor(Vector<T>& v, size_t n) -> size_t
{
    if (n == 0)
        return VectorCtor(v);

    T*  pointer = (T*) calloc(n, sizeof(T));
    if (pointer == nullptr)
    {
        VectorCtor(v);
        return 1;
    }

    v.start   = pointer;
    v.finish  = pointer + n;
    v.current = pointer;
    return 0;
}

template<class T>
auto VectorDtor(Vector<T>& v) -> size_t
{
    if (!std::is_trivially_destructible<T>::value)
        if (v.current != v.start)
            for(T* it = v.start; it != v.current; ++it)
                delete it;

    free(v.start);
    v.start   = nullptr;
    v.finish  = nullptr;
    v.current = nullptr;
    return 0;
}

template<class T>
auto VectorReserve(Vector<T>& v, size_t n) -> size_t
{
    if (n == v.capacity())
        return 0;
    if (n == 0)
        return VectorDtor(v) << 4;

    //bad reserve
    if (n < v.size())
        return 1;
    //to do:
    if (n < v.capacity())
        return 0;

    if (n > v.capacity)
    {
        if (std::is_trivially_move_assignable<T>::value)
        {
            T*  pointer = realloc(v.start, n * sizeof(T));
            if (pointer == nullptr)
                return 1;
            if (pointer == v.start)
                return 0;
            v.current = pointer + v.size();
            v.finish  = pointer + n;
            v.start   = pointer;
        }
        return 3;
    }
}

#endif // _Vector_
