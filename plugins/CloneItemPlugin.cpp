// -*- mode: C++; coding: utf-8-unix; -*-

/**
 * @file  CloneItemPlugin.cpp
 * @brief
 * @author Tatsuya Ishikawa
 */

#include <cnoid/Plugin>
#include <cnoid/ItemTreeView>
#include <cnoid/ToolBar>
#include <cnoid/BodyItem>
#include <iostream>
using namespace cnoid;

class CloneItemPlugin : public Plugin
{
    int half_side_num_; // num of items: (var * 2 - 1) * (var * 2 - 1)
    double interval_;

public:
    CloneItemPlugin() : Plugin("CloneItem")
    {
        require("Body");
    }

    virtual bool initialize() override
    {
        half_side_num_ = 3;
        interval_  = 0.05;
        ToolBar* bar = new ToolBar("CloneItem");
        bar->addButton("Clone")
            ->sigClicked().connect([this]() { this->cloneItem(this->half_side_num_, this->interval_); });
        addToolBar(bar);
        return true;
    }

private:
    void cloneItem(const int half_side_num, const double interval)
    {
        BodyItemPtr item = ItemTreeView::instance()->selectedItem<BodyItem>();
        ItemPtr parent = item->parentItem();
        for (int i = 0; i < half_side_num * 2 - 1; ++i) {
            for (int j = 0; j < half_side_num * 2 - 1; ++j) {
                if (i != half_side_num && j != half_side_num) {
                    BodyItemPtr duplicated = static_cast<BodyItem*>(item->duplicate());
                    LinkPtr root = duplicated->body()->rootLink();
                    // TODO: Reference position from original item
                    root->p()[0] = (i + 1 - half_side_num) * interval; // x
                    root->p()[1] = (j + 1 - half_side_num) * interval; // y
                    parent->addChildItem(duplicated);
                    ItemTreeView::instance()->checkItem(duplicated);
                }
            }
        }
    }
};

CNOID_IMPLEMENT_PLUGIN_ENTRY(CloneItemPlugin)
