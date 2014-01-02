#include "MainWindow.h"
#include <View.h>
#include <Menu.h>
#include <MenuItem.h>
#include <stdio.h>
#include <Alert.h>
#include <Bitmap.h>
#include <TranslationUtils.h>
#include <Roster.h>
#include <Path.h>
#include <Entry.h>
#include <Directory.h>
#include "AboutWindow.h"
#include "Preferences.h"
#include "TileView.h"
#include "Grid.h"
#include "ImageAlert.h"
#include "HighScores.h"
#include "HighScoresWindow.h"

enum
{
	M_NEW_GAME='nwgm',
	
	M_SMALL_TILES,
	M_MEDIUM_TILES,
	M_LARGE_TILES,
	M_HUGE_TILES,
	
	M_SET_BACKGROUND,
	
	M_SET_TILE_COUNT_3,
	M_SET_TILE_COUNT_4,
	M_SET_TILE_COUNT_5,
	M_SET_TILE_COUNT_6,
	M_SET_TILE_COUNT_7,
	
	M_HIGHSCORES_0,
	
	M_HIGHSCORES_1,
	M_HIGHSCORES_2,
	M_HIGHSCORES_3,
	M_HIGHSCORES_4,
	
	M_HOW_TO_PLAY
};

MainWindow::MainWindow(void)
 :	BWindow(BRect(100,100,500,400),"BeVexed",B_TITLED_WINDOW_LOOK,
 	B_NORMAL_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
 	fGrid(NULL),
 	fWorkGrid(NULL)
{
	app_info ai;
	be_app->GetAppInfo(&ai);
	BPath path(&ai.ref);
	path.GetParent(&path);
	path.Append("backgrounds");
	fBackPath = path.Path();
	fBackPath << "/";
	
	LoadHighScores();
	
	static const rgb_color beos_blue = {51,102,152,255};
	LoadPreferences(PREFERENCES_PATH);
	
	if(gPreferences.FindInt8("gridsize",(int8*)&fGridSize)!=B_OK)
	{
		fGridSize = 3;
		gPreferences.AddInt8("gridsize",3);
	}
	
	if(gPreferences.FindInt8("tilesize",(int8*)&fTileSize)!=B_OK)
	{
		fTileSize = TILESIZE_MEDIUM;
		gPreferences.AddInt8("tilesize",TILESIZE_MEDIUM);
	}
	
	fBack = new BView(Bounds(),"background",B_FOLLOW_ALL,B_WILL_DRAW);
	AddChild(fBack);
	fBack->SetViewColor(beos_blue);
	
	fMenuBar = new BMenuBar(BRect(0,0,Bounds().Width(),20),"menubar");
	fBack->AddChild(fMenuBar);
	
	BMenu *menu = new BMenu("Game");
	fMenuBar->AddItem(menu);
	
	menu->AddItem(new BMenuItem("New",new BMessage(M_NEW_GAME),'N'));
	
	BMenu *submenu = new BMenu("Difficulty");
	submenu->AddItem(new BMenuItem("Beginner",new BMessage(M_SET_TILE_COUNT_3)));
	submenu->AddItem(new BMenuItem("Easy",new BMessage(M_SET_TILE_COUNT_4)));
	submenu->AddItem(new BMenuItem("Medium",new BMessage(M_SET_TILE_COUNT_5)));
	submenu->AddItem(new BMenuItem("Hard",new BMessage(M_SET_TILE_COUNT_6)));
	submenu->AddItem(new BMenuItem("Master",new BMessage(M_SET_TILE_COUNT_7)));
	submenu->SetRadioMode(true);
	menu->AddSeparatorItem();
	menu->AddItem(submenu);
	
	BMenuItem *item = submenu->ItemAt(fGridSize - 3);
	item->SetMarked(true);
	
	submenu = new BMenu("Tile Size");
	submenu->AddItem(new BMenuItem("Small",new BMessage(M_SMALL_TILES)));
	submenu->AddItem(new BMenuItem("Medium",new BMessage(M_MEDIUM_TILES)));
	submenu->AddItem(new BMenuItem("Large",new BMessage(M_LARGE_TILES)));
	submenu->AddItem(new BMenuItem("Extra large",new BMessage(M_HUGE_TILES)));
	submenu->SetRadioMode(true);
	menu->AddItem(submenu);
	
	fBackMenu = new BMenu("Background");
	menu->AddItem(fBackMenu);
	ScanBackgrounds();
	
	switch(fTileSize)
	{
		case TILESIZE_SMALL:
		{
			submenu->ItemAt(0)->SetMarked(true);
			break;
		}
		case TILESIZE_MEDIUM:
		{
			submenu->ItemAt(1)->SetMarked(true);
			break;
		}
		case TILESIZE_LARGE:
		{
			submenu->ItemAt(2)->SetMarked(true);
			break;
		}
		case TILESIZE_HUGE:
		{
			submenu->ItemAt(3)->SetMarked(true);
			break;
		}
		default:
		{
			submenu->ItemAt(2)->SetMarked(true);
			break;
		}
	}
	
	submenu = new BMenu("Highscores");
	submenu->AddItem(new BMenuItem("Beginner",new BMessage(M_HIGHSCORES_0)));
	submenu->AddItem(new BMenuItem("Easy",new BMessage(M_HIGHSCORES_1)));
	submenu->AddItem(new BMenuItem("Medium",new BMessage(M_HIGHSCORES_2)));
	submenu->AddItem(new BMenuItem("Hard",new BMessage(M_HIGHSCORES_3)));
	submenu->AddItem(new BMenuItem("Master",new BMessage(M_HIGHSCORES_4)));
	submenu->SetRadioMode(true);
	menu->AddSeparatorItem();
	menu->AddItem(submenu);
	
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("How to play…",new BMessage(M_HOW_TO_PLAY)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("About BeVexed…",new BMessage(B_ABOUT_REQUESTED)));
	GenerateGrid(fGridSize);

	BPoint corner;
	if(gPreferences.FindPoint("corner",&corner)!=B_OK)
	{
		corner = Frame().LeftTop();
		gPreferences.AddPoint("corner",Frame().LeftTop());
	}
	
	BRect r(Frame());
	r.OffsetTo(corner.x,corner.y);
	ConstrainWindowFrameToScreen(&r);
	MoveTo(r.left,r.top);

	if(gPreferences.FindString("background",&fBackName) == B_OK)
		SetBackground(fBackName.String());
}

bool MainWindow::QuitRequested(void)
{
	gPreferences.ReplacePoint("corner",Frame().LeftTop());
	SavePreferences(PREFERENCES_PATH);
	SaveHighScores();
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void MainWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case B_ABOUT_REQUESTED:
		{
			AboutWindow *ab = new AboutWindow();
			ab->Show();
			break;
		}
		case M_NEW_GAME:
		{
			GenerateGrid(fGridSize);
			break;
		}
		case M_SMALL_TILES:
		{
			fTileSize = TILESIZE_SMALL;
			gPreferences.ReplaceInt8("tilesize",TILESIZE_SMALL);
			TileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize);
			break;
		}
		case M_MEDIUM_TILES:
		{
			fTileSize = TILESIZE_MEDIUM;
			gPreferences.ReplaceInt8("tilesize",TILESIZE_MEDIUM);
			TileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize);
			break;
		}
		case M_LARGE_TILES:
		{
			fTileSize = TILESIZE_LARGE;
			gPreferences.ReplaceInt8("tilesize",TILESIZE_LARGE);
			TileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize);
			break;
		}
		case M_HUGE_TILES:
		{
			fTileSize = TILESIZE_HUGE;
			gPreferences.ReplaceInt8("tilesize",TILESIZE_LARGE);
			TileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize);
			break;
		}
		case M_SET_TILE_COUNT_3:
		{
			fGridSize = 3;
			gPreferences.ReplaceInt8("gridsize",3);
			GenerateGrid(fGridSize);
			break;
		}
		case M_SET_TILE_COUNT_4:
		{
			fGridSize = 4;
			gPreferences.ReplaceInt8("gridsize",4);
			GenerateGrid(fGridSize);
			break;
		}
		case M_SET_TILE_COUNT_5:
		{
			fGridSize = 5;
			gPreferences.ReplaceInt8("gridsize",5);
			GenerateGrid(fGridSize);
			break;
		}
		case M_SET_TILE_COUNT_6:
		{
			fGridSize = 6;
			gPreferences.ReplaceInt8("gridsize",6);
			GenerateGrid(fGridSize);
			break;
		}
		case M_SET_TILE_COUNT_7:
		{
			fGridSize = 7;
			gPreferences.ReplaceInt8("gridsize",7);
			GenerateGrid(fGridSize);
			break;
		}
		case M_SET_BACKGROUND:
		{
			BString name;
			if (msg->FindString("name",&name) == B_OK)
				SetBackground(name.String());
			else
				SetBackground(NULL);
			break;
		}
		case M_HIGHSCORES_0:
		case M_HIGHSCORES_1:
		case M_HIGHSCORES_2:
		case M_HIGHSCORES_3:
		case M_HIGHSCORES_4:
		{
			HighScoresWindow* window = new HighScoresWindow( 0, -1, msg->what - M_HIGHSCORES_0);
			window->Run();
			delete window;
		}
		case M_HOW_TO_PLAY:
		{
			BString string =
				"How to Play BeVexed:\n\n"
				"Place the tiles in the grid on the right into the "
				"grid on the left so that the numbers match wherever "
				"one tile touches another. Easy to learn, tough to master.\n";
			BAlert *help = new BAlert("BeVexed",string.String(),"OK");
			help->Go();
			break;
		}
		case M_CHECK_DROP:
		{
			TileView *from, *to;
			if(msg->FindPointer("from",(void**)&from)!=B_OK ||
					msg->FindPointer("to",(void**)&to)!=B_OK)
				break;
					
			// If we're dropping to the storage grid
			if(fGrid->HasTile(to->GetTile()))
			{
				*to->GetTile() = *from->GetTile();
				from->GetTile()->MakeEmpty();
				to->Invalidate();
				from->Invalidate();
			}
			else
			// If we're dropping to the tile grid
			if(fWorkGrid->HasTile(to->GetTile()))
			{
				if(fWorkGrid->TryTile(from->GetTile(),to->GetTile()))
				{
					*to->GetTile() = *from->GetTile();
					from->GetTile()->MakeEmpty();
					to->Invalidate();
					from->Invalidate();
					
					if(fWorkGrid->IsSolved())
					{
						ImageAlert *alert = new ImageAlert("BeVexedYouWin.jpg",'JPEG');
						alert->Show();
						GenerateGrid(fGridSize);
					}
				}
			}
			else
				debugger("Programmer Error: Orphaned Tile");
						
			break;
		}
		default:
			BWindow::MessageReceived(msg);
	}
}

