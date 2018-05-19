#include "LabelBoxList.hpp"
#include "Mathf.hpp"

START_NAMESPACE_DISTRHO

LabelBoxList::LabelBoxList(NanoWidget *widget, Size<uint> size) noexcept : LabelContainer(widget, size),
                                                                           fLabelBox(widget, size)
{
    setSize(size);
}

void LabelBoxList::onNanoDisplay()
{
    fLabelBox.setText(getLabels()[getSelectedIndex()]);
    fLabelBox.setAbsolutePos(getAbsolutePos());
}

END_NAMESPACE_DISTRHO