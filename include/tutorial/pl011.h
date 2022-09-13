
#ifndef TUTORIAL_PL011_H
#define TUTORIAL_PL011_H

#include "vcml.h"
#include "vcml/protocols/gpio.h"
#include "vcml/protocols/serial.h"
#include "vcml/protocols/tlm_sockets.h"

namespace tutorial {

class pl011 : public vcml::peripheral, public vcml::serial_host
{
private:
    unsigned int m_fifo_size;
    std::queue<vcml::u16> m_fifo;

    // from vcml::serial_host
    virtual void serial_receive(vcml::u8 data) override;

    // update flags and interrupts
    void update();

    vcml::u16 read_dr();
    void write_dr(vcml::u16 val);
    void write_rsr(vcml::u8 val);
    void write_ibrd(vcml::u16 val);
    void write_fbrd(vcml::u16 val);
    void write_lcr(vcml::u8 val);
    void write_cr(vcml::u16 val);
    void write_ifls(vcml::u16 val);
    void write_imsc(vcml::u16 val);
    void write_icr(vcml::u16 val);

public:
    enum amba_ids : vcml::u32 {
        AMBA_PID = 0x00141011, // Peripheral ID
        AMBA_CID = 0xb105f00d, // PrimeCell ID
    };

    enum : vcml::u32 {
        FIFOSIZE = 16, // FIFO size
    };

    enum dr_bits : vcml::u16 {
        DR_FE = 1 << 8,  // Framing Error
        DR_PE = 1 << 9,  // Parity Error
        DR_BE = 1 << 10, // Break Error
        DR_OE = 1 << 11, // Overrun Error
    };

    enum rsr_bits : vcml::u32 {
        RSR_O = 0x8, // RSR Flags Offset
        RSR_M = 0xf, // RSR Flags Mask
    };

    enum fr_bits : vcml::u16 {
        FR_CTS = 1 << 0,  // Clear To Send
        FR_DSR = 1 << 1,  // Data Set Ready
        FR_DCD = 1 << 2,  // Data Carrier Detect
        FR_BUSY = 1 << 3, // Busy/Transmitting
        FR_RXFE = 1 << 4, // Receive FIFO Empty
        FR_TXFF = 1 << 5, // Transmit FIFO FULL
        FR_RXFF = 1 << 6, // Receive FIFO Full
        FR_TXFE = 1 << 7, // Transmit FIFO Empty
        FR_RI = 1 << 8    // Ring Indicator
    };

    enum ris_bits : vcml::u32 {
        RIS_RX = 1 << 4,  // Receive Interrupt Status
        RIS_TX = 1 << 5,  // Transmit Interrupt Status
        RIS_RT = 1 << 6,  // Receive Timeout Interrupt Status
        RIS_FE = 1 << 7,  // Framing Error Interrupt Status
        RIS_PE = 1 << 8,  // Parity Error Interrupt Status
        RIS_BE = 1 << 9,  // Break Error Interrupt Status
        RIS_OE = 1 << 10, // Overrun Error Interrupt Status
        RIS_M = 0x7f      // Raw Interrupt Status Mask
    };

    enum lcr_bits : vcml::u32 {
        LCR_BRK = 1 << 0,    // Send Break
        LCR_PEN = 1 << 1,    // Parity Enable
        LCR_EPS = 1 << 2,    // Even Parity Select
        LCR_STP2 = 1 << 3,   // Two Stop Bits Select
        LCR_FEN = 1 << 4,    // FIFO Enable
        LCR_WLEN = 3 << 5,   // Word Length
        LCR_SPS = 1 << 7,    // Stick Parity Select
        LCR_IBRD_M = 0xffff, // Integer Baud Rate Divider Mask
        LCR_FBRD_M = 0x003f, // Fraction Baud Raid Divider Mask
        LCR_H_M = 0xff       // LCR Header Mask
    };

    enum cr_bits {
        CR_UARTEN = 1 << 0, // UART Enable
        CR_TXE = 1 << 8,    // Transmit Enable
        CR_RXE = 1 << 9     // Receive Enable
    };

    vcml::reg<vcml::u16> dr;   // Data Register
    vcml::reg<vcml::u8> rsr;   // Receive Status Register
    vcml::reg<vcml::u16> fr;   // Flag Register
    vcml::reg<vcml::u8> ilpr;  // IrDA Low-Power Counter Register
    vcml::reg<vcml::u16> ibrd; // Integer Baud Rate Register
    vcml::reg<vcml::u16> fbrd; // Fractional Baud Rate Register
    vcml::reg<vcml::u8> lcr;   // Line Control Register
    vcml::reg<vcml::u16> cr;   // Control Register
    vcml::reg<vcml::u16> ifls; // Interrupt FIFO Level select
    vcml::reg<vcml::u16> imsc; // Interrupt Mask Set/Clear Register
    vcml::reg<vcml::u16> ris;  // Raw Interrupt Status
    vcml::reg<vcml::u16> mis;  // Masked Interrupt Status
    vcml::reg<vcml::u16> icr;  // Interrupt Clear Register
    vcml::reg<vcml::u16> dmac; // DMA Control

    vcml::reg<vcml::u32, 4> pid; // Peripheral ID Register
    vcml::reg<vcml::u32, 4> cid; // Cell ID Register

    vcml::tlm_target_socket in;
    vcml::gpio_initiator_socket irq;

    vcml::serial_initiator_socket serial_tx;
    vcml::serial_target_socket serial_rx;

    explicit pl011(const sc_core::sc_module_name& name);
    pl011() = delete;
    pl011(const pl011&) = delete;
    virtual ~pl011() = default;

    bool is_enabled() const { return cr & CR_UARTEN; }
    bool is_rx_enabled() const { return cr & CR_RXE; }
    bool is_tx_enabled() const { return cr & CR_TXE; }

    // from vcml::peripheral
    virtual void reset() override;
};

} // namespace tutorial

#endif
