#pragma once

#include <functional>

// Callbacks for communicating some kind of progress
// This is primarily created to get rid of the Game debendencies due to the Game::LoadingScreen(); calls
using ProgressCallback = std::function<void()>;

using PlaySoundCallback = std::function<void()>;