#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <String.h>
#include <List.h>

#define HIGHSCORES_PATH "/boot/home/config/settings/BeVexed.hiscores"

int LoadHighScores(void);
int SaveHighScores(void);

struct Score
{
	Score( BString& _name, int _time):
		name(_name), time(_time)
		{}
	BString name;
	int time;
};

extern BList highscores[5];  //contain pointer to Score


#endif // HIGHSCORES_H
