
#include "gui/dialogs/SkipDialog.h";

#include "gui/self-voicing/datamodel/DataTree.h"
#include "gui/self-voicing/datamodel/Dialog.h"
#include "gui/self-voicing/datamodel/Switch.h"
#include "gui/self-voicing/datamodel/Label.h"
#include "gui/self-voicing/Prompt.h"
#include "gui/self-voicing/UiMediaTypes.h"

namespace amis
{
namespace gui
{
namespace dialogs
{

class SkipDialogVoicing
{
private:
	amis::gui::dialogs::SkipDialog * mpDialog;
	void appendSeqListContent(AudioSequence* seq, amis::dtb::CustomTest* p_custom_test, int i);

public:
	void OnSelchangeSkips();
	SkipDialogVoicing(amis::gui::dialogs::SkipDialog * dlg);
	~SkipDialogVoicing(void);
	
	static amis::gui::spoken::PromptItem* getPromptItemForReadingOptionName(string item_id);
	 
};
}}}