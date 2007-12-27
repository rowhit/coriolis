// ****************************************************************************************************
// File: Nets.h
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#ifndef HURRICANE_NETS
#define HURRICANE_NETS

#include "Collection.h"

namespace Hurricane {

class Net;



// ****************************************************************************************************
// Nets declaration
// ****************************************************************************************************

typedef GenericCollection<Net*> Nets;



// ****************************************************************************************************
// NetLocator declaration
// ****************************************************************************************************

typedef GenericLocator<Net*> NetLocator;



// ****************************************************************************************************
// NetFilter declaration
// ****************************************************************************************************

typedef GenericFilter<Net*> NetFilter;



// ****************************************************************************************************
// for_each_net declaration
// ****************************************************************************************************

#define for_each_net(net, nets)\
/******************************/\
{\
	NetLocator _locator = nets.GetLocator();\
	while (_locator.IsValid()) {\
		Net* net = _locator.GetElement();\
		_locator.Progress();



} // End of Hurricane namespace.

#endif // HURRICANE_NETS

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************