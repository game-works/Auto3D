#pragma once
#include "UINode.h"

namespace Auto3D
{

class AUTO_API Button : public UINode
{
	REGISTER_OBJECT_CLASS(Button, UINode)
public:
	/// The constructor
	Button();
	/// The destructor
	~Button();
	/// Register factory and attributes.
	static void RegisterObject();

	/// Set text
	void SetText(const char* fmt);
	/// Set text
	void SetText(const String& fmt);

	/// Return button click state
	bool IsClick();
	/// Define node in ui
	void DefineNode()override;
private:
	/// Text string
	String _text;
	/// Is click
	bool _click;
};

}