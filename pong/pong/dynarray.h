#pragma once
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

typedef size_t arrln;
typedef signed int sarrln;
#define SZ_DEFAULT 1000
#define STC_SZ_MAX 5.f
#define STC_SZ_MIN 1.5f
#define STC_N_STAT 25
#define msgassert(expression, message) { if (!expression) printf ("%s\n", message); \
                                       (void)((!!(expression)) || \
                                       (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0)); }

template <typename dataType> class darray
    {
    private:
        float stretch_k;
        arrln currentLen, allocLen;
        dataType* container;
        unsigned long long int nPushes, nOperations;

        // Allocates memory and fils it with poison value
        bool allocate (dataType* & newContainer, arrln len);

        void updateStretchK ();


    public:
        // Constructors and destructor
        darray (arrln Size = SZ_DEFAULT);
        ~darray ();

        // Container getters
        dataType & back ();
        dataType & front ();

        // Operators
        dataType & operator [] (sarrln index);

        // Modifiers
        bool push_back (dataType value);
        bool pop_back ();
        bool push_front (dataType value);
        bool pop_front ();
        bool insert (dataType value, sarrln index);
        bool erase (sarrln index);

        // Size getters
        arrln size ();
        bool empty ();
        arrln capacity ();

        // Size setters
        bool shrink ();
        bool resize (arrln newSize);
        void clear ();

    };


// Util
template<typename dataType>
inline bool darray<dataType>::allocate (dataType *& newContainer, arrln len)
    {
    newContainer = nullptr;

    msgassert (allocLen, "Size of array must be greater than zero\n");
    newContainer = (dataType*)calloc (len, sizeof (dataType));

    if (newContainer == nullptr)
        {
        printf ("Failed to allocate memory\n");
        return false;
        }
    else
        {
        //printf ("Allocated %d bytes\n", len * sizeof (dataType));

        //for (int i = 0; i < len; i++)
            //newContainer [i] = NAN;

        return true;
        }
    }

template<typename dataType>
inline void darray<dataType>::updateStretchK ()
    {
    nOperations++;

    if (nOperations > STC_N_STAT)
        {
        if (nOperations > 0)
            nOperations--;
        if (nPushes > 0)
            nPushes--;
        }

    stretch_k = STC_SZ_MIN +
        float ((STC_SZ_MAX - STC_SZ_MIN) * (float (nPushes) / float (nOperations)));
    }

// Constructors and destructors
template<typename dataType>
inline darray<dataType>::darray (arrln Size)
    {
    // Default values
    currentLen = 0;
    allocLen = Size;
    nPushes = nOperations = 0;
    stretch_k = STC_SZ_MAX;

    // Allocates memory
    allocate (container, allocLen);
    }

template<typename dataType>
inline darray<dataType>::~darray ()
    {
    free (container);
    }


// Container getters
template<typename dataType>
dataType & darray <dataType>::back ()
    {
    msgassert (currentLen, "Unable to back ():\n\tArray is empty\n");
    return container [currentLen - 1];
    }

template<typename dataType>
dataType & darray <dataType>::front ()
    {
    msgassert (currentLen, "Unable to back ():\n\tArray is empty\n");
    return container [0];
    }


// Operators
template<typename dataType>
inline dataType & darray<dataType>::operator[](sarrln index)
    {
    if (index < 0)
        index = currentLen + index;

    return container [index];
    }


// Modifiers
template<typename dataType>
inline bool darray<dataType>::push_back (dataType value)
    {
    nPushes++;
    updateStretchK ();

    if (currentLen == allocLen - 1)
        if (!resize ((allocLen * stretch_k) + 1))
            return false;

    container [currentLen] = value;
    currentLen++;

    return true;
    }

template<typename dataType>
bool darray<dataType>::pop_back ()
    {
    updateStretchK ();

    if (!currentLen)
        {
        printf ("Unable to pop ():\n\tArray is empty\n");
        return false;
        }

    container [currentLen - 1] = NAN;
    currentLen--;

    return true;
    }

template<typename dataType>
bool darray<dataType>::push_front (dataType value)
    {
    nPushes++;
    updateStretchK ();

    if (currentLen == allocLen - 1)
        if (!resize ((allocLen * stretch_k) + 1))
            return false;

    for (int i = currentLen + 1; i > 0; i--)
        container [i] = container [i - 1];

    container [0] = value;
    currentLen++;

    return true;
    }

template<typename dataType>
bool darray<dataType>::pop_front ()
    {
    updateStretchK ();

    for (int i = 0; i < currentLen - 1; i++)
        container [i] = container [i + 1];
    container [currentLen - 1] = NAN;

    currentLen--;

    return true;
    }

template<typename dataType>
inline bool darray<dataType>::insert (dataType value, sarrln index)
    {
    if (index < 0)
        index = currentLen + index;
    if (index < 0 || index > currentLen)
        {
        printf ("Index is out of range\n");
        return false;
        }

    nPushes++;
    updateStretchK ();

    if (currentLen == allocLen - 1)
        if (!resize ((allocLen * stretch_k) + 1))
            return false;

    for (int i = currentLen + 1; i > index; i--)
        container [i] = container [i - 1];

    container [index] = value;
    currentLen++;

    return true;
    }

template<typename dataType>
inline bool darray<dataType>::erase (sarrln index)
    {
    if (index < 0)
        index = currentLen + index;
    if (index < 0 || index > currentLen)
        {
        printf ("Index is out of range\n");
        return false;
        }

    updateStretchK ();

    for (int i = index; i < currentLen; i++)
        container [i] = container [i + 1];

    container [currentLen - 1] = NAN;
    currentLen--;

    return true;
    }


// Size getters
template<typename dataType>
arrln darray<dataType>::size ()
    {
    return currentLen;
    }

template<typename dataType>
bool darray<dataType>::empty ()
    {
    return (currentLen == 0);
    }

template<typename dataType>
inline arrln darray<dataType>::capacity ()
    {
    return allocLen;
    }


// Size setters
template<typename dataType>
bool darray<dataType>::shrink ()
    {
    return resize (currentLen);
    }

template<typename dataType>
bool darray<dataType>::resize (arrln newSize)
    {
    if (newSize >= currentLen)
        {
        dataType* newContainer = nullptr;

        if (allocate (newContainer, newSize))
            {
            // Copies the memory
            for (int i = 0; i < currentLen; i++)
                newContainer [i] = container [i];

            allocLen = newSize;

            // Releases memory
            free (container);
            container = newContainer;

            return true;
            }
        else
            return false;
        }
    else
        printf ("Failed to 'resize ()'\n\tNew length is less than old one\n");

    return false;
    }

template<typename dataType>
inline void darray<dataType>::clear ()
    {
    currentLen = 0;
    }


#undef SZ_DEFAULT
#undef STC_SZ_MAX
#undef STC_SZ_MIN
#undef STC_N_STAT