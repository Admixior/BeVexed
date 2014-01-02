#include "HighScoresWindow.h"
#include <View.h>
#include <Button.h>
#include <Messenger.h>
#include <StringView.h>
#include <TextView.h>
#include <Font.h>
#include <Alert.h>

HighScoresWindow::HighScoresWindow(Score* player, int place, int difficulty)
 : BWindow(BRect(100,100,500,600),"Highscores",B_TITLED_WINDOW_LOOK,
 	B_NORMAL_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE)
{BString abc;
abc<<place<<":"<<difficulty<<":"<<(int)player;
	BAlert* ale =new BAlert ("Highscores", abc.String(), "OK"); ale->Go();
#define MSGERR(x) ale =new BAlert ("Highscores", x, "OK"); ale->Go();
	MSGERR("aa");
	BFont* font = new BFont;
	MSGERR("createbview");
	BView *all = new BView(BRect(0,0,400,500),"highscores",B_FOLLOW_ALL,B_WILL_DRAW);
	MSGERR("getfont");
	all->GetFont(font);
	
	MSGERR("setface");
	font->SetFace(B_BOLD_FACE);
	MSGERR("Font");
	if(player)
	{
		MSGERR("Player");
		BView* vfs = new BView(BRect(10,10,490,30),"ViewForText1_hiscores", B_FOLLOW_ALL,B_WILL_DRAW);
		vfs->SetFontSize(17);
		BStringView* stars = new BStringView(BRect(0,0,490,20),
						"Text1_hiscores","*** You Won! ***",0, B_WILL_DRAW);
		stars->SetAlignment(B_ALIGN_CENTER);
		vfs->SetFont(font);
		vfs->AddChild(stars);
		
		BStringView* text= new BStringView(BRect(10,30,490,40),
						"Text2_hiscores","Please enter your name for the highscores:",0, B_WILL_DRAW);
		stars->SetAlignment(B_ALIGN_CENTER);
		
		BTextView* name = new BTextView(BRect(150,40,350,50),"YourName_hiscores", BRect(4,2,196,8), 0);
		
		all->AddChild(vfs);
		all->AddChild(text);
		all->AddChild(name);
	}
	
	if(difficulty < 5 && difficulty >= 0)
	{
		MSGERR("Creating");
		char *name[]={"beginner", "easy", "medium", "hard", "master"};
		
		BView* text1 = new BView(BRect(10,60,490,70), "Text3_hiscores", B_FOLLOW_ALL,B_WILL_DRAW);
		text1->SetFontSize(9);
		text1->SetFont(font);
		
		MSGERR("Creating");
		BString text1_h = "Highscores for ";
		text1_h+=name[difficulty];
		text1->AddChild(new BStringView(BRect(10,30,490,40),
						"Text4_hiscores",text1_h.String(),0, B_WILL_DRAW));
		all->AddChild(text1);
		
		MSGERR("Creating");
		BView* left_view = new BView(BRect(20, 90, 350, 400), "Leftview_hiscores", B_FOLLOW_ALL,B_WILL_DRAW);
		BString  start = "\xe2\x91\xa0  ";
		
		MSGERR("Creating");
		for(int i = 0; i < highscores[difficulty].CountItems(); ++i)
		{
			
		MSGERR("Creating Loop");
			Score* people = reinterpret_cast<Score*>(highscores[difficulty].ItemAt(i));
			BString text = start;
			text += people->name;
			start[3]++;
			BView* special = new BView(BRect(10, 10+i*10, 200, 20+i*10), 0, B_FOLLOW_ALL,B_WILL_DRAW);
			if(place==i)	special->SetFont(font);
			special->AddChild(new BStringView(BRect(0,0,200,10),0 ,text.String(),0, B_WILL_DRAW));
		}
	}
	
		MSGERR("Ending");
	AddChild(all);
	
		MSGERR("Ending");
	CenterOnScreen();
	MSGERR("Success");
}

void HighScoresWindow::MessageReceived(BMessage *msg)
{
	BWindow::MessageReceived(msg);
}

