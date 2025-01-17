/****************************************************************************/
/// @file    MFXAddEditTypedTable.h
/// @author  Daniel Krajzewicz
/// @date    2004-07-02
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MFXAddEditTypedTable_h
#define MFXAddEditTypedTable_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MFXEditableTable.h"
#include <vector>
#include <utils/foxtools/FXRealSpinDial.h>

enum CellType {
    CT_UNDEFINED = -1,
    CT_REAL = 0,
    CT_STRING = 1,
    CT_INT = 2,
    CT_BOOL = 3,
    CT_ENUM = 4,
    CT_MAX
};



class MFXAddEditTypedTable : public FXTable {
    FXDECLARE(MFXAddEditTypedTable)
public:
    MFXAddEditTypedTable(FXComposite *p,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint pl=DEFAULT_MARGIN,FXint pr=DEFAULT_MARGIN,FXint pt=DEFAULT_MARGIN,FXint pb=DEFAULT_MARGIN);
    ~MFXAddEditTypedTable();

public:
    struct NumberCellParams {
        int pos;
        double min;
        double max;
        double steps1;
        double steps2;
        double steps3;
        std::string format;
    };

    struct EditedTableItem {
        FXTableItem *item;
        int row;
        int col;
        bool updateOnly;
    };


    CellType getCellType(size_t pos) const;
    void setCellType(size_t pos, CellType t);
    void setNumberCellParams(size_t pos, double min, double max,
                             double steps1, double steps2, double steps3,
                             const std::string &format);
    NumberCellParams getNumberCellParams(size_t pos) const;
    void setEnums(size_t pos, const std::vector<std::string> &params);
    void addEnum(size_t pos, const std::string &e);
    const std::vector<std::string> &getEnums(size_t pos) const;
    /*
        class FXTableItem_Int : public FXTableItem {
        public:
            FXTableItem_Int(const FXString& text,FXIcon* ic=NULL,void* ptr=NULL);
            ~FXTableItem_Int();
        protected:
            /// Create input control for editing this item
            virtual FXWindow *getControlFor(FXTable* table);

            /// Set value from input control
            virtual void setFromControl(FXWindow *control);

        };

        class FXTableItem_Real : public FXTableItem {
        public:
            FXTableItem_Real(const FXString& text,FXIcon* ic=NULL,void* ptr=NULL);
            ~FXTableItem_Real();
        protected:
            /// Create input control for editing this item
            virtual FXWindow *getControlFor(FXTable* table);

            /// Set value from input control
            virtual void setFromControl(FXWindow *control);

        };

        class FXTableItem_Enum : public FXTableItem {
        public:
            FXTableItem_Enum(const FXString& text,FXIcon* ic=NULL,void* ptr=NULL);
            ~FXTableItem_Enum();
        protected:
            /// Create input control for editing this item
            virtual FXWindow *getControlFor(FXTable* table);

            /// Set value from input control
            virtual void setFromControl(FXWindow *control);

        };

        class FXTableItem_Bool : public FXTableItem {
        public:
            FXTableItem_Bool(const FXString& text,FXIcon* ic=NULL,void* ptr=NULL);
            ~FXTableItem_Bool();
        protected:
            /// Create input control for editing this item
            virtual FXWindow *getControlFor(FXTable* table);

            /// Set value from input control
            virtual void setFromControl(FXWindow *control);

        };
    */

    enum {
        ID_TEXT_CHANGED = FXTable::ID_LAST,
        ID_LAST
    };

    void cancelInput();
    long onClicked(FXObject*,FXSelector,void* ptr);
    long onDoubleClicked(FXObject*,FXSelector,void* ptr);
    long onLeftBtnRelease(FXObject*,FXSelector,void* ptr);
    long onLeftBtnPress(FXObject*,FXSelector,void* ptr);

protected:
    virtual FXWindow *getControlForItem(FXint r,FXint c);
    virtual void setItemFromControl(FXint r,FXint c,FXWindow *control);
    void acceptInput(FXbool notify);
    void setItemFromControl_NoRelease(FXint r,FXint c,FXWindow *control);

protected:
    std::vector<CellType> myCellTypes;
    std::vector<NumberCellParams> myNumberCellParams;
    std::vector<std::vector<std::string> > myEnums;

protected:
    MFXAddEditTypedTable() { }

};


#endif

/****************************************************************************/

