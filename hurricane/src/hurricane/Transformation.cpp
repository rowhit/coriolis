// ****************************************************************************************************
// File: Transformation.cpp
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#include "Transformation.h"

namespace Hurricane {

#define TO Transformation::Orientation



// ****************************************************************************************************
// Variables
// ****************************************************************************************************

static const int A[8] = {1, 0, -1, 0, -1, 0, 1, 0};
static const int B[8] = {0, -1, 0, 1, 0, -1, 0, 1};
static const int C[8] = {0, 1, 0, -1, 0, -1, 0, 1};
static const int D[8] = {1, 0, -1, 0, 1, 0, -1, 0};

static const Transformation::Orientation INVERT[8] =
	{TO::ID, TO::R3, TO::R2, TO::R1, TO::MX, TO::XR, TO::MY, TO::YR};

static const int DISCREMINENT[8] = {1, 1, 1, 1, -1, -1, -1, -1};

static const Transformation::Orientation COMPOSE[] = {
	TO::ID, TO::R1, TO::R2, TO::R3, TO::MX, TO::XR, TO::MY, TO::YR,
	TO::R1, TO::R2, TO::R3, TO::ID, TO::XR, TO::MY, TO::YR, TO::MX,
	TO::R2, TO::R3, TO::ID, TO::R1, TO::MY, TO::YR, TO::MX, TO::XR,
	TO::R3, TO::ID, TO::R1, TO::R2, TO::YR, TO::MX, TO::XR, TO::MY,
	TO::MX, TO::YR, TO::MY, TO::XR, TO::ID, TO::R3, TO::R2, TO::R1,
	TO::XR, TO::MX, TO::YR, TO::MY, TO::R1, TO::ID, TO::R3, TO::R2,
	TO::MY, TO::XR, TO::MX, TO::YR, TO::R2, TO::R1, TO::ID, TO::R3,
	TO::YR, TO::MY, TO::XR, TO::MX, TO::R3, TO::R2, TO::R1, TO::ID
};



// ****************************************************************************************************
// Transformation implementation
// ****************************************************************************************************

Transformation::Transformation()
// *****************************
:	_tx(0),
	_ty(0),
	_orientation()
{
}

Transformation::Transformation(const Unit& tx, const Unit& ty, const Orientation& orientation)
// *******************************************************************************************
:	_tx(tx),
	_ty(ty),
	_orientation(orientation)
{
}

Transformation::Transformation(const Point& translation, const Orientation& orientation)
// *************************************************************************************
:	_tx(translation.GetX()),
	_ty(translation.GetY()),
	_orientation(orientation)
{
}

Transformation::Transformation(const Transformation& transformation)
// *****************************************************************
:	_tx(transformation._tx),
	_ty(transformation._ty),
	_orientation(transformation._orientation)
{
}

Transformation& Transformation::operator=(const Transformation& transformation)
// ****************************************************************************
{
	_tx = transformation._tx;
	_ty = transformation._ty;
	_orientation = transformation._orientation;
	return *this;
}

bool Transformation::operator==(const Transformation& transformation) const
// ************************************************************************
{
	return ((_tx == transformation._tx) &&
			  (_ty == transformation._ty) &&
			  (_orientation == transformation._orientation));
}

bool Transformation::operator!=(const Transformation& transformation) const
// ************************************************************************
{
	return ((_tx != transformation._tx) ||
			  (_ty != transformation._ty) ||
			  (_orientation != transformation._orientation));
}

Unit Transformation::GetX(const Unit& x, const Unit& y) const
// **********************************************************
{
	return (x * A[_orientation]) + (y * B[_orientation]) + _tx;
}

Unit Transformation::GetY(const Unit& x, const Unit& y) const
// **********************************************************
{
	return (x * C[_orientation]) + (y * D[_orientation]) + _ty;
}

Unit Transformation::GetX(const Point& point) const
// ************************************************
{
	return GetX(point.GetX(), point.GetY());
}

Unit Transformation::GetY(const Point& point) const
// ************************************************
{
	return GetY(point.GetX(), point.GetY());
}

Unit Transformation::GetDx(const Unit& dx, const Unit& dy) const
// *************************************************************
{
	return (dx * A[_orientation]) + (dy * B[_orientation]);
}

Unit Transformation::GetDy(const Unit& dx, const Unit& dy) const
// *************************************************************
{
	return (dx * C[_orientation]) + (dy * D[_orientation]);
}

Point Transformation::GetPoint(const Unit& x, const Unit& y) const
// ***************************************************************
{
	return Point(GetX(x, y), GetY(x, y));
}

Point Transformation::GetPoint(const Point& point) const
// *****************************************************
{
	return GetPoint(point.GetX(), point.GetY());
}

Box Transformation::GetBox(const Unit& x1, const Unit& y1, const Unit& x2, const Unit& y2) const
// *********************************************************************************************
{
	return Box(GetX(x1, y1), GetY(x1, y1), GetX(x2, y2), GetY(x2, y2));
}

Box Transformation::GetBox(const Point& point1, const Point& point2) const
// ***********************************************************************
{
	return GetBox(point1.GetX(), point1.GetY(), point2.GetX(), point2.GetY());
}

Box Transformation::GetBox(const Box& box) const
// *********************************************
{
	if (box.IsEmpty()) return box;
	return GetBox(box.GetXMin(), box.GetYMin(), box.GetXMax(), box.GetYMax());
}

Transformation Transformation::GetTransformation(const Transformation& transformation) const
// *****************************************************************************************
{
	Unit x = transformation._tx;
	Unit y = transformation._ty;

	return Transformation(
				(x * A[_orientation]) + (y * B[_orientation]) + _tx,
				(x * C[_orientation]) + (y * D[_orientation]) + _ty,
				COMPOSE[(_orientation * 8) + transformation._orientation]);
}

Transformation Transformation::GetInvert() const
// *********************************************
{
	Transformation transformation = *this;
	return transformation.Invert();
}

Transformation& Transformation::Invert()
// *************************************
{
	return operator=(
		Transformation(
			((_ty * B[_orientation]) - (_tx * D[_orientation])) * DISCREMINENT[_orientation],
			((_tx * C[_orientation]) - (_ty * A[_orientation])) * DISCREMINENT[_orientation],
			INVERT[_orientation]));
}

void Transformation::ApplyOn(Unit& x, Unit& y) const
// *************************************************
{
	Unit xi = x, yi = y;
	x = GetX(xi, yi);
	y = GetY(xi, yi);
}

void Transformation::ApplyOn(Point& point) const
// *********************************************
{
	point = GetPoint(point);
}

void Transformation::ApplyOn(Box& box) const
// *****************************************
{
	box = GetBox(box);
}

void Transformation::ApplyOn(Transformation& transformation) const
// ***************************************************************
{
	transformation = GetTransformation(transformation);
}

string Transformation::_GetString() const
// **************************************
{
	return "<" + _TName("Transformation") + " " +
			 GetValueString(_tx) + " " +
			 GetValueString(_ty) + " " +
			 GetString(_orientation) +
			 ">";
}

Record* Transformation::_GetRecord() const
// *********************************
{
	Record* record = new Record(GetString(this));
	record->Add(GetSlot("X", &_tx));
	record->Add(GetSlot("Y", &_ty));
	record->Add(GetSlot("Orientation", _orientation));
	return record;
}



// ****************************************************************************************************
// Transformation::Orientation implementation
// ****************************************************************************************************

Transformation::Orientation::Orientation(const Code& code)
// *******************************************************
:	_code(code)
{
}

Transformation::Orientation::Orientation(const Orientation& orientation)
// *********************************************************************
:	_code(orientation._code)
{
}

Transformation::Orientation& Transformation::Orientation::operator=(const Orientation& orientation)
// ************************************************************************************************
{
	_code = orientation._code;
	return *this;
}

string Transformation::Orientation::_GetString() const
// ***************************************************
{
  return GetString(&_code);
}

Record* Transformation::Orientation::_GetRecord() const
// **********************************************
{
	Record* record = new Record(GetString(this));
	record->Add(GetSlot("Code", &_code));
	return record;
}



} // End of Hurricane namespace.

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************