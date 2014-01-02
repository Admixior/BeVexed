#ifndef HIGHSCORESWINDOW_H
#define HIGHSCORESWINDOW_H

#include <Window.h>
#include "HighScores.h"



class HighScoresWindow : public BWindow
{
public:
	HighScoresWindow(Score* player, int place , int difficulty);
	void MessageReceived(BMessage *msg);
};

#endif
