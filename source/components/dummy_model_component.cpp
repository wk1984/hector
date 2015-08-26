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
/*
 *  dummy_model_component.cpp
 *  hector
 *
 *  Created by Pralit Patel on 10/29/10.
 *
 */

#include <boost/lexical_cast.hpp>

#include "components/dummy_model_component.hpp"
#include "core/core.hpp"
#include "h_util.hpp"
#include "visitors/avisitor.hpp"

namespace Hector {
  
using namespace std;

//------------------------------------------------------------------------------
/*! \brief Constructor
 */
DummyModelComponent::DummyModelComponent():
slope( -1 ),
prevX( -1 ),
y( -1 ),
c()
{
    c.allowInterp( true );
}

//------------------------------------------------------------------------------
/*! \brief Destructor
 */
DummyModelComponent::~DummyModelComponent() {
}

//------------------------------------------------------------------------------
// documentation is inherited
string DummyModelComponent::getComponentName() const {
    const string name = DUMMY_COMPONENT_NAME;
    
    return name;
}

//------------------------------------------------------------------------------
/*! \brief Gets the current y.
 *  \return The current y that has been calculated.
 */
double DummyModelComponent::getY() const {
    return y;
}

//------------------------------------------------------------------------------
/*! \brief Gets a reference to c.  Used for testing
 *  \return c.
 */
const tseries<double>& DummyModelComponent::getC() const {
    return c;
}

//------------------------------------------------------------------------------
// documentation is inherited
void DummyModelComponent::init( Core* core ) {
    logger.open( getComponentName(), false, Logger::DEBUG );
    H_LOG( logger, Logger::DEBUG ) << "hello " << getComponentName() << std::endl;
}

//------------------------------------------------------------------------------
// documentation is inherited
unitval DummyModelComponent::sendMessage( const std::string& message,
                                         const std::string& datum,
                                         const message_data info ) throw ( h_exception )
{
    unitval returnval;
    
    if( message==M_GETDATA ) {          //! Caller is requesting data
        return getData( datum, info.date );
        
    } else if( message==M_SETDATA ) {   //! Caller is requesting to set data
        //TODO: call setData below
        //TODO: change core so that parsing is routed through sendMessage
        //TODO: make setData private
        
    } else {                        //! We don't handle any other messages
        H_THROW( "Caller sent unknown message: "+message );
    }
    
    return returnval;
}

//------------------------------------------------------------------------------
// documentation is inherited
void DummyModelComponent::setData( const string& varName,
                                   const message_data& data ) throw ( h_exception )
{
    using namespace boost;
    
    try {
        if( varName == H_STRINGIFY_VAR( slope ) ) {
            H_ASSERT( data.date == Core::undefinedIndex(), "date not allowed" );
            slope = lexical_cast<double>( data.value_str );
        } else if( varName == H_STRINGIFY_VAR( y ) ) {
            H_ASSERT( data.date == Core::undefinedIndex(), "date not allowed" );
            y = lexical_cast<double>( data.value_str );
        } else if( varName == H_STRINGIFY_VAR( c ) ) {
            c.set( data.date, lexical_cast<double>( data.value_str ) );
        } else {
            H_THROW( "Unknown variable name while parsing " + getComponentName() + ": "
                    + varName );
        }
    } catch( bad_lexical_cast& castException ) {
        H_THROW( "Could not convert var: "+varName+", value: " + data.value_str + ", exception: "
                +castException.what() );
    }
}

//------------------------------------------------------------------------------
// documentation is inherited
void DummyModelComponent::prepareToRun() throw ( h_exception ) {
    
    H_LOG( logger, Logger::DEBUG ) << "prepareToRun " << std::endl;
    
    // do some error checking
    H_ASSERT( slope != -1, "" );
    H_ASSERT( y != -1, "" );
    H_ASSERT( c.size() > 0, "" );
    //    prevX = coreptr->getStartDate();
}

//------------------------------------------------------------------------------
// documentation is inherited
void DummyModelComponent::run( const double runToDate ) throw ( h_exception ) {
    // for some reason we need to calculate at a time-step of .1
    const double timeStep = 0.1;
    // make sure we can take at least 1 time-step.
    H_ASSERT( ( runToDate - prevX ) > timeStep, "time step error" );
    
    for( ; prevX <= runToDate; prevX += timeStep ) {
        y = slope * timeStep + y + c.get( prevX ) - c.get( prevX - timeStep );
        H_LOG( logger, Logger::DEBUG ) << "Y = " << y << " at x " << prevX << std::endl;
    }
}

//------------------------------------------------------------------------------
// documentation is inherited
unitval DummyModelComponent::getData( const std::string& varName,
                                     const double date ) throw ( h_exception ) {
    
    unitval returnval;
    
    if( varName == H_STRINGIFY_VAR( x ) ) {
        H_ASSERT( date == Core::undefinedIndex(), "Date not allowed for x" );
        returnval.set( y, U_UNITLESS );
    } else {
        H_THROW( "Caller is requesting unknown variable: " + varName );
    }
    
    return returnval;
}

//------------------------------------------------------------------------------
// documentation is inherited
void DummyModelComponent::shutDown() {
	H_LOG( logger, Logger::DEBUG ) << "goodbye " << getComponentName() << std::endl;
    logger.close();
}

//------------------------------------------------------------------------------
// documentation is inherited
void DummyModelComponent::accept( AVisitor* visitor ) {
    visitor->visit( this );
}

}
