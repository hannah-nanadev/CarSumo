#pragma once
enum class StateID
{
	kNone,
	kTitle,
	kMenu,
	kCarSelectSolo,
	kCarSelectHost,
	kCarSelectJoin,
	kGame,
	kPause,
	kNetworkPause,
	kSettings,
	kP1Win,
	kP2Win,
	kDraw,
	kHostGame,
	kJoinGame
};

static const char* StateIDNames[] = {
	"None",
	"Title",
	"Menu",
	"CarSelectSolo",
	"CarSelectHost",
	"CarSelectJoin",
	"Game",
	"Pause",
	"NetworkPause",
	"Settings",
	"P1Win",
	"P2Win",
	"Draw",
	"HostGame",
	"JoinGame"
};