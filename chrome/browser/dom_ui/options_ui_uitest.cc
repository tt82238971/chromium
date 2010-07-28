// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "app/l10n_util.h"
#include "base/command_line.h"
#include "chrome/app/chrome_dll_resource.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/url_constants.h"
#include "chrome/test/automation/browser_proxy.h"
#include "chrome/test/automation/tab_proxy.h"
#include "chrome/test/automation/window_proxy.h"
#include "chrome/test/ui/ui_test.h"

#include "grit/chromium_strings.h"
#include "grit/generated_resources.h"

namespace {

class OptionsUITest : public UITest {
 public:
  OptionsUITest() {
    dom_automation_enabled_ = true;
    // TODO(csilv): Remove when dom-ui options is enabled by default.
    launch_arguments_.AppendSwitch(switches::kEnableTabbedOptions);
  }

  void AssertIsOptionsPage(TabProxy* tab) {
    std::wstring title;
    ASSERT_TRUE(tab->GetTabTitle(&title));
    std::wstring expected_title =
        l10n_util::GetStringF(IDS_OPTIONS_DIALOG_TITLE,
            l10n_util::GetString(IDS_PRODUCT_NAME));
    ASSERT_EQ(expected_title, title);
  }
};

TEST_F(OptionsUITest, LoadOptionsByURL) {
  scoped_refptr<BrowserProxy> browser(automation()->GetBrowserWindow(0));
  ASSERT_TRUE(browser.get());

  scoped_refptr<TabProxy> tab = browser->GetActiveTab();
  ASSERT_TRUE(tab.get());

  // Go to the options tab via URL.
  NavigateToURL(GURL(chrome::kChromeUIOptionsURL));
  AssertIsOptionsPage(tab);
}

TEST_F(OptionsUITest, CommandOpensOptionsTab) {
  scoped_refptr<BrowserProxy> browser(automation()->GetBrowserWindow(0));
  ASSERT_TRUE(browser.get());

  int tab_count = -1;
  ASSERT_TRUE(browser->GetTabCount(&tab_count));
  ASSERT_EQ(1, tab_count);

  // Bring up the options tab via command.
  ASSERT_TRUE(browser->RunCommand(IDC_OPTIONS));
  ASSERT_TRUE(browser->GetTabCount(&tab_count));
  ASSERT_EQ(2, tab_count);

  scoped_refptr<TabProxy> tab = browser->GetActiveTab();
  ASSERT_TRUE(tab.get());
  AssertIsOptionsPage(tab);
}

// TODO(csilv): Investigate why this fails and fix. http://crbug.com/48521
TEST_F(OptionsUITest, FAILS_CommandAgainGoesBackToOptionsTab) {
  scoped_refptr<BrowserProxy> browser(automation()->GetBrowserWindow(0));
  ASSERT_TRUE(browser.get());

  int tab_count = -1;
  ASSERT_TRUE(browser->GetTabCount(&tab_count));
  ASSERT_EQ(1, tab_count);

  // Bring up the options tab via command.
  ASSERT_TRUE(browser->RunCommand(IDC_OPTIONS));
  ASSERT_TRUE(browser->GetTabCount(&tab_count));
  ASSERT_EQ(2, tab_count);

  scoped_refptr<TabProxy> tab = browser->GetActiveTab();
  ASSERT_TRUE(tab.get());
  AssertIsOptionsPage(tab);

  // Switch to first tab and run command again.
  ASSERT_TRUE(browser->ActivateTab(0));
  ASSERT_TRUE(browser->WaitForTabToBecomeActive(0, action_max_timeout_ms()));
  ASSERT_TRUE(browser->RunCommandAsync(IDC_OPTIONS));

  // Ensure the options ui tab is active.
  ASSERT_TRUE(browser->WaitForTabToBecomeActive(1, action_max_timeout_ms()));
  ASSERT_TRUE(browser->GetTabCount(&tab_count));
  ASSERT_EQ(2, tab_count);
}

// TODO(csilv): Investigate why this fails (sometimes) on 10.5 and fix.
// http://crbug.com/48521
TEST_F(OptionsUITest, FLAKY_TwoCommandsOneTab) {
  scoped_refptr<BrowserProxy> browser(automation()->GetBrowserWindow(0));
  ASSERT_TRUE(browser.get());

  int tab_count = -1;
  ASSERT_TRUE(browser->GetTabCount(&tab_count));
  ASSERT_EQ(1, tab_count);

  ASSERT_TRUE(browser->RunCommand(IDC_OPTIONS));
  ASSERT_TRUE(browser->RunCommandAsync(IDC_OPTIONS));
  ASSERT_TRUE(browser->GetTabCount(&tab_count));
  ASSERT_EQ(2, tab_count);
}

}  // namespace
