#ifndef GNB_rectangular
#define GNB_rectangular

/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Please feel free to copy this file into your own project.  
 * This software comes with NO WARRANTY.
 *
 * For more information, please see the associated LICENSE file or refer to <https://unlicense.org>
 * 
 * This version for C++98 and C++03.  Version for C++11 and later available separately.
 *
 * Bugs/Comments/Pull requests to https://github.com/gnbond/Rectangular
 */

#include <stdexcept>
#include <utility>
#include <vector>

namespace gnb {

/* 
 * Wrap a 1-D vector<> to represent a 2-D container 
 * 
 * rectangular<char> pa(3,2); // a 3x2 array, 3 rows 2 columns
 * 
 * As per example from std::vector<> or std::array<>,
 * pa.at(0,1); // Bounds-checked, will throw std::out_of_range if required
 * pa[0][1]; // no bound check, but fast.  Use at your own risk!
 */
template <typename T, class Allocator = std::allocator<T> >
class rectangular {
    private:
        typedef std::vector<T, Allocator> BaseType;
    public:
        // Types for STL
        typedef typename BaseType::iterator iterator;
        typedef typename BaseType::const_iterator const_iterator;
        typedef typename BaseType::value_type value_type;
        typedef typename BaseType::reference reference;
        typedef typename BaseType::const_reference const_reference;
        typedef typename BaseType::pointer pointer;
        typedef typename BaseType::const_pointer const_pointer;
        typedef typename BaseType::size_type size_type;
        typedef typename BaseType::difference_type difference_type;

        rectangular() : m_height(0), m_width(0), m_data() {}
        explicit rectangular(size_type height, size_type width, value_type value = value_type()) : 
            m_height(height), m_width(width),
            m_data(height * width, value) {
            }
        // Create from an iterator pair, throw std::out_of_range if not exactly right amount of data
        template <typename InputIterator>
        explicit rectangular(size_type height, size_type width, InputIterator b, InputIterator e) :
            m_height(height), m_width(width),
            m_data() {
                m_data.reserve(height * width);
                m_data.insert(begin(), b, e);
                if (m_data.size() != height * width) throw std::out_of_range("rectangular create");
        }
        
        // Efficiently move a std:vector<> into a rectangular
        // NB: will erase the argument!
        rectangular(size_type height, size_type width, std::vector<value_type>& vec) :
            m_height(height), m_width(width),
            m_data() {
                if (vec.size() != height * width) throw std::out_of_range("rectangular vector<> create");
                // Check _then_ destroy the source!
                m_data.swap(vec);
        }

        // Default dtor/copy/assign/move OK

        // Iterate over the data in row-major order
        iterator begin() { return m_data.begin(); }
        iterator end() { return m_data.end(); }
        const_iterator begin() const { return m_data.begin(); }
        const_iterator end() const { return m_data.end(); }

        size_type size() const { return m_data.size(); }
        bool empty() const { return m_data.empty(); }

        size_type height() const { return m_height; }
        size_type width() const { return m_width; }
        
        // Bounds-checked, will throw std::out_of_range() if required
        reference at(size_type y, size_type x) {
            if (y >= m_height) throw std::out_of_range("rectangular Y index");
            if (x >= m_width) throw std::out_of_range("rectangular X index");
            return m_data[y * m_width + x];
        }
        
        // Bounds-checked, will throw std::out_of_range() if required
        const_reference at(size_type y, size_type x) const {
            if (y >= m_height) throw std::out_of_range("rectangular Y index");
            if (x >= m_width) throw std::out_of_range("rectangular X index");
            return m_data[y * m_width + x];
        }

        // Raw pointers, fast but no bounds checking
        // Allows client code to use r[y][x] if it is careful
        pointer operator[](size_type y) { return &m_data[y * m_width]; }
        const_pointer operator[](size_type y) const { return &m_data[y * m_width]; }

        // Will retain existing data, erasing elements that are no longer 
        // required, and using value for any new data
        void resize(size_type new_height, size_type new_width, 
            value_type value = value_type()) {
                m_data.resize(new_height * new_width, value);
                m_height = new_height;
                m_width = new_width;
        }

        // Set every element to the given fallue
        void fill(const_reference value) {
            std::fill(begin(), end(), value);
        }

        void swap(rectangular& r) {
            std::swap(m_height, r.m_height);
            std::swap(m_width, r.m_height);
            std::swap(m_data, r.m_data);
        }
    private:
        size_type m_height, m_width;
        BaseType m_data;
};

/*
 * checked_rectangular IS-A rectangular
 * but r[y][x] is now range-checked and may throw std::out_of_range like r.at(y,x)
 * 
 * Implemented using a proxy class returned from operator[].  Do not save/reuse the 
 * proxy object if you wish to avoid dangling references.
 * 
 */
template <typename T, class Allocator = std::allocator<T> >
class checked_rectangular : public rectangular<T, Allocator> {
        typedef rectangular<T, Allocator> Base;
    public:
        typedef typename Base::size_type size_type;
        typedef typename Base::value_type value_type;
        /*
         * This proxy class is not constructable or assignable outside the checked_rectangular class
         * It is implemented as a template to avoid cut-n-paste for the const version;
         * For the non-const case, Rect = checked_rectangular and RefType = Rect::reference (aka T&)
         * For the const case, Rect = const checked_rectangular and RefType = Rect::const_reference (aka const T&)
         */
        template <typename Rect, typename RefType>
        class RowProxy {
                friend class checked_rectangular; // so it can call private constructor
                typedef typename Rect::size_type size_type;
            private:
                Rect& m_rect;
                size_type m_y;
                // Private, so can't be created by anyone except owning friend class
                RowProxy(Rect& rect, typename Rect::size_type y) : m_rect(rect), m_y(y) {}
                RowProxy(const RowProxy& r) : m_rect(r.m_rect), m_y(r.m_y) {}
            public:
                ~RowProxy() {};
                RefType operator[](size_type x) { return m_rect.at(m_y, x); }
        };

        // Inherit all the base class constructors
        // C++03 doesnt allow inheritance of constructors with using-declaration (C++11 feature)

        checked_rectangular() : Base() {}
        explicit checked_rectangular(size_type height, size_type width, value_type value = value_type()) : 
            Base(height, width, value) {}
        template <typename InputIterator>
        explicit checked_rectangular(size_type height, size_type width, InputIterator b, InputIterator e) :
            Base(height, width, b, e) {}
        
        // Efficiently move a std:vector<> into a rectangular
        // NB: will erase the argument!
        checked_rectangular(size_type height, size_type width, std::vector<value_type>& vec) :
            Base(height, width, vec) {}

        // Default dtor/copy/assign/move OK

        RowProxy<Base, typename Base::reference> operator[](size_type y) { return RowProxy<Base, typename Base::reference>(*this, y); }
        RowProxy<const Base, typename Base::const_reference> operator[](size_type y) const {
            return RowProxy<const Base, typename Base::const_reference>(*this, y);
        }
};

} // namespace gnb

#endif // GNB_rectangular
