/* Hector -- A Simple Climate Model
   Copyright (C) 2014-2015  Battelle Memorial Institute

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#ifndef H_EXCEPTION_H
#define H_EXCEPTION_H
/*
 *  exceptions.h - class and macro for program exceptions
 *  hector
 *
 *  Created by Ben on 9/16/10.
 *
 */

#include <string>
#include <iostream>

//-----------------------------------------------------------------------
/*! \brief Exception class.
 *
 *  Sends string (message), line, function and file up the call chain.
 */
class h_exception /* : public std::exception */ {
public:
	std::string extractFilename( const std::string& path )
	{
		unsigned long pos = path.find_last_of( '/' );						// *nix
		if( pos == path.npos ) pos = path.find_last_of( '\\' );	// win
		return path.substr( ( pos == path.npos ) ? 0 : pos+1 );
        // degrades gracefully if no separators found
	}
	std::string msg, func, file, fullfile;
	int linenum;
};


//-----------------------------------------------------------------------
/*! \brief Insertion operator for h_exception objects
 *
 */
inline std::ostream & operator<<( std::ostream &os, const h_exception &he )
{
    os << "msg:  \t" << he.msg << "\nfunc: \t" << he.func
       << "\nfile: \t" << he.file << "\nffile:\t" << he.fullfile << "\n"
       << "\nline: \t" << he.linenum << "\n";
    return os;
}

//-----------------------------------------------------------------------
/*!
 * \brief Some systems define the function name macro differently, so double
 *		  check and redefine as necessary.
 */
#ifndef __func__
#define __func__ __FUNCTION__
#endif

//-----------------------------------------------------------------------
/*!
 * \brief H_ASSERT is the Hector assertion, taking a condition and message
 */

// Note that originally this macro used variable number of arguments (so you
// could say 'H_ASSERT(x)' or 'H_ASSERT(x,"blah")' but Visual Studio doesn't
// seem to support this, so now all assertions require a message.
#define H_ASSERT(x,s) if( !(x) ) H_THROW( "Assertion failed: "+std::string( s ) );
/*
#define H_ASSERT1(x) if( !(x) ) H_THROW( "Assertion failed" );
#define GET_3RD_ARG(arg1, arg2, arg3, ...) arg3
#define H_ASSERT_MACRO_CHOOSER(...) \
GET_3RD_ARG(__VA_ARGS__, H_ASSERT2, H_ASSERT1, )
#define H_ASSERT(...) H_ASSERT_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
*/

/*! \brief Macro for easy exceptions.
 *
 *  Creates an h_exception object, fills in message, and throws it.
 */
#define H_THROW(s) { \
h_exception e; \
e.msg = s; \
e.func = __func__ ; \
e.file = e.extractFilename( __FILE__ ); \
e.fullfile = __FILE__ ; \
e.linenum = __LINE__; \
throw e; }

/*! \brief Macro to re-throw an exception.
 *
 *  The exception location information will be replaced and the message will be
 *  appended from the original exception.
 */
#define H_RETHROW(oe, s) { \
h_exception e; \
e.msg = s + " - " +oe.msg; \
e.func = __func__ ; \
e.file = e.extractFilename( __FILE__ ); \
e.fullfile = __FILE__ ; \
e.linenum = __LINE__; \
throw e; }

#endif

