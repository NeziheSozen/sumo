/****************************************************************************/
/// @file    GUIDialog_GLChosenEditor.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 11.03.2004
/// @version $Id$
///
// Editor for the list of chosen objects
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/foxtools/MFXUtils.h>
#include "GUIDialog_GLChosenEditor.h"
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_GLChosenEditor) GUIDialog_GLChosenEditorMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,       GUIDialog_GLChosenEditor::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,       GUIDialog_GLChosenEditor::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_DESELECT,   GUIDialog_GLChosenEditor::onCmdDeselect),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,      GUIDialog_GLChosenEditor::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,             GUIDialog_GLChosenEditor::onCmdClose),
};

FXIMPLEMENT(GUIDialog_GLChosenEditor, FXMainWindow, GUIDialog_GLChosenEditorMap, ARRAYNUMBER(GUIDialog_GLChosenEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_GLChosenEditor::GUIDialog_GLChosenEditor(GUIMainWindow *parent,
        GUISelectedStorage *str) throw()
        : FXMainWindow(parent->getApp(), "List of Selected Items", NULL, NULL, DECOR_ALL, 20,20,300, 300),
        myParent(parent), myStorage(str) {
    myStorage->add2Update(this);
    FXHorizontalFrame *hbox =
        new FXHorizontalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,
                              0,0,0,0);
    // build the list
    myList = new FXList(hbox, 0, 0,
                        LAYOUT_FILL_X|LAYOUT_FILL_Y|LIST_MULTIPLESELECT);
    rebuildList();
    // build the layout
    FXVerticalFrame *layout = new FXVerticalFrame(hbox, LAYOUT_TOP,0,0,0,0,
            4,4,4,4);
    // "Load"
    new FXButton(layout, "Load\t\t", 0, this, MID_CHOOSEN_LOAD,
                 ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "Save"
    new FXButton(layout, "Save\t\t", 0, this, MID_CHOOSEN_SAVE,
                 ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);

    new FXHorizontalSeparator(layout,SEPARATOR_GROOVE|LAYOUT_FILL_X);

    // "Deselect Chosen"
    new FXButton(layout, "Deselect Chosen\t\t", 0, this, MID_CHOOSEN_DESELECT,
                 ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "Clear List"
    new FXButton(layout, "Clear\t\t", 0, this, MID_CHOOSEN_CLEAR,
                 ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);

    new FXHorizontalSeparator(layout,SEPARATOR_GROOVE|LAYOUT_FILL_X);

    // "Close"
    new FXButton(layout, "Close\t\t", 0, this, MID_CANCEL,
                 ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    setIcon(GUIIconSubSys::getIcon(ICON_APP_SELECTOR));
    myParent->addChild(this);
}


GUIDialog_GLChosenEditor::~GUIDialog_GLChosenEditor() throw() {
    myStorage->remove2Update(this);
    myParent->removeChild(this);
}


void
GUIDialog_GLChosenEditor::rebuildList() throw() {
    myList->clearItems();
    const std::vector<GLuint> &chosen = gSelected.getSelected();
    for (std::vector<GLuint>::const_iterator i=chosen.begin(); i!=chosen.end(); ++i) {
        GUIGlObject *object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(*i);
        if (object!=0) {
            std::string name = object->getFullName();
            FXListItem *item = myList->getItem(myList->appendItem(name.c_str()));
            item->setData((void*) *i);
            GUIGlObjectStorage::gIDStorage.unblockObject(*i);
        }
    }
    update();
}


long
GUIDialog_GLChosenEditor::onCmdLoad(FXObject*,FXSelector,void*) {
    // get the new file name
    FXFileDialog opendialog(this,"Open List of Selected Items");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("*.txt");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        std::string msg;
        if (!myParent->loadSelection(file, msg)) {
            FXMessageBox::error(this, MBOX_OK, "Loading failed.", msg.c_str());
        }
        rebuildList();
    }
    return 1;
}


long
GUIDialog_GLChosenEditor::onCmdSave(FXObject*,FXSelector,void*) {
    FXString file = MFXUtils::getFilename2Write(this, "Save List of selected Items", ".txt", GUIIconSubSys::getIcon(ICON_EMPTY), gCurrentFolder);
    if (file=="") {
        return 1;
    }
    try {
        gSelected.save(-1, file.text());
    } catch (IOError &e) {
        FXMessageBox::error(this, MBOX_OK, "Storing failed!", e.what());
    }
    return 1;
}


long
GUIDialog_GLChosenEditor::onCmdDeselect(FXObject*,FXSelector,void*) {
    FXint no = myList->getNumItems();
    FXint i;
    std::vector<GLuint> selected;
    for (i=0; i<no; ++i) {
        if (myList->getItem(i)->isSelected()) {
            selected.push_back((GLuint)(FXuval) myList->getItem(i)->getData());
        }
    }
    // remove items from list
    for (i=0; i<(FXint) selected.size(); ++i) {
        gSelected.deselect(-1, selected[i]);
    }
    // rebuild list
    rebuildList();
    myParent->updateChildren();
    return 1;
}



long
GUIDialog_GLChosenEditor::onCmdClear(FXObject*,FXSelector,void*) {
    myList->clearItems();
    gSelected.clear();
    myParent->updateChildren();
    return 1;
}



long
GUIDialog_GLChosenEditor::onCmdClose(FXObject*,FXSelector,void*) {
    close(true);
    return 1;
}



/****************************************************************************/

