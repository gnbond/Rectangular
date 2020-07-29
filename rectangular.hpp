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
 * Bugs/Comments/Pull requests to https://github.com/gnbond/Rectangular
 */

#include <stdexcept>
#include <utility>
#include <vector>

namespace gnb {

/* 
 * Wrap a 1-D vector<> to represent a 2-D container 
 * 
 * rectangular<char> pa{3,2}; // a 3x2 array, 3 rows 2 columns
 * 
 * As per example from std::vector<> or std::array<>,
 * pa.at(0,1); // Bounds-checked, will throw std::out_of_range if required
 * pa[0][1]; // no bound check, but fast.  Use at your own risk!
 */
template <typename T, class Allocator = std::allocator<T> >
class rectangular {
    private:
        using BaseType = std::vector<T, Allocator>;
    public:
        // Types for STL
        using iterator = typename BaseType::iterator;
        using const_iterator = typename BaseType::const_iterator;
        using value_type = typename BaseType::value_type;
        using reference = typename BaseType::reference;
        using const_reference = typename BaseType::const_reference;
        using pointer = typename BaseType::pointer;
        using const_pointer = typename BaseType::const_pointer;
        using size_type = typename BaseType::size_type;
        using difference_type = typename BaseType::difference_type;

        rectangular() : m_height{0}, m_width{0}, m_data{} {}
        explicit rectangular(size_type height, size_type width, value_type value = value_type()) : 
            m_height{height}, m_width{width},
            m_data(height * width, value) {
            }
        // Create from an iterator pair, throw std::out_of_range if not exactly right amount of data
        template <typename InputIterator>
        explicit rectangular(size_type height, size_type width, InputIterator b, InputIterator e) :
            m_height{height}, m_width{width},
            m_data{} {
                m_data.reserve(height * width);
                m_data.insert(begin(), b, e);
                if (m_data.size() != height * width) throw std::out_of_range("rectangular create");
        }

        // Default dtor/copy/assign/move OK

        // Iterate over the data in row-major order
        iterator begin() { return m_data.begin(); }
        iterator end() { return m_data.end(); }
        const_iterator begin() const { return m_data.cbegin(); }
        const_iterator end() const { return m_data.cend(); }
        const_iterator cbegin() const { return m_data.cbegin(); }
        const_iterator cend() const { return m_data.cend(); }

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
        using Base = rectangular<T, Allocator>;
    public:
        using size_type = typename Base::size_type;
        using value_type = typename Base::value_type;
        /*
         * This proxy class is not constructable or assignable outside the checked_rectangular class
         * It is implemented as a template to avoid cut-n-paste for the const version;
         * For the non-const case, Rect = checked_rectangular and RefType = Rect::reference (aka T&)
         * For the const case, Rect = const checked_rectangular and RefType = Rect::const_reference (aka const T&)
         */
        template <typename Rect>
        class RowProxy {
                friend class checked_rectangular; // so it can call private constructor
                using RefType = decltype(Rect(1,1).at(0,0));
                using size_type = typename Rect::size_type;
            private:
                Rect& m_rect;
                size_type m_y;
                // Private, so can't be created by anyone except owning friend class
                RowProxy(Rect& rect, typename Rect::size_type y) : m_rect{rect}, m_y{y} {}
                // C++11 Needs move constructor to return from a function
                // C++17 doesn't, due to required copy elision
                // MacOS XCode 10 Clang doesn't support __cpp_guaranteed_copy_elision but does do the elision
#if __cpp_guaranteed_copy_elision >= 201606
                RowProxy(RowProxy&&) = delete;
#else
                RowProxy(RowProxy&&) = default;
#endif
                // Rest are deleted to help avoid dangling references
                RowProxy(const RowProxy&) = delete;
                RowProxy& operator=(const RowProxy&) = delete;
                RowProxy& operator=(RowProxy&&) = delete;
            public:
                ~RowProxy() = default;
                RefType operator[](size_type x) { return m_rect.at(m_y, x); }
        };
        
        checked_rectangular() : Base{} {}
        explicit checked_rectangular(size_type height, size_type width, 
                                        value_type value = value_type())
            : Base(height, width, value) {}
        // Create from an iterator pair, throw std::out_of_range if not exactly right amount of data
        template <typename InputIterator>
        explicit checked_rectangular(size_type height, size_type width, InputIterator b, InputIterator e) 
            : Base(height, width, b, e) {}
        // Default dtor/copy/assign/move OK

        RowProxy<Base> operator[](size_type y) { return RowProxy<Base>(*this, y); }
        RowProxy<const Base> operator[](size_type y) const {
            return RowProxy<const Base>(*this, y);
        }
};

} // namespace gnb

#endif // GNB_rectangular
