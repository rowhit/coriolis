
// -*- C++ -*-
//
// Copyright (c) BULL S.A. 2000-2010, All Rights Reserved
//
// This file is part of Hurricane.
//
// Hurricane is free software: you can redistribute it  and/or  modify
// it under the terms of the GNU  Lesser  General  Public  License  as
// published by the Free Software Foundation, either version 3 of  the
// License, or (at your option) any later version.
//
// Hurricane is distributed in the hope that it will  be  useful,  but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHAN-
// TABILITY or FITNESS FOR A PARTICULAR PURPOSE. See  the  Lesser  GNU
// General Public License for more details.
//
// You should have received a copy of the Lesser  GNU  General  Public
// License along with Hurricane. If not, see
//                                     <http://www.gnu.org/licenses/>.
//
// ===================================================================
//
// $Id$
//
// x-----------------------------------------------------------------x
// |                                                                 |
// |                  H U R R I C A N E                              |
// |     V L S I   B a c k e n d   D a t a - B a s e                 |
// |                                                                 |
// |  Authors     :          Hugo Clement & Marek Sroka              |
// |  E-mail      :            Jean-Paul.Chaput@lip6.fr              |
// | =============================================================== |
// |  C++ Header  :  "./RoutingPad.h"                                |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


#include "hurricane/Net.h"
#include "hurricane/NetExternalComponents.h"
#include "hurricane/Layer.h"
#include "hurricane/Plug.h"
#include "hurricane/Pin.h"
#include "hurricane/Segment.h"
#include "hurricane/Horizontal.h"
#include "hurricane/Vertical.h"
#include "hurricane/Cell.h"
#include "hurricane/Instance.h"
#include "hurricane/Error.h"
#include "hurricane/RoutingPad.h"


namespace Hurricane {


  RoutingPad::RoutingPad ( Net* net, const Point& p, Occurrence occurrence )
    :  Inherit   (net)
    , _x         (p.getX())
    , _y         (p.getY())
    , _occurrence(occurrence)
  { }


  RoutingPad* RoutingPad::create ( Net* net, Occurrence occurrence, unsigned int flags )
  {
    if ( not net ) throw Error ("Can't create RoutingPad : NULL net");
    if ( not occurrence.isValid() ) throw Error ("Can't create RoutingPag : Invalid occurrence");

    Plug*    plug    = NULL;
    Pin*     pin     = NULL;
    Contact* contact = NULL;
    Point    position;

    if ( (plug = dynamic_cast<Plug*>(occurrence.getEntity()) ) ) {
      position = occurrence.getPath().getTransformation().getPoint( plug->getPosition() );
    } else if ( (pin = dynamic_cast<Pin*>(occurrence.getEntity()) ) ) {
      position = occurrence.getPath().getTransformation().getPoint( pin->getPosition() );
    } else if ( (contact = dynamic_cast<Contact*>(occurrence.getEntity()) ) ) {
      position = occurrence.getPath().getTransformation().getPoint( contact->getPosition() );
    }

    if ( (not plug) and (not pin) and (not contact) )
      throw Error ("Can't create RoutingPad : Plug Pin, or Contact Occurrence *required*");

    RoutingPad* routingPad = new RoutingPad(net, position, occurrence);
    routingPad->_postCreate();

    if ( plug and (flags & ComponentSelection) ) routingPad->setOnBestComponent(flags);

    return routingPad;
  }


  void  RoutingPad::_postCreate ()
  {
    Inherit::_postCreate();

    if ( not _occurrence.getPath().isEmpty() )
      _occurrence.getMasterCell()->_addSlaveEntity(_occurrence.getEntity(),this);
  }


  RoutingPad* RoutingPad::create ( Pin* pin )
  {
    Occurrence  pinOccurrence ( pin, Path() );

#if 0
    forEach ( RoutingPad*, irp, pin->getNet()->getRoutingPads() ) {
      if ( (*irp)->getOccurrence() == pinOccurrence )
        return (*irp);
    }
#endif

    return RoutingPad::create ( pin->getNet(), pinOccurrence );
  }