void MainWindow::GenerateGrid(uint8 size)
{
	if(fGrid)
	{
		delete fGrid;
		delete fWorkGrid;
	}
	
	fBack->RemoveChild(fMenuBar);
	while(fBack->CountChildren()>0)
	{
		BView *child = fBack->ChildAt(0);
		child->RemoveSelf();
		delete child;
	}
	fBack->AddChild(fMenuBar);
	
	fGrid = new Grid(size);
	fGrid->GeneratePuzzle();
	fWorkGrid = new Grid(size);
	
	ResizeTo( ((fTileSize+5) * size * 2) + 70,
				((fTileSize+5) * size) + fMenuBar->Frame().Height() + 20 );
	
	BRect r(10,fMenuBar->Frame().bottom + 10,
			10 + fTileSize,10 + fMenuBar->Frame().bottom + fTileSize);
	
	for(uint8 row=0; row<size; row++)
	{
		for(uint8 col=0; col<size; col++)
		{
			TileView *tile = new TileView(r.LeftTop(),fTileSize,"tile",
											B_FOLLOW_NONE,B_WILL_DRAW);
			fBack->AddChild(tile);
			r.OffsetBy(fTileSize + 5,0);
			
			tile->SetTile(fWorkGrid->TileAt((size*row) + col));
		}
		
		r.OffsetBy(50,0);
		for(uint8 col=0; col<size; col++)
		{
			TileView *tile = new TileView(r.LeftTop(),fTileSize,"tile",
											B_FOLLOW_NONE,B_WILL_DRAW);
			fBack->AddChild(tile);
			r.OffsetBy(fTileSize + 5,0);
			
			tile->SetTile(fGrid->TileAt((size*row) + col));
		}
		
		r.OffsetBy(-(r.left - 10),fTileSize+5);
	}
}

