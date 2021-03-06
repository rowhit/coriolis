
// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC/LIP6 2008-2010, All Rights Reserved
//
// +-----------------------------------------------------------------+ 
// |                   C O R I O L I S                               |
// |          U n i c o r n  -  M a i n   G U I                      |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Header  :       "./unicorn/OpenCellDialog.h"               |
// +-----------------------------------------------------------------+


#ifndef  __UNICORN_OPEN_CELL_DIALOG_H__
#define  __UNICORN_OPEN_CELL_DIALOG_H__

#include  <QDialog>

class QCheckBox;
class QLineEdit;


namespace Unicorn {


  class OpenCellDialog : public QDialog {
      Q_OBJECT;

    public:
      static  bool           runDialog        ( QWidget* parent, QString& name, bool& newViewerRequest );
              const QString  getCellName      () const;
              bool           newViewerRequest () const;
    protected:
                             OpenCellDialog   ( QWidget* parent=NULL );
    protected:
              QLineEdit*     _lineEdit;
              QCheckBox*     _viewerCheckBox;
  };




} // End of Unicorn namespace.


#endif
