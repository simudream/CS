// awsMenu.cpp: implementation of the awsMenu class.
//
//////////////////////////////////////////////////////////////////////
#include "cssysdef.h"
#include "awsMenu.h"
#include "csutil/util.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/fontserv.h"
#include "iutil/event.h"
#include "aws/awslayot.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int awsMenuEntry::signalClicked = 1;
const int awsMenuEntry::signalSelected = 2;

awsMenuEntry::awsMenuEntry() :
  caption(NULL),
  popup(NULL),
  selected(false),
  mouse_down(false),
  mouse_over(false),
  user_param(NULL),
  image(NULL),
  image_width(0),
  image_height(0),
  sub_menu_image(NULL),
  sub_menu_image_width(0),
  sub_menu_image_height(0)
{}

awsMenuEntry::~awsMenuEntry() 
{
  if(caption) caption->DecRef();
  if(popup) popup->DecRef();
  if(image) image->DecRef();
  if(sub_menu_image) sub_menu_image->DecRef();
}

bool awsMenuEntry::Setup (iAws *_wmgr, awsComponentNode *settings)
{
  if (!awsPanel::Setup (_wmgr, settings)) return false;

  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();

  pm->GetString (settings, "Caption", caption);
  
  iString* image_name = NULL;
  pm->GetString (settings, "Image", image_name);
  if(image_name)
    image = pm->GetTexture(image_name->GetData(), image_name->GetData());


  pm->LookupIntKey("MenuItemImageWidth", image_width);
  pm->GetInt(settings, "ImageWidth", image_width);

  pm->LookupIntKey("MenuItemImageHeigth", image_width);
  pm->GetInt(settings, "ImageHeight", image_width);

  image_name = NULL;
  pm->LookupStringKey("MenuItemSubMenuImage", image_name);
  if(image_name)
  {
    sub_menu_image = pm->GetTexture(image_name->GetData(), image_name->GetData());
    if(sub_menu_image)
    {
      sub_menu_image->GetOriginalDimensions(sub_menu_image_width,
                                            sub_menu_image_height);
    }
  }

  SizeToFit();

  return true;
}

const char* awsMenuEntry::Type() { return "Menu Entry"; }

bool awsMenuEntry::GetProperty(const char* name, void **parm)
{
  if (awsPanel::GetProperty (name, parm)) return true;
	
  if (strcmp ("Caption", name) == 0)
  {
    char *st = NULL;
    
    if (caption) st = caption->GetData ();
    
    iString *s = new scfString (st);
    *parm = (void *)s;
    return true;
  }
  else if(strcmp(name, "PopupMenu") == 0)
  {
    *parm = (void *)popup;
    return true;
  }
  else if(strcmp(name, "Selected") == 0)
  {
    *parm = (void*)selected;
    return true;
  }
  else if(strcmp(name, "UserParam") == 0)
  {
    *parm = user_param;
    return true;
  }
  else if(strcmp(name, "CloseSignal") == 0)
  {
    *parm = (void *)signalClicked;
    return true;
  }
  else if(strcmp(name, "SelectSignal") == 0)
  {
    *parm = (void *)signalSelected;
    return true;
  }
  else if(strcmp(name, "Image") == 0)
  {
    *parm = (void*) image;
    return true;
  }
	
  return false;
}

bool awsMenuEntry::SetProperty(const char *name, void *parm)
{
  if (awsPanel::SetProperty (name, parm)) return true;
	
  if (strcmp ("Caption", name) == 0)
  {
    iString *s = (iString *) (parm);
    
    if (s && s->Length ())
    {
      if (caption) caption->DecRef ();
      caption = s;
      caption->IncRef (); 
      SizeToFit();
      Invalidate ();
    }
    else
    {
      if (caption) caption->DecRef ();
      caption = NULL;
    }
    
    return true;
  }
  if(strcmp(name, "PopupMenu") == 0)
  {
    awsPopupMenu *pm = (awsPopupMenu *) (parm);
    
    if (popup) popup->DecRef ();
    popup = pm;
    if (popup) popup->IncRef ();
    SizeToFit();
    Invalidate ();
    return true;
  }
  else if(strcmp(name, "Selected") == 0)
  {
    selected = (bool)parm;
    return true;
  }
  else if(strcmp(name, "UserParam") == 0)
  {
    user_param = parm;
    return true;
  }
  else if(strcmp(name, "Image") == 0)
  {
    iTextureHandle *im = (iTextureHandle*) (parm);
    
    if (image) image->DecRef ();
    image = im;
    if (image) image->IncRef ();
    Invalidate ();
  }
  
  return false;
}

