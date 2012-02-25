#ifndef _pcrexx_pattern_hpp__
#define _pcrexx_pattern_hpp__

// Copyright (c) 2012, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/*!
 * @file Pattern.hpp
 * @see http://www.pcre.org/pcre.txt
 */

#include <pcre.h>
#include "traits.hpp"
#include <vector>

namespace pcrexx {

    template<typename T>
    class basic_pattern
    {
        // Not copyable.
        basic_pattern ( const basic_pattern& );
        basic_pattern& operator= ( const basic_pattern& );

        /* nested types. */
    public:
        typedef T char_type;
        typedef typename traits<char_type> traits_type;

        typedef typename traits_type::handle handle_type;
        typedef typename traits_type::string string_type;

    private:
        typedef basic_pattern<T> self_type;
        typedef typename traits_type::const_char_ptr const_char_ptr;

        /* class methods. */
    private:
        static const_char_ptr name_table ( handle_type pattern )
        {
            const_char_ptr table = 0;
            const int status = traits_type::query
                (pattern, 0, PCRE_INFO_NAMETABLE, (void*)&table);
            if (status != 0)
            {
                std::cerr
                    << "Failed to fetch group name table!"
                    << std::endl;
            }
            return (table);
        }

        static int name_table_stride ( handle_type pattern )
        {
            int stride = 0;
            const int status = traits_type::query
                (pattern, 0, PCRE_INFO_NAMEENTRYSIZE, &stride);
            if (status != 0)
            {
                std::cerr
                    << "Failed to fetch group name table stride!"
                    << std::endl;
            }
            return (stride);
        }

        /* data. */
    private:
        string_type myText;
        handle_type myHandle;

        /* construction. */
    public:
        /*!
         * @brief Compile a regular expression.
         */
        basic_pattern ( const string_type& text )
            : myText(text), myHandle(0)
        {
            int error = 0;
            int offset = 0;
            int options = 0;
            const char * help = 0;
            myHandle = traits_type::compile
                (myText.c_str(), options, &error, &help, &offset, 0);
            if (myHandle == 0)
            {
                std::cerr
                    << "Failed to compile pattern ("
                    << error << ": '" << ((help==0)?"":help) << "')!"
                    << std::endl;
            }
        }

        ~basic_pattern ()
        {
            traits_type::release(myHandle);
        }

        /* methods. */
    public:
        handle_type handle () const
        {
            return (myHandle);
        }

        /*!
         * @brief Regular expression used to compile the pattern.
         */
        const string_type& text () const
        {
            return (myText);
        }

        /*!
         * @brief Obtain the number of capturing groups in the pattern.
         */
        int capturing_groups () const
        {
            int groups = 0;
            const int status = traits_type::query
                (myHandle, 0, PCRE_INFO_CAPTURECOUNT, &groups);
            if (status != 0)
            {
                std::cerr
                    << "Failed to count groups!"
                    << std::endl;
            }
            return (groups);
        }

        /*!
         * @brief Obtain the index of a group using its name.
         */
        int group_index ( const string_type& name ) const
        {
            const int index =
                traits_type::string_number(myHandle, name.c_str());
            if (index == PCRE_ERROR_NOSUBSTRING)
            {
#if 0
                std::cerr
                    << "No such group: '" << name << "'!"
                    << std::endl;
#endif
            }
            return (index);
        }

        /*!
         * @brief Obtain the names of all capturing groups.
         *
         * The name of capturing groups are listed in alphabetical order.
         */
        std::vector<string_type> group_names () const
        {
            // Access the name table.
            const_char_ptr table = name_table(myHandle);
            const int stride = name_table_stride(myHandle);
            // Scan the table for names.
            std::vector<string_type> names;
            for (int i=0; (i < capturing_groups()); ++i) {
                names.push_back(table+(i*stride)+traits_type::table_offset());
            }
            return (names);
        }
    };

    typedef basic_pattern<char> pattern;
    typedef basic_pattern<wchar_t> wpattern;

}

#endif /* _pcrexx_pattern_hpp__ */