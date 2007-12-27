// ****************************************************************************************************
// File: ListCollection.h
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#ifndef HURRICANE_LIST_COLLECTION
#define HURRICANE_LIST_COLLECTION

#include "Commons.h"

namespace Hurricane {



// ****************************************************************************************************
// ListCollection declaration
// ****************************************************************************************************

template<class Element> class ListCollection : public Collection<Element> {
// **********************************************************************

// Types
// *****

	public: typedef Collection<Element> Inherit;

	public: typedef list<Element> ElementList;

	public: class Locator : public Hurricane::Locator<Element> {
	// *******************************************************

		public: typedef Hurricane::Locator<Element> Inherit;

		private: const ElementList* _elementList;
		private: typename ElementList::const_iterator _iterator; // AD

		public: Locator(const ElementList* elementList)
		// ********************************************
		:	Inherit(),
			_elementList(elementList),
			_iterator()
		{
			if (_elementList) _iterator = _elementList->begin();
		};

		public: virtual Element GetElement() const
		// ***************************************
		{
			return (IsValid()) ? *_iterator : Element();
		};

		public: virtual Hurricane::Locator<Element>* GetClone() const
		// **********************************************************
		{
			return new Locator(_elementList);
		};

		public: virtual bool IsValid() const
		// *********************************
		{
			return (_elementList && (_iterator != _elementList->end()));
		};

		public: virtual void Progress()
		// ****************************
		{
			++_iterator;
		};

	};

// Attributes
// **********

	private: const ElementList* _elementList;

// Constructors
// ************

	public: ListCollection(const ElementList* elementList = NULL)
	// **********************************************************
	:	Inherit(),
		_elementList(elementList)
	{
	};

	public: ListCollection(const ElementList& elementList)
	// ***************************************************
	:	Inherit(),
		_elementList(&elementList)
	{
	};

	public: ListCollection(const ListCollection& listCollection)
	// *********************************************************
	:	Inherit(),
		_elementList(listCollection._elementList)
	{
	};

// Operators
// *********

	public: ListCollection& operator=(const ListCollection& listCollection)
	// ********************************************************************
	{
		_elementList = listCollection._elementList;
		return *this;
	};

// Accessors
// *********

	public: virtual Collection<Element>* GetClone() const
	// **************************************************
	{
		return new ListCollection(*this);
	}

	public: virtual Hurricane::Locator<Element>* GetLocator() const
	// ************************************************************
	{
		// return (_elementList) ? new Locator<Element>(_elementList) : NULL;
		// V3
		return (_elementList) ? new Locator(_elementList) : NULL;
	}

	public: virtual unsigned GetSize() const
	// *************************************
	{
		return (_elementList) ? _elementList->size() : 0;
	};

// Others
// ******

    public: virtual string _GetTypeName() const
	// **************************************
    {
      return _TName("ListCollection");
    };

	public: virtual string _GetString() const
	// **************************************
	{
		if (!_elementList)
			return "<" + _GetTypeName() + " unbound>";
		else {
			if (_elementList->empty())
				return "<" + _GetTypeName() + " empty>";
			else
				return "<" + _GetTypeName() + " " + GetString(_elementList->size()) + ">";
		}
	};

    public: Record* _GetRecord() const
    // *************************
    {
      Record* record = NULL;
      if (!_elementList->empty()) {
		record = new Record(_GetString());
		unsigned n = 1;
		typename list<Element>::const_iterator iterator = _elementList->begin(); // AD
		while (iterator != _elementList->end()) {
          string   slotName   = GetString(n++);
          Element  slotObject = *iterator;
          record->Add(GetSlot(slotName, slotObject));
          ++iterator;
		}
      }
      return record;
    }

};



// ****************************************************************************************************
// Generic functions
// ****************************************************************************************************

template<class Element>
	inline GenericCollection<Element> GetCollection(const list<Element>& elementList)
// *********************************************************************************
{
	return ListCollection<Element>(elementList);
}

template<class Element>
	inline GenericCollection<Element> GetCollection(const list<Element>* elementList)
// *********************************************************************************
{
	return ListCollection<Element>(elementList);
}



} // End of Hurricane namespace.

#endif // HURRICANE_LIST_COLLECTION

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************