void awsMenuEntry::SizeToFit()
{
  int tw, th;
  WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->GetDimensions ( 
      caption->GetData(), tw, th);
 
  th = MAX(image_height, th);

  if(popup && sub_menu_image)
    tw += sub_menu_image_width;

  csRect r = getInsets();
  Resize(tw + r.xmin + r.xmax + image_width + 10, th + r.ymin + r.ymax + 10);
}

void awsMenuEntry::OnDraw (csRect clip)
{
  iGraphics2D *g2d = WindowManager ()->G2D ();
  iGraphics3D *g3d = WindowManager ()->G3D ();

  int selectTextColor = WindowManager ()->GetPrefMgr ()->GetColor(AC_SELECTTEXTFORE);
  int selectBackColor = WindowManager ()->GetPrefMgr ()->GetColor(AC_SELECTTEXTBACK);
  int textColor =       WindowManager ()->GetPrefMgr ()->GetColor(AC_TEXTFORE);
  int fill  =       WindowManager ()->GetPrefMgr ()->GetColor(AC_FILL);

  if(selected)
    frame_drawer.SetBackgroundColor(selectBackColor);
  else
    frame_drawer.SetBackgroundColor(fill);

  awsPanel::OnDraw(clip);


  // Draw the caption, if there is one
  if (caption)
  {
    int tw, th, ty, mcc;

    mcc = WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->GetLength (
        caption->GetData (),
        Frame ().Width () - image_width 
        - (popup && sub_menu_image ? sub_menu_image_width : 0));

    scfString tmp (caption->GetData ());
    tmp.Truncate (mcc);

    // Get the size of the text
    WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->GetDimensions (
        tmp.GetData (),
        tw,
        th);

    // Calculate the center
    ty = (Frame ().Height () >> 1) - (th >> 1);

    int color = selected ? selectTextColor : textColor;

    // Draw the text
    g2d->Write (
        WindowManager ()->GetPrefMgr ()->GetDefaultFont (),
        ClientFrame ().xmin + image_width,
        ClientFrame ().ymin + ty,
        color,
        -1,
        tmp.GetData ());
  }

  if(image)
  {
    csRect r = Frame();
    r.xmax = r.xmin + image_width;
    
    int tw, th;
    image->GetOriginalDimensions(tw, th);

    g3d->DrawPixmap (image,
                     r.xmin,
                     r.ymin,
                     MIN(tw,r.Width()),
                     MIN(th,r.Height()),
                     0,
                     0,
                     MIN(tw,r.Width()),
                     MIN(th,r.Height()));
  }

  if(popup && sub_menu_image)
  {
    csRect r = Frame();
    r.xmin = r.xmax - sub_menu_image_width;
    
    int tw, th;
    image->GetOriginalDimensions(tw, th);

    g3d->DrawPixmap (sub_menu_image,
                     r.xmin,
                     r.ymin,
                      MIN(tw,r.Width()),
                     MIN(th,r.Height()),
                     0,
                     0,
                     MIN(tw,r.Width()),
                     MIN(th,r.Height()));
  }
}


bool awsMenuEntry::OnMouseDown(int button, int x, int y)
{
  mouse_down = true;
  awsPanel::OnMouseDown(button, x, y);
  return true;
}

bool awsMenuEntry::OnMouseUp(int button, int x, int y)
{
  if(mouse_down)
  {
    Broadcast(signalClicked);
    mouse_down = false;
    awsPanel::OnMouseUp(button, x, y);
    return true;
  }
  return awsPanel::OnMouseUp(button, x, y);
}


bool awsMenuEntry::OnMouseEnter()
{
  mouse_over = true;
  selected = true;
  Broadcast(signalSelected);
  return true;
}

bool awsMenuEntry::OnMouseExit()
{
  mouse_down = false;
  mouse_over = false;
  awsPanel::OnMouseExit();
  return true;
}

