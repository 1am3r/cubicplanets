
#ifndef __INPUTRECEIVER_H_
#define __INPUTRECEIVER_H_

class InputReceiver
{
public:
	InputReceiver();
	~InputReceiver();

	// OIS::KeyListener
	virtual bool keyPressed(const OIS::KeyEvent &arg) = 0;
	virtual bool keyReleased(const OIS::KeyEvent &arg) = 0;
	// OIS::MouseListener
	virtual bool mouseMoved(const OIS::MouseEvent &arg) = 0;
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) = 0;
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) = 0;
};

#endif // __INPUTRECEIVER_H_
