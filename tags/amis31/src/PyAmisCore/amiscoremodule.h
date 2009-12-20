extern PyObject *ErrorObj_New(amis::Error* itself);
extern int ErrorObj_Convert(PyObject *v, amis::Error* *p_itself);

extern PyObject *MediaNodeObj_New(amis::MediaNode* itself);
extern int MediaNodeObj_Convert(PyObject *v, amis::MediaNode* *p_itself);

extern PyObject *TextNodeObj_New(amis::TextNode* itself);
extern int TextNodeObj_Convert(PyObject *v, amis::TextNode* *p_itself);

extern PyObject *AudioNodeObj_New(amis::AudioNode* itself);
extern int AudioNodeObj_Convert(PyObject *v, amis::AudioNode* *p_itself);

extern PyObject *ImageNodeObj_New(amis::ImageNode* itself);
extern int ImageNodeObj_Convert(PyObject *v, amis::ImageNode* *p_itself);

extern PyObject *MediaGroupObj_New(amis::MediaGroup* itself);
extern int MediaGroupObj_Convert(PyObject *v, amis::MediaGroup* *p_itself);

extern PyObject *ModuleDescDataObj_New(amis::ModuleDescData* itself);
extern int ModuleDescDataObj_Convert(PyObject *v, amis::ModuleDescData* *p_itself);

extern PyObject *RecentBookEntryObj_New(amis::RecentBookEntry* itself);
extern int RecentBookEntryObj_Convert(PyObject *v, amis::RecentBookEntry* *p_itself);

extern PyObject *RecentBooksObj_New(amis::RecentBooks* itself);
extern int RecentBooksObj_Convert(PyObject *v, amis::RecentBooks* *p_itself);

extern PyObject *PositionDataObj_New(amis::dtb::PositionData* itself);
extern int PositionDataObj_Convert(PyObject *v, amis::dtb::PositionData* *p_itself);

extern PyObject *PositionMarkObj_New(amis::dtb::PositionMark* itself);
extern int PositionMarkObj_Convert(PyObject *v, amis::dtb::PositionMark* *p_itself);

extern PyObject *BookmarkObj_New(amis::dtb::Bookmark* itself);
extern int BookmarkObj_Convert(PyObject *v, amis::dtb::Bookmark* *p_itself);

extern PyObject *HiliteObj_New(amis::dtb::Hilite* itself);
extern int HiliteObj_Convert(PyObject *v, amis::dtb::Hilite* *p_itself);

extern PyObject *BookmarkSetObj_New(amis::dtb::BookmarkSet* itself);
extern int BookmarkSetObj_Convert(PyObject *v, amis::dtb::BookmarkSet* *p_itself);

extern PyObject *CustomTestObj_New(amis::dtb::CustomTest* itself);
extern int CustomTestObj_Convert(PyObject *v, amis::dtb::CustomTest* *p_itself);

extern PyObject *CustomTestSetObj_New(amis::dtb::CustomTestSet* itself);
extern int CustomTestSetObj_Convert(PyObject *v, amis::dtb::CustomTestSet* *p_itself);

extern PyObject *DtbObj_New(amis::dtb::Dtb* itself);
extern int DtbObj_Convert(PyObject *v, amis::dtb::Dtb* *p_itself);

extern PyObject *DtbFileSetObj_New(amis::dtb::DtbFileSet* itself);
extern int DtbFileSetObj_Convert(PyObject *v, amis::dtb::DtbFileSet* *p_itself);

extern PyObject *MetaItemObj_New(amis::dtb::MetaItem* itself);
extern int MetaItemObj_Convert(PyObject *v, amis::dtb::MetaItem* *p_itself);

extern PyObject *MetadataObj_New(amis::dtb::Metadata* itself);
extern int MetadataObj_Convert(PyObject *v, amis::dtb::Metadata* *p_itself);

extern PyObject *SpineObj_New(amis::dtb::Spine* itself);
extern int SpineObj_Convert(PyObject *v, amis::dtb::Spine* *p_itself);

extern PyObject *NavContainerObj_New(amis::dtb::nav::NavContainer* itself);
extern int NavContainerObj_Convert(PyObject *v, amis::dtb::nav::NavContainer* *p_itself);

extern PyObject *NavListObj_New(amis::dtb::nav::NavList* itself);
extern int NavListObj_Convert(PyObject *v, amis::dtb::nav::NavList* *p_itself);

extern PyObject *NavMapObj_New(amis::dtb::nav::NavMap* itself);
extern int NavMapObj_Convert(PyObject *v, amis::dtb::nav::NavMap* *p_itself);

extern PyObject *NavModelObj_New(amis::dtb::nav::NavModel* itself);
extern int NavModelObj_Convert(PyObject *v, amis::dtb::nav::NavModel* *p_itself);

extern PyObject *NavNodeObj_New(amis::dtb::nav::NavNode* itself);
extern int NavNodeObj_Convert(PyObject *v, amis::dtb::nav::NavNode* *p_itself);

extern PyObject *NavPointObj_New(amis::dtb::nav::NavPoint* itself);
extern int NavPointObj_Convert(PyObject *v, amis::dtb::nav::NavPoint* *p_itself);

extern PyObject *NavTargetObj_New(amis::dtb::nav::NavTarget* itself);
extern int NavTargetObj_Convert(PyObject *v, amis::dtb::nav::NavTarget* *p_itself);

extern PyObject *PageListObj_New(amis::dtb::nav::PageList* itself);
extern int PageListObj_Convert(PyObject *v, amis::dtb::nav::PageList* *p_itself);

extern PyObject *PageTargetObj_New(amis::dtb::nav::PageTarget* itself);
extern int PageTargetObj_Convert(PyObject *v, amis::dtb::nav::PageTarget* *p_itself);

extern PyObject *SmilMediaGroupObj_New(amis::dtb::smil::SmilMediaGroup* itself);
extern int SmilMediaGroupObj_Convert(PyObject *v, amis::dtb::smil::SmilMediaGroup* *p_itself);

extern PyObject *SmilTreeObj_New(amis::dtb::smil::SmilTree* itself);
extern int SmilTreeObj_Convert(PyObject *v, amis::dtb::smil::SmilTree* *p_itself);

extern PyObject *ModuleDescReaderObj_New(amis::io::ModuleDescReader* itself);
extern int ModuleDescReaderObj_Convert(PyObject *v, amis::io::ModuleDescReader* *p_itself);

