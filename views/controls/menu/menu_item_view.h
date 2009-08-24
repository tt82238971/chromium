// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VIEWS_CONTROLS_MENU_MENU_ITEM_VIEW_H_
#define VIEWS_CONTROLS_MENU_MENU_ITEM_VIEW_H_

#include "third_party/skia/include/core/SkBitmap.h"
#include "views/view.h"

namespace views {

class MenuController;
class MenuDelegate;
class SubmenuView;

// MenuItemView --------------------------------------------------------------

// MenuItemView represents a single menu item with a label and optional icon.
// Each MenuItemView may also contain a submenu, which in turn may contain
// any number of child MenuItemViews.
//
// To use a menu create an initial MenuItemView using the constructor that
// takes a MenuDelegate, then create any number of child menu items by way
// of the various AddXXX methods.
//
// MenuItemView is itself a View, which means you can add Views to each
// MenuItemView. This normally NOT want you want, rather add other child Views
// to the submenu of the MenuItemView.
//
// There are two ways to show a MenuItemView:
// 1. Use RunMenuAt. This blocks the caller, executing the selected command
//    on success.
// 2. Use RunMenuForDropAt. This is intended for use during a drop session
//    and does NOT block the caller. Instead the delegate is notified when the
//    menu closes via the DropMenuClosed method.

class MenuItemView : public View {
 public:
  friend class MenuController;

  // ID used to identify menu items.
  static const int kMenuItemViewID;

  // ID used to identify empty menu items.
  static const int kEmptyMenuItemViewID;

  // If true SetNestableTasksAllowed(true) is invoked before MessageLoop::Run
  // is invoked. This is only useful for testing and defaults to false.
  static bool allow_task_nesting_during_run_;

  // Different types of menu items.
  enum Type {
    NORMAL,
    SUBMENU,
    CHECKBOX,
    RADIO,
    SEPARATOR
  };

  // Where the menu should be anchored to.
  enum AnchorPosition {
    TOPLEFT,
    TOPRIGHT
  };

  // Constructor for use with the top level menu item. This menu is never
  // shown to the user, rather its use as the parent for all menu items.
  explicit MenuItemView(MenuDelegate* delegate);

  virtual ~MenuItemView();

  // Returns the preferred height of menu items. This is only valid when the
  // menu is about to be shown.
  static int pref_menu_height() { return pref_menu_height_; }

  // X-coordinate of where the label starts.
  static int label_start() { return label_start_; }

  // Run methods. See description above class for details. Both Run methods take
  // a rectangle, which is used to position the menu. |has_mnemonics| indicates
  // whether the items have mnemonics. Mnemonics are identified by way of the
  // character following the '&'.
  void RunMenuAt(gfx::NativeView parent,
                 const gfx::Rect& bounds,
                 AnchorPosition anchor,
                 bool has_mnemonics);
  void RunMenuForDropAt(gfx::NativeView parent,
                        const gfx::Rect& bounds,
                        AnchorPosition anchor);

  // Hides and cancels the menu. This does nothing if the menu is not open.
  void Cancel();

  // Adds an item to this menu.
  // item_id    The id of the item, used to identify it in delegate callbacks
  //            or (if delegate is NULL) to identify the command associated
  //            with this item with the controller specified in the ctor. Note
  //            that this value should not be 0 as this has a special meaning
  //            ("NULL command, no item selected")
  // label      The text label shown.
  // type       The type of item.
  void AppendMenuItem(int item_id,
                      const std::wstring& label,
                      Type type) {
    AppendMenuItemInternal(item_id, label, SkBitmap(), type);
  }

  // Append a submenu to this menu.
  // The returned pointer is owned by this menu.
  MenuItemView* AppendSubMenu(int item_id,
                              const std::wstring& label) {
    return AppendMenuItemInternal(item_id, label, SkBitmap(), SUBMENU);
  }

  // Append a submenu with an icon to this menu.
  // The returned pointer is owned by this menu.
  MenuItemView* AppendSubMenuWithIcon(int item_id,
                                      const std::wstring& label,
                                      const SkBitmap& icon) {
    return AppendMenuItemInternal(item_id, label, icon, SUBMENU);
  }

  // This is a convenience for standard text label menu items where the label
  // is provided with this call.
  void AppendMenuItemWithLabel(int item_id,
                               const std::wstring& label) {
    AppendMenuItem(item_id, label, NORMAL);
  }

  // This is a convenience for text label menu items where the label is
  // provided by the delegate.
  void AppendDelegateMenuItem(int item_id) {
    AppendMenuItem(item_id, std::wstring(), NORMAL);
  }

  // Adds a separator to this menu
  void AppendSeparator() {
    AppendMenuItemInternal(0, std::wstring(), SkBitmap(), SEPARATOR);
  }

  // Appends a menu item with an icon. This is for the menu item which
  // needs an icon. Calling this function forces the Menu class to draw
  // the menu, instead of relying on Windows.
  void AppendMenuItemWithIcon(int item_id,
                              const std::wstring& label,
                              const SkBitmap& icon) {
    AppendMenuItemInternal(item_id, label, icon, NORMAL);
  }

  // Returns the view that contains child menu items. If the submenu has
  // not been creates, this creates it.
  virtual SubmenuView* CreateSubmenu();