/********************************** awsMenuEntryFactory ************************/

awsMenuEntryFactory::awsMenuEntryFactory(iAws* wmgr) : awsComponentFactory(wmgr)
{
  Register("Menu Entry");
}

awsMenuEntryFactory::~awsMenuEntryFactory()
{}

iAwsComponent* awsMenuEntryFactory::Create()
{
  return (iAwsComponent*) new awsMenuEntry;
}

/********************************* awsMenuBarEnty ******************************/


awsMenuBarEntry::awsMenuBarEntry() :
popup(NULL)
{
style = fsToolbar;
is_switch = true;
}

awsMenuBarEntry::~awsMenuBarEntry() 
{
  if(popup) popup->DecRef();
}

bool awsMenuBarEntry::Setup (iAws *_wmgr, awsComponentNode *settings)
{
  if (!awsCmdButton::Setup (_wmgr, settings)) return false;

  ResizeTo(getPreferredSize());

  return true;
}

const char* awsMenuBarEntry::Type() { return "Menu Bar Entry"; }

bool awsMenuBarEntry::GetProperty(const char* name, void **parm)
{
	if (awsCmdButton::GetProperty (name, parm)) return true;
	
	if (strcmp ("Caption", name) == 0)
	{
		char *st = NULL;
		
		if (caption) st = caption->GetData ();
		
		iString *s = new scfString (st);
		*parm = (void *)s;
		return true;
	}
	else if(strcmp(name, "PopupMenu") == 0)
	{
        *parm = (void *)popup;
		return true;
	}
	else if(strcmp(name, "Selected") == 0)
	{
		// cmd buttons use state
    return GetProperty("State", parm);
	}
  else if(strcmp(name, "SelectSignal") == 0)
  {
    *parm = (void *)signalClicked;
    return true;
  }
	
	return false;
}

bool awsMenuBarEntry::SetProperty(const char *name, void *parm)
{
	if (awsCmdButton::SetProperty (name, parm)) return true;
	
	if(strcmp(name, "PopupMenu") == 0)
	{
		awsPopupMenu *pm = (awsPopupMenu *) (parm);
		
		if (popup) popup->DecRef ();
		popup = pm;
		if (popup) popup->IncRef ();
		Invalidate ();
        return true;
	}
	else if(strcmp(name, "Selected") == 0)
	{
    // cmd buttons use state
		return SetProperty("State", parm);
	}
	
	return false;
}

bool awsMenuBarEntry::OnMouseEnter()
{
  iAwsComponent* cmp = NULL;
  Parent()->GetProperty("Selected", (void**) &cmp);
  if(cmp)
  {
    SetProperty("Selected", (void**)true);

    // our selection signal
    Broadcast(signalClicked);
  }

  return awsCmdButton::OnMouseEnter();
}


/********************************** awsMenuBarEntryFactory ************************/

awsMenuBarEntryFactory::awsMenuBarEntryFactory(iAws* wmgr) : awsComponentFactory(wmgr)
{
  Register("Menu Bar Entry");
}

awsMenuBarEntryFactory::~awsMenuBarEntryFactory()
{}

iAwsComponent* awsMenuBarEntryFactory::Create()
{
  return (iAwsComponent*) new awsMenuBarEntry;
}


/********************************* awsMenu  ************************************/

awsMenu::awsMenu() 
  : awsControlBar(),
    select(NULL),
    popup_showing(NULL),
    child_menu(NULL),
    parent_menu(NULL),
    mouse_pos(0,0),
    sink(this),
    let_mouse_exit(true)
{
}

awsMenu::~awsMenu()
{
  if(child_menu)
  {
    child_menu->Hide();
    child_menu->DecRef();
  }
}

bool awsMenu::Setup(iAws *wmgr, awsComponentNode *settings)
{
  sink.RegisterTrigger("Select", &OnSelect);
  sink.RegisterTrigger("Close", &OnClose);
  if(!awsControlBar::Setup(wmgr, settings)) return false;

  // we initially fit it to size
  // but after that you can change it to whatever you like
  SizeToFitHorz();
  
  return true;
}

bool awsMenu::GetProperty(const char* name, void** parm)
{
  if(awsControlBar::GetProperty(name, parm)) return true;
  else if(strcmp(name, "Selected") == 0)
  {
    *parm = (void*) select;
    return true;
  }

  return false;
}


