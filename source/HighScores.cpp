#include "HighScores.h"
#include <cstdlib>
#include <File.h>
#include <List.h>
#include <Alert.h>

BList highscores[5];

int compare_int(const void* one,const void* two)
{
	return *(int*)two - *(int*)one;	
}

void Trim(BString& str)
{
	//str.Trim(); //TODO: check if it work
	return;
}

int GetLine(int start, int end, char* buff, BString& line)
{
	int i = start;
	for(; i<end && buff[i]!='\n'; ++i);
	if(i>start)line = BString( buff+start, i-start );
	else line = "";
	return i+1;
}

bool SeparateLine(BString& str, BString& name, int& score)
{
	if(str.Length()==0) return false;
	int sep = str.FindLast(':');
	if(sep == B_ERROR || sep == (str.Length()-1) || sep == 0) return false;
	
	//get score
	if( str.Length()-sep-5 ) return false;
	score = *(int*)(str.String()+sep+1);
	
	//get name
	str.CopyInto( name, 0, sep-1);
	
	return true;
}

int LoadHighScores()
{
	int start = 0, list_c=0;
	BString line;
	BFile* file = new BFile(HIGHSCORES_PATH, B_READ_ONLY);
	status_t status = file->InitCheck();
	if(status!=B_OK)
	{
		return status;
	}
	
	off_t size=0;
	file->GetSize(&size);
	if(size>1024*1024 && size)	//something goes wrong
		return -1;

	char *buff = new char[size];
	file->Read(buff, size);
	delete file;
	
	while(start!=size)
	{
		start = GetLine( start, size, buff, line );
		Trim( line );
		if( line.Length() == 0 ) continue;
		if( line[0] != '+' ) //new person
		{
			BString name;
			int score;
			if( !SeparateLine( line, name, score ) ) continue;
			highscores[list_c].AddItem(new Score(name, score));
		}
		else
		{
			//sort
			if( highscores[list_c].CountItems() )
				highscores[list_c].SortItems(&compare_int);
			if(++list_c > 4) //whoops
				break;
		}
	}
	delete[] buff;
	return 0;
}


int SaveHighScores()
{
	BFile file(HIGHSCORES_PATH, B_READ_WRITE | B_ERASE_FILE | B_CREATE_FILE);
	if( file.InitCheck() != B_OK ) return file.InitCheck();
	
	for(int list_c = 0; list_c < 5; ++list_c)
	{
		for(int i = 0; i < highscores[list_c].CountItems(); ++i)
		{
			Score* person = (Score*)highscores[list_c].ItemAt(i);
			file.Write( person->name.String(), person->name.Length() );
			file.Write( ":", 1);
			
			file.Write( (void*)person->time, (int)sizeof(person->time) );
			file.Write( "\n", 1 );
		}
		file.Write("+\n", 2);
	}
}