  // Returns true if this menu item has a submenu.
  virtual bool HasSubmenu() const { return (submenu_ != NULL); }

  // Returns the view containing child menu items.
  virtual SubmenuView* GetSubmenu() const { return submenu_; }

  // Returns the parent menu item.
  MenuItemView* GetParentMenuItem() const { return parent_menu_item_; }

  // Sets the title
  void SetTitle(const std::wstring& title) {
    title_ = title;
  }

  // Returns the title.
  const std::wstring& GetTitle() const { return title_; }

  // Sets whether this item is selected. This is invoked as the user moves
  // the mouse around the menu while open.
  void SetSelected(bool selected);

  // Returns true if the item is selected.
  bool IsSelected() const { return selected_; }

  // Sets the icon for the descendant identified by item_id.
  void SetIcon(const SkBitmap& icon, int item_id);

  // Sets the icon of this menu item.
  void SetIcon(const SkBitmap& icon);

  // Returns the icon.
  const SkBitmap& GetIcon() const { return icon_; }

  // Sets the command id of this menu item.
  void SetCommand(int command) { command_ = command; }

  // Returns the command id of this item.
  int GetCommand() const { return command_; }

  // Paints the menu item.
  virtual void Paint(gfx::Canvas* canvas);

  // Returns the preferred size of this item.
  virtual gfx::Size GetPreferredSize();

  // Returns the object responsible for controlling showing the menu.
  MenuController* GetMenuController();

  // Returns the delegate. This returns the delegate of the root menu item.
  MenuDelegate* GetDelegate();

  // Returns the root parent, or this if this has no parent.
  MenuItemView* GetRootMenuItem();

  // Returns the mnemonic for this MenuItemView, or 0 if this MenuItemView
  // doesn't have a mnemonic.
  wchar_t GetMnemonic();

  // Do we have icons? This only has effect on the top menu. Turning this on
  // makes the menus slightly wider and taller.
  void set_has_icons(bool has_icons) {
    has_icons_ = has_icons;
  }

 protected:
  // Creates a MenuItemView. This is used by the various AddXXX methods.
  MenuItemView(MenuItemView* parent, int command, Type type);

 private:
  // Calculates all sizes that we can from the OS.
  //
  // This is invoked prior to Running a menu.
  static void UpdateMenuPartSizes(bool has_icons);

  // Called by the two constructors to initialize this menu item.
  void Init(MenuItemView* parent,
            int command,
            MenuItemView::Type type,
            MenuDelegate* delegate);

  // All the AddXXX methods funnel into this.
  MenuItemView* AppendMenuItemInternal(int item_id,
                                       const std::wstring& label,
                                       const SkBitmap& icon,
                                       Type type);

  // Returns the descendant with the specified command.
  MenuItemView* GetDescendantByID(int id);

  // Invoked by the MenuController when the menu closes as the result of
  // drag and drop run.
  void DropMenuClosed(bool notify_delegate);

  // The RunXXX methods call into this to set up the necessary state before
  // running.
  void PrepareForRun(bool has_mnemonics);

  // Returns the flags passed to DrawStringInt.
  int GetDrawStringFlags();

  // If this menu item has no children a child is added showing it has no
  // children. Otherwise AddEmtpyMenuIfNecessary is recursively invoked on
  // child menu items that have children.
  void AddEmptyMenus();

  // Undoes the work of AddEmptyMenus.
  void RemoveEmptyMenus();

  // Given bounds within our View, this helper routine mirrors the bounds if
  // necessary.
  void AdjustBoundsForRTLUI(gfx::Rect* rect) const;

  // Actual paint implementation. If for_drag is true, portions of the menu
  // are not rendered.
  void Paint(gfx::Canvas* canvas, bool for_drag);

  // Destroys the window used to display this menu and recursively destroys
  // the windows used to display all descendants.
  void DestroyAllMenuHosts();

  // Returns the various margins.
  int GetTopMargin();
  int GetBottomMargin();

  // The delegate. This is only valid for the root menu item. You shouldn't
  // use this directly, instead use GetDelegate() which walks the tree as
  // as necessary.
  MenuDelegate* delegate_;

  // Returns the controller for the run operation, or NULL if the menu isn't
  // showing.
  MenuController* controller_;

  // Used to detect when Cancel was invoked.
  bool canceled_;

  // Our parent.
  MenuItemView* parent_menu_item_;

  // Type of menu. NOTE: MenuItemView doesn't itself represent SEPARATOR,
  // that is handled by an entirely different view class.
  Type type_;

  // Whether we're selected.
  bool selected_;

  // Command id.
  int command_;

  // Submenu, created via CreateSubmenu.
  SubmenuView* submenu_;

  // Title.
  std::wstring title_;

  // Icon.
  SkBitmap icon_;

  // Does the title have a mnemonic?
  bool has_mnemonics_;

  bool has_icons_;

  // X-coordinate of where the label starts.
  static int label_start_;

  // Margins between the right of the item and the label.
  static int item_right_margin_;

  // Preferred height of menu items. Reset every time a menu is run.
  static int pref_menu_height_;

  DISALLOW_COPY_AND_ASSIGN(MenuItemView);
};

}  // namespace views

#endif  // VIEWS_CONTROLS_MENU_MENU_ITEM_VIEW_H_
