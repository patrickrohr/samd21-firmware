// Copyright 2019, Patrick Rohr

#include "clock/clock_source_generic.hpp"
#include <cmath>

namespace SAMD
{

static auto reg_GCLK    = MakeRegisterGuard(GCLK);
static auto reg_CLKCTRL = MakeRegisterGuard(&reg_GCLK->Get().CLKCTRL);
static auto reg_GENCTRL = MakeRegisterGuard(&reg_GCLK->Get().GENCTRL);
static auto reg_GENDIV  = MakeRegisterGuard(&reg_GCLK->Get().GENDIV);

ClockSourceGeneric::ClockSourceGeneric(gclk_id_t id) : m_uGclkId(id)
{
    static constexpr gclk_id_t g_uGclkIdMax(9);
    samd_assert(m_uGclkId < g_uGclkIdMax, "GCLK ID out of range: %u", uGclkId);
}

ClockSourceGeneric::~ClockSourceGeneric()
{
    bool isEnabled = IsEnabled();
    samd_assert(
        isEnabled,
        "Clock was never enabled: %u",
        static_cast<unsigned>(uGclkId));

    if (isEnabled)
    {
        Disable();
    }
}

error_t RegisterSync()
{
    // TODO: Protect register for simulation purposes
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
    return 0;
}

error_t ClockSourceGeneric::Enable(uint32_t uDivisionFactor)
{
    RegisterGuard<GCLK_GENDIV_Type> tmp_GENDIV;
    tmp_GENDIV.Get().bit.ID  = static_cast<uint8_t>(m_uGclkId);
    tmp_GENDIV.Get().bit.DIV = uDivisionFactor;

    *reg_GENDIV = tmp_GENDIV;
    RegisterSync();

    // Write first byte of GEN CTRL before reading
    // TODO: Could Register_Guard provide support to do this directly?
    *reinterpret_cast<volatile uint8_t*>(reg_GENCTRL) =
        static_cast<uint8_t>(m_uGclkId);
    RegisterGuard<GCLK_GENCTRL_Type> tmp_GENCTRL(*reg_GENCTRL);

    tmp_GENCTRL.Get().bit.GENEN = 1;
    tmp_GENCTRL.Get().bit.SRC   = static_cast<uint32_t>(GetClockSourceType());

    *reg_GENCTRL = tmp_GENCTRL;
    RegisterSync();

    return 0;
}

error_t ClockSourceGeneric::Disable()
{
    *reinterpret_cast<volatile uint8_t*>(reg_GENCTRL) =
        static_cast<uint8_t>(m_uGclkId);
    RegisterGuard<GCLK_GENCTRL_Type> tmp_GENCTRL(*reg_GENCTRL);

    tmp_GENCTRL.Get().bit.GENEN = 0;
    *reg_GENCTRL               = tmp_GENCTRL;

    RegisterSync();

    return 0;
}

bool ClockSourceGeneric::IsEnabled() const
{
    // check hardware
    *reinterpret_cast<volatile uint8_t*>(reg_GENCTRL) =
        static_cast<uint8_t>(m_uGclkId);
    RegisterGuard<GCLK_GENCTRL_Type> tmp_GENCTRL(*reg_GENCTRL);

    return tmp_GENCTRL.Get().bit.GENEN;
}

void ClockSourceGeneric::AddOutput(ClockOutput eOutput)
{
    SetOutput(eOutput, true);
}


void ClockSourceGeneric::RemoveOutput(ClockOutput eOutput)
{
    SetOutput(eOutput, false);
}

void ClockSourceGeneric::SetOutput(ClockOutput eOutput, bool enable)
{
    samd_assert(
        static_cast<unsigned>(eOutput) <
        static_cast<unsigned>(ClockOutput::eCount),
        "Invalid ClockOutput %u", static_cast<unsigned>(eOutput));

    RegisterGuard<GCLK_CLKCTRL_Type> tmp_CLKCTRL;

    tmp_CLKCTRL.Get().bit.ID = m_uGclkId.Get();
    tmp_CLKCTRL.Get().bit.GEN = static_cast<unsigned>(eOutput);
    tmp_CLKCTRL.Get().bit.CLKEN = static_cast<unsigned>(enable);

    *reg_CLKCTRL = tmp_CLKCTRL;
    RegisterSync();
}

error_t ClockSourceGeneric::WaitOnClockIsRunning() const
{
    while (!PollIsRunning()) {}
    return 0;
}

unsigned ClockSourceGeneric::GetDivisionFactor() const
{
    *reinterpret_cast<volatile uint8_t*>(reg_GENDIV) =
        static_cast<uint8_t>(m_uGclkId);
    RegisterGuard<GCLK_GENDIV_Type> tmp_GENDIV(*reg_GENDIV);

    *reinterpret_cast<volatile uint8_t*>(reg_GENCTRL) =
        static_cast<uint8_t>(m_uGclkId);
    RegisterGuard<GCLK_GENCTRL_Type> tmp_GENCTRL(*reg_GENCTRL);

    uint32_t uDivisionFactor = tmp_GENDIV.Get().bit.DIV;

    enum class DivisionSelector
    {
        eLinear      = 0,
        eExponential = 1
    };

    DivisionSelector eDivisionSelector =
        static_cast<DivisionSelector>(tmp_GENCTRL.Get().bit.DIVSEL);

    if (DivisionSelector::eExponential == eDivisionSelector)
    {
        uDivisionFactor = std::pow(2, uDivisionFactor + 1);
    }
    else if (uDivisionFactor == 0)
    {
        // Division Factor of 0 is equivalent to 1
        uDivisionFactor = 1;
    }

    samd_assert(uDivisionFactor > 0, "Division Factor cannot be 0!");
    return uDivisionFactor;
}

frequency_t ClockSourceGeneric::GetOutputFrequency() const
{
    // Division factor never returns 0.
    frequency_t result = GetFrequency() / GetDivisionFactor();
    samd_assert(
        result > 0, "Output frequency must be larger than 0: %u", result);
    return result;
}

} // namespace SAMD