void awsMenu::SetMenuParent(awsMenu *_parent_menu)
{
  parent_menu = _parent_menu;
}

void awsMenu::AddChild(iAwsComponent* comp)
{
  int selectSignal, closeSignal;
  if(comp->GetProperty("SelectSignal", (void**) &selectSignal))
    slot_select.Connect(comp, selectSignal, &sink, sink.GetTriggerID("Select"));
  if(comp->GetProperty("CloseSignal", (void**) &closeSignal))
    slot_close.Connect(comp, closeSignal, &sink, sink.GetTriggerID("Close"));

  awsControlBar::AddChild(comp);
}

iAwsSource* awsMenu::AddChild(const char* caption, iTextureHandle* image, awsPopupMenu* popup)
{
  iAwsComponent* child = GetNewDefaultEntry();
  // XXX: memory leak here, should delete the string later
  child->SetProperty("Caption", csStrNew(caption));
  child->SetProperty("Image", image);
  child->SetProperty("PopupMenu", popup);

  // create will link the component to us
  child->Create(WindowManager(), this, NULL);
  child->DecRef();
  return (iAwsSource*) child;
}

void awsMenu::RemoveChild(iAwsComponent* comp)
{
  int selectSignal, closeSignal;
  if(comp->GetProperty("SelectionSignal", (void**) &selectSignal))
    slot_select.Disconnect(comp, selectSignal, &sink, sink.GetTriggerID("Select"));
  if(comp->GetProperty("CloseSignal", (void**) &closeSignal))
    slot_close.Disconnect(comp, closeSignal, &sink, sink.GetTriggerID("Close"));

  awsControlBar::RemoveChild(comp);
}

void awsMenu::RemoveChild(const char* caption)
{
  for(iAwsComponent* cmp = GetTopChild(); cmp; cmp = cmp->ComponentBelow())
  {
    iString* temp_caption;
    if(!cmp->GetProperty("Caption", (void**)&temp_caption)) continue;
    if(!caption)
    {
      if(!temp_caption)
      {
        RemoveChild(cmp);
        break;
      }
      else continue;
    }
    if(!temp_caption) continue;
    if(strcmp(temp_caption->GetData(), caption) == 0)
    {
      temp_caption->DecRef();
      RemoveChild(cmp);
      break;
    }
    else
      temp_caption->DecRef();
  }
}

void awsMenu::RemoveChild(iAwsSource* src)
{
  RemoveChild(src->GetComponent());
}
    

bool awsMenu::HandleEvent(iEvent &Event)
{
  switch(Event.Type)
  {
    case csevMouseMove:
    case csevMouseUp:
	  case csevMouseClick:
    case csevMouseDown:
    case csevMouseEnter:
    case csevMouseExit:
        mouse_pos.Set(Event.Mouse.x, Event.Mouse.y);
        break;
  }
  return awsControlBar::HandleEvent(Event);
}

bool awsMenu::IsOverChildMenu(int x, int y)
{
  return (child_menu && ( child_menu->Frame().Contains(x,y) ||
                           child_menu->IsOverChildMenu(x,y)    ));
}

bool awsMenu::IsOverParentMenu(int x, int y)
{
  return (parent_menu && (parent_menu->Frame().Contains(x,y) ||
                           parent_menu->IsOverParentMenu(x,y)   ));
}

bool awsMenu::OnMouseMove(int button, int x, int y)
{
  if(IsOverChildMenu(x,y) || IsOverParentMenu(x,y)) 
  {
    if(mouse_captured)
    {
      WindowManager()->ReleaseMouse();
      mouse_captured = false;
    }
  }

  else if(Frame().Contains(x,y))
  {
    if(mouse_captured)
    {
      WindowManager()->ReleaseMouse();
      mouse_captured = false;
    }
  }

  return awsControlBar::OnMouseMove(button, x, y);
}


