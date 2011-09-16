
#ifndef __MSCMAINMENU_H_
#define __MSCMAINMENU_H_

class MscMainMenu
{
public:
	MscMainMenu();
	~MscMainMenu();

	void showMainMenu();

		// OIS::KeyListener
	bool keyPressed( const OIS::KeyEvent &arg );
	bool keyReleased( const OIS::KeyEvent &arg );
	// OIS::MouseListener
	bool mouseMoved( const OIS::MouseEvent &arg );
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
};

#endif // __MSCMAINMENU_H_