  DbU::Unit RoutingPad::getX       () const { return _x; }
  DbU::Unit RoutingPad::getY       () const { return _y; }
  DbU::Unit RoutingPad::getSourceX () const { return getSourcePosition().getX(); }
  DbU::Unit RoutingPad::getSourceY () const { return getSourcePosition().getY(); }
  DbU::Unit RoutingPad::getTargetX () const { return getTargetPosition().getX(); }
  DbU::Unit RoutingPad::getTargetY () const { return getTargetPosition().getY(); }


  Box RoutingPad::getBoundingBox () const
  {
    Component* component = _getEntityAsComponent();
    if ( component )
      return _occurrence.getPath().getTransformation().getBox ( component->getBoundingBox() );

    return Box(getPosition());
  }


  Box RoutingPad::getBoundingBox ( const BasicLayer* basicLayer ) const
  {
    Component* component = _getEntityAsComponent();
    if ( component )
      return _occurrence.getPath().getTransformation().getBox ( component->getBoundingBox(basicLayer) );

    return Box(getPosition());
  }


  const Layer* RoutingPad::getLayer () const
  {
    Component* component = _getEntityAsComponent();
    if ( component ) return component->getLayer ();

    return NULL;
  }


  Point RoutingPad::getSourcePosition () const
  {
    Segment* segment = _getEntityAsSegment();
    if ( segment )
      return _occurrence.getPath().getTransformation().getPoint ( segment->getSourcePosition() );

    return getPosition();
  }


  Point RoutingPad::getTargetPosition() const
  {
    Segment* segment = _getEntityAsSegment();
    if ( segment )
      return _occurrence.getPath().getTransformation().getPoint ( segment->getTargetPosition() );

    return getPosition();
  }


  Point RoutingPad::getCenter() const
  {
    Segment* segment = _getEntityAsSegment();
    if ( segment )
      return _occurrence.getPath().getTransformation().getPoint ( segment->getCenter() );
    
    return getPosition();
  }


  void RoutingPad::translate(const DbU::Unit& dx, const DbU::Unit& dy)
  {
    if ( (dx != 0) or (dy != 0) ) {
      invalidate(true);
      _x += dx;
      _y += dy;
    }
  }


  void RoutingPad::setX        ( const DbU::Unit& x ) { setPosition(x, getY()); }
  void RoutingPad::setY        ( const DbU::Unit& y ) { setPosition(getX(), y); }
  void RoutingPad::setPosition ( const DbU::Unit& x, const DbU::Unit& y ) { setOffset(x, y); }
  void RoutingPad::setPosition ( const Point& position) { setPosition(position.getX(), position.getY()); }


  void RoutingPad::setOffset ( const DbU::Unit& x, const DbU::Unit& y )
  {
    invalidate(true);
    _x = x;
    _y = y;
  }


  void RoutingPad::_preDestroy ()
  {
  // trace << "entering RoutingPad::preDestroy: " << this << endl;
  // trace_in();

    if ( not _occurrence.getPath().isEmpty() )
      _occurrence.getMasterCell()->_removeSlaveEntity(_occurrence.getEntity(),this);
    Inherit::_preDestroy();

  // trace << "exiting RoutingPad::preDestroy:" << endl;
  // trace_out();
  }


  string RoutingPad::_getString () const
  {
    string s = Inherit::_getString();
    s.insert(s.length() - 1, " [" + DbU::getValueString(getX()));
    s.insert(s.length() - 1, " " + DbU::getValueString(getY()));
    s.insert(s.length() - 1, "] ");
    s.insert(s.length() - 1, getString(_occurrence));
    return s;
  }


  Record* RoutingPad::_getRecord () const
  {
    Record* record = Inherit::_getRecord();
    if ( record ) {
      record->add(getSlot("_x"         , &_x       ));
      record->add(getSlot("_y"         , &_y       ));
      record->add(getSlot("_occurrence",_occurrence));
    }
    return record;
  }