bool awsMenu::OnMouseExit()
{
  // if the mouse attempts to leave the border of the popup we capture it
  // unless it is entering another popup or one of our children
  // or if let_mouse_exit is set
  if(!IsOverChildMenu(mouse_pos.x, mouse_pos.y) && 
     !IsOverParentMenu(mouse_pos.x, mouse_pos.y) &&
     !Frame().Contains(mouse_pos.x, mouse_pos.y) &&
     ShouldTrackMouse()                                    )
  {
    WindowManager()->CaptureMouse(this);
    mouse_captured = true;
  }

  let_mouse_exit = false;

  awsControlBar::OnMouseExit();

  return true;
}

bool awsMenu::OnMouseDown(int button, int x, int y)
{
  if(mouse_captured)
  {
    let_mouse_exit = true;
    WindowManager()->ReleaseMouse();
    mouse_captured = false;
  }

  // if the user clicks elsewhere then dismiss the menus
  if(!IsOverChildMenu(x, y) && 
     !IsOverParentMenu(x, y) &&
     !Frame().Contains(x, y)     )
  {
    HideAllPopups();
  }

  return awsControlBar::OnMouseDown(button, x, y);
}

bool awsMenu::ShouldTrackMouse()
{
  return !let_mouse_exit;
}

void awsMenu::TrackMouse()
{
  WindowManager()->CaptureMouse(this);
  mouse_captured = true;
}

void awsMenu::Select(iAwsComponent* child)
{
  // eliminate spurious calls
  if(child == select) return;

  // first we deactivate the old active child
  if(select)
    select->SetProperty("Selected", (void*)false);

  select = child;

  if(select)
    select->SetProperty("Selected", (void*)true);

  StartPopupChange();
}

void awsMenu::OnSelect(void* p, iAwsSource* src)
{
  awsMenu* m = (awsMenu*)p;
  iAwsComponent* menu_entry = src->GetComponent();
  bool selected = false;
  menu_entry->GetProperty("Selected", (void**) &selected);
  // an inactive entry is telling us its inactive
  if(!selected && menu_entry != m->select) return;

  if(selected)  // make this entry selected
    m->Select(src->GetComponent());
  else  // make the selected entry unselected
    m->Select(NULL);
}

void awsMenu::OnClose(void* p, iAwsSource* )
{
  awsMenu* m = (awsMenu*)p;
  m->HideAllPopups();
}

void awsMenu::Hide()
{
  if(child_menu)
  {
    child_menu->Hide();
    child_menu = NULL;
    popup_showing = NULL;
  }

  Select(NULL);
  awsControlBar::Hide();
}

void awsMenu::SwitchPopups()
{

  // if the popup doesn't need to change
  if(popup_showing == select)
    return;

  // take down the old pop-up
  if(child_menu)
  {
    child_menu->Hide();
    child_menu = NULL;
  }

  popup_showing = NULL;

  // get the new popup if any

  if(select)
    select->GetProperty("PopupMenu", (void**) &child_menu);

  // display the new popup if necessary
  if(child_menu)
  {
    popup_showing = select;
    child_menu->SetMenuParent(this);

    PositionPopupMenu(popup_showing, child_menu);
    child_menu->Raise();
    child_menu->Show();
  }
}

/********************************* awsMenuBar    *******************************/


awsMenuBar::awsMenuBar()
{
  style = fsFlat;
  SetVertical(false);
  SetVertGap(2);
  SetSizeToFitHorz(false);
}

awsMenuBar::~awsMenuBar()
{
  this->awsMenu::~awsMenu();
}

bool awsMenuBar::Setup(iAws *wmgr, awsComponentNode *settings)
{
  if(!awsMenu::Setup(wmgr, settings)) return false;
  
  return true;
}

const char* awsMenuBar::Type() 
{
  return "Menu Bar";
}

void awsMenuBar::PositionPopupMenu(iAwsComponent* entry, awsMenu* menu)
{
  menu->MoveTo(entry->Frame().xmin, Frame().ymax);
}

void awsMenuBar::StartPopupChange()
{
  SwitchPopups();
}

void awsMenuBar::HideAllPopups()
{
  Select(NULL);
}

bool awsMenuBar::ShouldTrackMouse()
{
  if(popup_showing)
    return awsMenu::ShouldTrackMouse();
  else
    return false;
}

