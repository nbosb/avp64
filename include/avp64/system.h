/******************************************************************************
 *                                                                            *
 * Copyright 2020 Lukas Jünger                                                *
 *                                                                            *
 * This software is licensed under the MIT license found in the               *
 * LICENSE file at the root directory of this source tree.                    *
 *                                                                            *
 ******************************************************************************/

#ifndef AVP64_SYSTEM_H
#define AVP64_SYSTEM_H

#include "vcml.h"
#include "avp64/arm64_cpu.h"
#include "tutorial/pl011.h"

namespace avp64 {

class system : public vcml::system
{
public:
    // properties
    vcml::property<unsigned int> nrcpu;

    vcml::property<vcml::range> addr_ram;
    vcml::property<vcml::range> addr_gic_cpuif;
    vcml::property<vcml::range> addr_gic_distif;
    vcml::property<vcml::range> addr_gic_vifctrl;
    vcml::property<vcml::range> addr_gic_vcpuif;
    vcml::property<vcml::range> addr_uart;
    vcml::property<vcml::range> addr_ethoc;
    vcml::property<vcml::range> addr_sdhci;
    vcml::property<vcml::range> addr_simdev;
    vcml::property<vcml::range> addr_hwrng;
    vcml::property<vcml::range> addr_spi;
    vcml::property<vcml::range> addr_gpio;

    vcml::property<int> irq_uart;
    vcml::property<int> irq_ethoc;
    vcml::property<int> irq_sdhci;
    vcml::property<int> irq_gt_hyp;
    vcml::property<int> irq_gt_virt;
    vcml::property<int> irq_gt_ns;
    vcml::property<int> irq_gt_s;
    vcml::property<int> irq_spi;

    system(const sc_core::sc_module_name& name);
    system() = delete;
    system(const system&) = delete;
    virtual ~system() = default;

    int run() override;

    virtual void end_of_elaboration() override;

private:
    std::vector<std::shared_ptr<arm64_cpu>> m_cpus;

    vcml::generic::clock m_clock;
    vcml::generic::reset m_reset;

    vcml::generic::bus m_bus;
    vcml::generic::memory m_ram;
    vcml::arm::gic400 m_gic;
    vcml::serial::terminal m_term;
    vcml::opencores::ethoc m_ethoc;
    vcml::ethernet::network m_net;
    vcml::ethernet::bridge m_bridge;
    vcml::sd::sdcard m_sdcard;
    vcml::sd::sdhci m_sdhci;
    vcml::meta::simdev m_simdev;
    vcml::generic::hwrng m_hwrng;
    vcml::spi::bus m_spibus;
    vcml::opencores::ocspi m_spi;
    vcml::generic::gpio m_gpio;
    vcml::spi::max31855 m_max31855;

    /// TODO: define a tutorial::pl011 UART model

    void construct_system_arm64();
};

} // namespace avp64

#endif // AVP64_SYSTEM_H