  Component* RoutingPad::_getEntityAsComponent () const
  {
    if ( _occurrence.isValid() )
      return dynamic_cast<Component*>( _occurrence.getEntity() );

    return NULL;
  }


  Segment* RoutingPad::_getEntityAsSegment () const
  {
    if ( _occurrence.isValid() )
      return dynamic_cast<Segment*>( _occurrence.getEntity() );
    
    return NULL;
  }


  void RoutingPad::setExternalComponent ( Component* component )
  {
    if ( isMaterialized() ) invalidate(false);

    Occurrence plugOccurrence = getPlugOccurrence();
    Plug*      plug           = static_cast<Plug*>(plugOccurrence.getEntity());
    if ( plug->getMasterNet() != component->getNet() )
      throw Error("Cannot Set External Component to Routing Pad : Inconsistant Net");

    _occurrence.getMasterCell()->_removeSlaveEntity(_occurrence.getEntity(),this);
    _occurrence = Occurrence(component,Path(plugOccurrence.getPath(),plug->getInstance()));

    Point       position   = _occurrence.getPath().getTransformation().getPoint ( component->getPosition() );
    Horizontal* horizontal = dynamic_cast<Horizontal*>(component);

    if ( horizontal ) {
      setX ( 0 );
      setY ( position.getY() );
    } else {
      Vertical* vertical = dynamic_cast<Vertical*>(component);
      if ( vertical ) {
        setX ( position.getX() );
        setY ( 0 );
      } else
        setPosition ( position );
    }

    _occurrence.getMasterCell()->_addSlaveEntity(_occurrence.getEntity(),this);

    if (!isMaterialized()) materialize();
  }


  Occurrence RoutingPad::getPlugOccurrence ()
  {
    if (dynamic_cast<Plug*>(_occurrence.getEntity()))
      return _occurrence;

    Component* component = static_cast<Component*>(_occurrence.getEntity());
    Net*       net       = component->getNet();
    Path       path      = _occurrence.getPath();

    if ( path.isEmpty() )
      throw Error("Empty Path => not in an instance");

    Instance* instance = path.getTailInstance();
    Plug*     plug     = instance->getPlug(net);

    return Occurrence(plug,path.getHeadPath());
  }


  void RoutingPad::restorePlugOccurrence ()
  {
    if (isMaterialized()) unmaterialize();

    _occurrence=getPlugOccurrence();
    setPosition ( _occurrence.getPath().getTransformation().getPoint
      ( dynamic_cast<Component*>(_occurrence.getEntity())->getPosition() ) );
  }


  Component* RoutingPad::setOnBestComponent ( unsigned int flags )
  {
    restorePlugOccurrence ();

    Component* bestComponent = NULL;
    Plug*      plug          = static_cast<Plug*>(_occurrence.getEntity());

    forEach ( Component*, icomponent, NetExternalComponents::get(plug->getMasterNet()) ) {
      if ( not bestComponent ) { bestComponent = *icomponent; continue; }

      switch ( flags & ComponentSelection ) {
        case LowestLayer:
          if ( icomponent->getLayer()->below(bestComponent->getLayer()) )
            bestComponent = *icomponent;
          break;
        case HighestLayer:
          if ( icomponent->getLayer()->above(bestComponent->getLayer()) )
            bestComponent = *icomponent;
          break;
        case BiggestArea:
        default:
           if ( getArea(*icomponent) > getArea(bestComponent) )
             bestComponent = *icomponent;
           break;
      }
    }

    if ( not bestComponent )
      throw Error ( "RoutingPad::_getBestComponent(): No external components for\n"
                   "  %s of %s."
                  ,getString(plug->getMasterNet()).c_str()
                  ,getString(plug->getInstance ()).c_str() );

    setExternalComponent ( bestComponent );

    return bestComponent;
  }


}   // End of Hurricane namespace.