bool awsMenuBar::Create(iAws* wmgr, iAwsComponent *parent, awsComponentNode *settings)
{
  SetID(settings->Name());
  SetParent(parent);
  
  if(!Setup(wmgr, settings)) return false;

  if(Parent())
  {
    if(!Parent()->SetProperty("Menu", this))
    {
      Parent()->AddChild(this);
      
      // unless you have set the non client flag by this point 
      // you get added to the parent's layout
      if(~Flags() & AWSF_CMP_NON_CLIENT && Parent()->Layout())
        Parent()->Layout()->AddComponent(this, settings);
    }
  }
  else
  {
    // Link into the current hierarchy, at the top.
    if (wmgr->GetTopComponent () == NULL)
    {
      wmgr->SetTopComponent (this);
    }
    else
    {
      LinkAbove (wmgr->GetTopComponent ());
      wmgr->SetTopComponent (this);
    }
  }

  return true;
}

iAwsComponent* awsMenuBar::GetNewDefaultEntry()
{
  return new awsMenuBarEntry();
}

/******************************** MenuBarFactory ***********************************/

awsMenuBarFactory::awsMenuBarFactory(iAws *wmgr) :
awsComponentFactory(wmgr)
{
  Register("Menu Bar");
}

awsMenuBarFactory::~awsMenuBarFactory()
{}

iAwsComponent* awsMenuBarFactory::Create()
{
  return (iAwsComponent*) new awsMenuBar;
}


/********************************** awsPopupMenu *******************************/


awsPopupMenu::awsPopupMenu() :
timer(NULL)
{
  SetStretchComponents(true);
  style = fsRaised;
  SetVertical(true);
  SetFlag(AWSF_CMP_HIDDEN);
}

awsPopupMenu::~awsPopupMenu()
{
  this->awsMenu::~awsMenu();

  if(timer) delete timer;
}


bool awsPopupMenu::Create(iAws* wmgr, iAwsComponent *parent, awsComponentNode *settings)
{
  SetID(settings->Name());
  SetParent(NULL);
  
  if(!Setup(wmgr, settings)) return false;

  if(parent)
    return parent->SetProperty("PopupMenu", this);
  else
  {
    // Link into the current hierarchy, at the top.
    if (wmgr->GetTopComponent () == NULL)
    {
      wmgr->SetTopComponent (this);
    }
    else
    {
      LinkAbove (wmgr->GetTopComponent ());
      wmgr->SetTopComponent (this);
    }
  }

  return true;
}

bool awsPopupMenu::Setup(iAws *wmgr, awsComponentNode *settings)
{
  timer = new awsTimer(wmgr->GetObjectRegistry(), this);
  
  sink.RegisterTrigger("Timer", &OnTimer);
  slot_timer.Connect(timer, awsTimer::signalTick, &sink, sink.GetTriggerID("Timer"));

  if(!awsMenu::Setup(wmgr, settings)) return false;
  
  return true;
}

const char* awsPopupMenu::Type() 
{
  return "Menu Bar";
}

void awsPopupMenu::PositionPopupMenu(iAwsComponent* entry, awsMenu* menu)
{
  menu->MoveTo(Frame().xmax, entry->Frame().ymin);
}

void awsPopupMenu::StartPopupChange()
{
  timer->SetTimer(500);
  timer->Start();
  
}

void awsPopupMenu::HideAllPopups()
{
  if(parent_menu)
    parent_menu->HideAllPopups();
  else
    Hide();
}

void awsPopupMenu::OnTimer(void* param, iAwsSource* )
{
  awsPopupMenu* pm = (awsPopupMenu*)param;
  pm->SwitchPopups();
}

void awsPopupMenu::SwitchPopups()
{
  timer->Stop();
  awsMenu::SwitchPopups();
}

bool awsPopupMenu::OnMouseExit()
{
  Select(popup_showing);
  return awsMenu::OnMouseExit();
}

iAwsComponent* awsPopupMenu::GetNewDefaultEntry()
{
  return new awsMenuEntry();
}

/******************************** PopupMenuFactory ***********************************/

awsPopupMenuFactory::awsPopupMenuFactory(iAws *wmgr) :
awsComponentFactory(wmgr)
{
  Register("Popup Menu");
}

awsPopupMenuFactory::~awsPopupMenuFactory()
{}

iAwsComponent* awsPopupMenuFactory::Create()
{
  return (iAwsComponent*) new awsPopupMenu;
}