void MainWindow::ScanBackgrounds(void)
{
	while (fBackMenu->CountItems() > 0)
	{
		BMenuItem *item = fBackMenu->ItemAt(0L);
		delete item;
	}
	
	BMessage *msg = new BMessage(M_SET_BACKGROUND);
	msg->AddString("name","");
	fBackMenu->AddItem(new BMenuItem("None",msg));
	fBackMenu->AddSeparatorItem();
	
	BDirectory dir(fBackPath.String());
	dir.Rewind();
	entry_ref ref;
	while (dir.GetNextRef(&ref) == B_OK)
	{
		BPath path(&ref);
		BString name(path.Leaf());
		int32 ext = name.FindLast(".");
		if (ext > 0)
			name.Truncate(ext);
		
		msg = new BMessage(M_SET_BACKGROUND);
		msg->AddString("name",path.Leaf());
		fBackMenu->AddItem(new BMenuItem(name.String(),msg));
	}
}

void MainWindow::SetBackground(const char *name)
{
	if (!name || strlen(name) == 0)
	{
		fBack->ClearViewBitmap();
		fBack->Invalidate();
		fBackName = "";
		gPreferences.RemoveData("background");
		return;
	}
	
	BString path(fBackPath);
	path << name;
	BBitmap *bmp = BTranslationUtils::GetBitmapFile(path.String());
	if (bmp)
	{
		fBack->SetViewBitmap(bmp);
		delete bmp;
		fBackName = name;
		gPreferences.RemoveData("background");
		gPreferences.AddString("background",fBackName);
		fBack->Invalidate();
	}
}

