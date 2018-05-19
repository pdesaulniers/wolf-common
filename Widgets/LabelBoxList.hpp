#ifndef WOLF_LABEL_BOX_LIST_HPP_INCLUDED
#define WOLF_LABEL_BOX_LIST_HPP_INCLUDED

#include "Widget.hpp"
#include "NanoVG.hpp"
#include "Layout.hpp"
#include "LabelBox.hpp"
#include "LabelContainer.hpp"

START_NAMESPACE_DISTRHO

class LabelBoxList : public LabelContainer
{
  public:
    explicit LabelBoxList(NanoWidget *widget, Size<uint> size) noexcept;

  protected:
    void onNanoDisplay() override;

  private:
    LabelBox fLabelBox;

    DISTRHO_LEAK_DETECTOR(LabelBoxList)
};

END_NAMESPACE_DISTRHO

#endif