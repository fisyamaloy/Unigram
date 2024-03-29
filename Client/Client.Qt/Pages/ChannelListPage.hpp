#pragma once

#include <Network/Primitives.hpp>
#include <QVBoxLayout>
#include <QWidget>

#include "Widgets/ListWidget.hpp"
#include "Widgets/Page.hpp"

class FlatButton;

/**
 *  @class ChannelListPage
 *  @brief This is channel list
 */
class ChannelListPage : public Page
{
public:
    /// Constructor for channel list window
    explicit ChannelListPage(std::shared_ptr<ListWidget>& anotherChannelListWidget, QWidget* parent = nullptr);

public:
    /// T\todo rework channel list in main window to make this private
    inline static std::vector<Network::ChannelInfo> channels;

private:
    void updateLayout();
private slots:
    /// set channels
    void setChannels(const std::vector<Network::ChannelInfo>& channels_);
    /// Adds selected channel to main window
    void addChannelToChannelListWidget();
    /// Adds subscribed channel to main window
    void addSubscribedChannelToMainChannelWidget(const std::vector<uint64_t>& ChannelSubscribeList);
    /// Do a request to server for new channels
    void requestChannels();
    /// Update channel view
    void updateChannelList();

private:
    inline static std::map<std::uint64_t, bool> _channelsAddMap;

    std::unique_ptr<ListWidget> _channelList;
    std::unique_ptr<FlatButton> _addChannelButton;
    std::unique_ptr<FlatButton> _updateChannelButton;
    std::shared_ptr<ListWidget> _widgetChannelList;
};
